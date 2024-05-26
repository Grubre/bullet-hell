#pragma once

#include <fmt/printf.h>
#include <imgui.h>
#include <entt.hpp>
#include <optional>

namespace bh {
struct DebugName {
    std::string name;
};

template <typename T>
concept InspectableComponent = requires(T t, entt::registry &registry, entt::entity entity) {
    { T::name } -> std::convertible_to<const char *>;
    { t.inspect(registry, entity) };
};

template <InspectableComponent... Component> struct Inspector {
    static constexpr auto EntityTypeName = "Entity";
    explicit Inspector(entt::registry *registry) : registry(registry) {}

    std::optional<entt::entity> current_entity;
    void draw_gui() {
        ImGui::Begin("Inspector");

        if (!current_entity) {
            ImGui::Text("No entity selected");
            ImGui::End();
            return;
        }

        display_entities();
        ImGui::SameLine();
        display_components();

        ImGui::End();
    }

  private:
    void display_entities() {
        ImGui::BeginChild("Entity list", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0), 1);
        ImGui::SeparatorText("Toggle components");
        auto i = 0u;
        ([&]() { ImGui::Checkbox(Component::name, &component_filter[i++]); }(), ...);

        ImGui::SeparatorText("List");
        for (auto entity : registry->view<entt::entity>()) {
            i = 0u;
            bool has_all_components =
                ([&]() { return registry->all_of<Component>(entity) == component_filter[i++]; }() && ...);

            if (!has_all_components) {
                continue;
            }

            auto *const name = registry->try_get<DebugName>(entity);

            ImGui::Text("%d (%s)", (int)entity, name != nullptr ? name->name.c_str() : "unknown");
            ImGui::PushID((int)entity);
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload(EntityTypeName, &entity, sizeof(entt::entity));
                ImGui::Text("%d (%s)", (int)entity, name != nullptr ? name->name.c_str() : "unknown");
                ImGui::EndDragDropSource();
            }
            ImGui::PopID();
        }
        ImGui::EndChild();
    }

    void display_components() {
        entt::entity entity = *current_entity;

        ImGui::BeginChild("Entity Inspector", ImVec2(ImGui::GetContentRegionAvail().x, 0), 1);

        ImGui::SeparatorText("Entity data");
        ImGui::PushID((int)entity);
        ImGui::BeginGroup();
        ImGui::Text("Entity: %d", (int)entity);
        if (auto *name = registry->try_get<DebugName>(entity)) {
            ImGui::Text("Name: %s", name->name.c_str());
        }
        ImGui::EndGroup();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(EntityTypeName)) {
                current_entity = *(entt::entity *)payload->Data;
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopID();

        ImGui::SeparatorText("Components");
        (
            [&]() {
                if (!registry->all_of<Component>(entity)) {
                    return;
                }
                if (ImGui::CollapsingHeader(Component::name)) {
                    auto &component = registry->get<Component>(entity);
                    component.inspect(*registry, entity);
                }
            }(),
            ...);
        ImGui::EndChild();
    }

    entt::registry *registry;
    std::array<bool, sizeof...(Component)> component_filter{};
};

} // namespace bh
