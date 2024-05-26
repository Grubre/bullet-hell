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
        // get parent width
        ImGui::BeginChild("Entities list", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0), 1);
        for (auto entity : registry->view<entt::entity>()) {
            auto name = registry->try_get<DebugName>(entity);

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

        ImGui::PushID((int)entity);
        ImGui::Text("Entity: %d", (int)entity);
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(EntityTypeName)) {
                current_entity = *(entt::entity *)payload->Data;
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::PopID();

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
};

} // namespace bh
