#pragma once

#include <fmt/printf.h>
#include <imgui.h>
#include <entt.hpp>
#include <imgui_internal.h>
#include <optional>

namespace bh {
struct DebugName {
    std::string name;
};

template <typename T>
concept InspectableComponent = requires(T t, entt::registry &registry, entt::entity entity) {
    { T::name } -> std::convertible_to<const char *>;
    { t.inspect(registry, entity) };
} || requires() {
    { T::name } -> std::convertible_to<const char *>;
    { T::inspect() };
    { std::is_empty_v<T> };
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

        display_entity_list();
        ImGui::SameLine();
        display_entity_info();

        ImGui::End();
    }

  private:
    void display_entity_list() {
        ImGui::BeginChild("Entity list", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0), 1);
        ImGui::SeparatorText("Toggle components");

        auto i = 0u;
        ([&]() { ImGui::Checkbox(Component::name, &component_filter[i++]); }(), ...);

        ImGui::SeparatorText("List");
        for (auto entity : registry->view<entt::entity>()) {
            i = 0u;
            bool has_toggled_components =
                ([&]() { return !component_filter[i++] || registry->all_of<Component>(entity); }() && ...);

            if (!has_toggled_components) {
                continue;
            }

            display_entity_list_entry(entity);
        }

        ImGui::EndChild();
    }

    void display_entity_list_entry(entt::entity entity) {
        auto *const name = registry->try_get<DebugName>(entity);

        ImGui::PushID((int)entity);
        const auto text = fmt::format("{} ({})", (int)entity, name != nullptr ? name->name : "unknown");

        if (ImGui::Selectable(text.c_str(), current_entity == entity, ImGuiSelectableFlags_SelectOnClick)) {
            if (ImGui::IsMouseClicked(0)) {
                current_entity = entity;
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload(EntityTypeName, &entity, sizeof(entt::entity));
            ImGui::Text(text.c_str());
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::SeparatorText(text.c_str());
            iterate_components([&]<InspectableComponent Comp>() {
                if (!registry->all_of<Comp>(entity)) {
                    return;
                }
                ImGui::Text(Comp::name);
            });
            ImGui::EndTooltip();
        }
    }

    void iterate_components(auto &&f) { (f.template operator()<Component>(), ...); }

    void display_entity_info() {
        entt::entity entity = *current_entity;

        ImGui::BeginChild("Entity Inspector", ImVec2(ImGui::GetContentRegionAvail().x, 0), 1);
        ImGui::PushID((int)entity);
        ImGui::SeparatorText("Entity data");

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

        display_components(entity);

        ImGui::PopID();
        ImGui::EndChild();
    }

    void display_components(entt::entity entity) {
        ImGui::SeparatorText("Components");
        iterate_components([&]<InspectableComponent Comp>() {
            if (!registry->all_of<Comp>(entity)) {
                return;
            }
            if (ImGui::CollapsingHeader(Comp::name)) {
                if constexpr (std::is_empty_v<Comp>) {
                    Comp::inspect();
                } else {
                    auto &component = registry->get<Comp>(entity);
                    component.inspect(*registry, entity);
                }
            }
        });
    }

    entt::registry *registry;
    std::array<bool, sizeof...(Component)> component_filter{};
};

} // namespace bh
