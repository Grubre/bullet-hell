#pragma once
#include <cstdint>
#include <entt.hpp>
#include <raylib.h>
#include <imgui.h>
#include "velocity.hpp"
#include "../assets/asset_manager.hpp"
#include "common.hpp"

namespace bh {

struct Visible {};

struct Sprite {
    TextureAsset asset;
    static constexpr auto name = "Sprite";
    Texture2D texture;
    uint16_t cell_size_x;
    uint16_t cell_size_y;
    uint16_t sprite_id;

    [[nodiscard]] Rectangle rect() const {
        auto columns_in_texture = asset.texture.width / asset.cell_size_x;

        auto row = sprite_id / columns_in_texture;
        auto column = sprite_id % columns_in_texture;

        return Rectangle{.x = (float)(column * cell_size_x),
                         .y = (float)(row * cell_size_y),
                         .width = (float)cell_size_x,
                         .height = (float)cell_size_y};
    }

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Text("Sprite");
        ImGui::DragScalar("Cell size x", ImGuiDataType_U16, &cell_size_x, 1.0f);
        ImGui::DragScalar("Cell size y", ImGuiDataType_U16, &cell_size_y, 1.0f);
        ImGui::DragScalar("Sprite id", ImGuiDataType_U16, &sprite_id, 1.0f);
    }
};

template <>
inline void emplace<Sprite, TextureEnum>(entt::registry &registry, entt::entity entity, const TextureEnum &id) {
    emplace<LocalTransform>(registry, entity);
    emplace<Visible>(registry, entity);
    auto texture = registry.ctx().get<AssetManager>().get_texture(id);
    registry.emplace<Sprite>(entity, texture, 0);
}

inline void render_sprites(entt::registry &registry) {
    auto sprite_group = registry.view<Sprite, GlobalTransform, Visible>();
    for (auto &&[entity, sprite, transform] : sprite_group.each()) {
        DrawTextureRec(sprite.asset.texture, sprite.rect(), transform.transform.position, WHITE);
    }
}
} // namespace bh
