#pragma once

#include <imgui.h>
#include <raylib.h>
#include <raymath.h>
#include <entt.hpp>
#include "common.hpp"

namespace bh {

struct Transform {
    static constexpr auto name = "Transform";
    Vector2 position;

    Transform combine(Transform& other) {
        return Transform { Vector2Add(position, other.position) };
    }
};

struct LocalTransform {
    Transform transform;

    static void inspect(entt::registry &registry, entt::entity entity) {
        auto &transform = registry.get<Transform>(entity);
        ImGui::DragFloat2("Transform", &transform.position.x, 1.0f);
    }
};

struct GlobalTransform {
    Transform transform;

    static void inspect(entt::registry &registry, entt::entity entity) {
        auto &transform = registry.get<Transform>(entity);
        ImGui::DragFloat2("Transform", &transform.position.x, 1.0f);
    }
};

using Velocity = Vector2;
template <> inline void emplace<Velocity>(entt::registry &registry, entt::entity entity) {
    emplace<LocalTransform>(registry, entity);
    registry.emplace<Velocity>(entity, Vector2{0, 0});
}

} // namespace bh
