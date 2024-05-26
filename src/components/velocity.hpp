#pragma once

#include <raylib.h>
#include <raymath.h>
#include <entt.hpp>
#include "common.hpp"

namespace bh {

struct Transform {
    Vector2 position;

    Transform combine(Transform& other) {
        return Transform { Vector2Add(position, other.position) };
    }
};

struct LocalTransform {
    Transform transform;
};

struct GlobalTransform {
    Transform transform;
};

using Velocity = Vector2;
template <> inline void emplace<Velocity>(entt::registry &registry, entt::entity entity) {
    emplace<LocalTransform>(registry, entity);
    registry.emplace<Velocity>(entity, Vector2{0, 0});
}

} // namespace bh
