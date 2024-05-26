#pragma once

#include <raylib.h>
#include <entt.hpp>
#include "../asset_manager.hpp"
#include "common.hpp"

namespace bh {
struct Transform {
    Vector2 position;
};

using Velocity = Vector2;
template <> inline void emplace<Velocity>(entt::registry &registry, entt::entity entity) {
    emplace<Transform>(registry, entity);
    registry.emplace<Velocity>(entity, Vector2{0, 0});
}

} // namespace bh
