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

    Transform combine(Transform& other) const {
        return Transform { Vector2Add(position, other.position) };
    }

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat2("Transform", &position.x, 1.0f);
    }
};

struct LocalTransform {
    Transform transform;

    static constexpr auto name = "Local Transform";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        transform.inspect(registry, entity);
    }
};

struct GlobalTransform {
    Transform transform;

    static constexpr auto name = "Global Transform";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        transform.inspect(registry, entity);
    }
};

template <> inline void emplace<LocalTransform>(entt::registry &registry, entt::entity entity) {
    registry.emplace<GlobalTransform>(entity);
    registry.emplace<LocalTransform>(entity);
}

template <> inline void emplace<GlobalTransform>(entt::registry &registry, entt::entity entity) {
    registry.emplace<GlobalTransform>(entity);
    registry.emplace<LocalTransform>(entity);
}

using Velocity = Vector2;
template <> inline void emplace<Velocity>(entt::registry &registry, entt::entity entity) {
    emplace<LocalTransform>(registry, entity);
    registry.emplace<Velocity>(entity, Vector2{0, 0});
}

} // namespace bh
