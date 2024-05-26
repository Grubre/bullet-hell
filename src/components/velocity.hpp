#pragma once

#include <imgui.h>
#include <raylib.h>
#include <entt.hpp>
#include "common.hpp"

namespace bh {
struct Transform {
    static constexpr auto name = "Transform";
    Vector2 position;

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat2("Transform", &position.x, 1.0f);
    }
};

using Velocity = Vector2;
template <> inline void emplace<Velocity>(entt::registry &registry, entt::entity entity) {
    emplace<Transform>(registry, entity);
    registry.emplace<Velocity>(entity, Vector2{0, 0});
}

} // namespace bh
