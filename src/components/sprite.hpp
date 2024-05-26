#pragma once
#include <cstdint>
#include <entt.hpp>
#include <raylib.h>
#include "velocity.hpp"
#include "../assets/asset_manager.hpp"
#include "common.hpp"

namespace bh {
struct Sprite {
    Texture2D texture;
    uint16_t cell_size_x;
    uint16_t cell_size_y;
    uint16_t sprite_id;

    [[nodiscard]] Rectangle rect() const {
        auto columns_in_texture = texture.width / cell_size_x;

        auto row = sprite_id / columns_in_texture;
        auto column = sprite_id % columns_in_texture;

        return Rectangle { .x = (float)(column * cell_size_x), .y = (float)(row * cell_size_y), .width = (float)cell_size_x, .height = (float)cell_size_y };
    }
};

struct Visible {};

template <>
inline void emplace<Sprite, TextureEnum>(entt::registry &registry, entt::entity entity, const TextureEnum &id) {
    emplace<Transform>(registry, entity);
    emplace<Visible>(registry, entity);
    auto texture = registry.ctx().get<AssetManager>().get_texture(id);
    registry.emplace<Sprite>(entity, texture, texture.width, texture.height, 0);
}

template <>
inline void
emplace<Sprite, TextureEnum, uint16_t, uint16_t, uint16_t>(entt::registry &registry, entt::entity entity,
                                                           const TextureEnum &id, const uint16_t &cell_size_x,
                                                           const uint16_t &cell_size_y, const uint16_t &sprite_id) {
    emplace<Transform>(registry, entity);
    emplace<Visible>(registry, entity);
    auto texture = registry.ctx().get<AssetManager>().get_texture(id);
    registry.emplace<Sprite>(entity, texture, cell_size_x, cell_size_y, sprite_id);
}

inline void render_sprites(entt::registry &registry) {
    auto sprite_group = registry.view<Sprite, Transform, Visible>();
    for (auto &&[entity, sprite, transform] : sprite_group.each()) {
        DrawTextureRec(sprite.texture, sprite.rect(), transform.position, WHITE);
    }
}
} // namespace bh
