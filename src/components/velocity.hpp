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
    float rotation;
    Vector2 scale = Vector2(1.0f,1.0f);

    /// Aplikuje najpierw swój transform, a dopiero potem other
    [[nodiscard]] Transform combine(const Transform& other) const {
        auto transformed_local = transform_point(other.position);
        return Transform { transformed_local, rotation + other.rotation, Vector2Multiply(scale, other.scale) };
    }

    [[nodiscard]] Vector2 transform_point(const Vector2& point) const {
        auto scaled = Vector2Multiply(point, scale);
        auto rotated = Vector2Rotate(scaled, rotation);
        auto translated = Vector2Add(rotated, position);

        return translated;
    }

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat2("Transform", &position.x, 1.0f);
        ImGui::DragFloat("Rotation", &rotation, 0.05f);
        ImGui::DragFloat2("Scale", &scale.x, 0.5f);
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
