#pragma once

#include <imgui.h>
#include <raylib.h>
#include <entt.hpp>
#include "common.hpp"

namespace bh {
struct Transform {
    static constexpr auto name = "Transform";
    Vector2 position;

    static void inspect(entt::registry &registry, entt::entity entity) {
        auto &transform = registry.get<Transform>(entity);
        ImGui::DragFloat2("Transform", &transform.position.x, 1.0f);
    }
};

using Velocity = Vector2;
template <> inline void emplace<Velocity>(entt::registry &registry, entt::entity entity) {
    emplace<Transform>(registry, entity);
    registry.emplace<Velocity>(entity, Vector2{0, 0});
}

} // namespace bh
