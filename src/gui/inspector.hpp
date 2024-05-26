#pragma once

#include <fmt/printf.h>
#include <imgui.h>
#include <entt.hpp>
#include <optional>

namespace bh {
template <typename T>
concept InspectableComponent = requires(entt::registry &registry, entt::entity entity) {
    { T::inspect(registry, entity) };
};

template <InspectableComponent... Component> struct Inspector {
    explicit Inspector(entt::registry *registry) : registry(registry) {}
    void draw_gui() {
        ImGui::Begin("Inspector");

        if (current_entity) {
            gui_component();
        } else {
            ImGui::Text("No entity selected");
        }

        ImGui::End();
    }

    void gui_component() {
        entt::entity entity = *current_entity;
        ImGui::Text("Entity: %d", (int)entity);
        (
            [&]() {
                if (!registry->all_of<Component>(entity)) {
                    return;
                }
                if (ImGui::CollapsingHeader(Component::name)) {
                    Component::inspect(*registry, entity);
                }
            }(),
            ...);
    }

    std::optional<entt::entity> current_entity;

  private:
    entt::registry *registry;
};

} // namespace bh
