#pragma once
#include <raylib.h>
#include <entt.hpp>
#include "velocity.hpp"
#include "../assets/asset_manager.hpp"
#include "common.hpp"
namespace bh {
struct Sprite {
    Texture2D texture;
};

struct Visible {};

template <>
inline void emplace<Sprite, TextureEnum>(entt::registry &registry, entt::entity entity, const TextureEnum &id) {
    emplace<Transform>(registry, entity);
    emplace<Visible>(registry, entity);
    auto texture = registry.ctx().get<AssetManager>().get_texture(id);
    registry.emplace<Sprite>(entity, texture);
}

inline void render_sprites(entt::registry &registry) {
    auto sprite_group = registry.view<Sprite, Transform, Visible>();
    for (auto &&[entity, sprite, transform] : sprite_group.each()) {
        DrawTexture(sprite.texture, transform.position.x, transform.position.y, WHITE);
    }
}
} // namespace bh
