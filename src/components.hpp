#pragma once

#include <raylib.h>
#include <entt.hpp>
#include "asset_manager.hpp"
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

struct Visible {};

struct Sprite {
    Texture2D texture;
};

inline void emplace_sprite(entt::registry &registry, entt::entity entity, TextureEnum id) {
    emplace<Transform>(registry, entity);
    auto texture = registry.ctx().get<AssetManager>().get_texture(id);
    registry.emplace<Sprite>(entity, texture);
}

inline void render_sprites(entt::registry &registry) {
    auto sprite_group = registry.view<Sprite, Transform, Visible>();
    for (auto &&[entity, sprite, transform] : sprite_group.each()) {
        DrawTexture(sprite.texture, transform.position.x, transform.position.y, BLACK);
    }
}

} // namespace bh
