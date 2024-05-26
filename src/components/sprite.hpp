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

    uint16_t sprite_id;

    [[nodiscard]] Rectangle rect() const {
        auto columns_in_texture = asset.texture.width / asset.cell_size_x;

        auto row = sprite_id / columns_in_texture;
        auto column = sprite_id % columns_in_texture;

        return Rectangle{.x = static_cast<float>(column * asset.cell_size_x),
                         .y = static_cast<float>(row * asset.cell_size_y),
                         .width = static_cast<float>(asset.cell_size_x),
                         .height = static_cast<float>(asset.cell_size_y)};
    }

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        static constexpr auto minimum = 1;
        ImGui::Text("Sprite");
        ImGui::DragScalar("Cell size x", ImGuiDataType_U16, &asset.cell_size_x, 1.0f, &minimum, &asset.texture.width);
        ImGui::DragScalar("Cell size y", ImGuiDataType_U16, &asset.cell_size_y, 1.0f, &minimum, &asset.texture.height);
        ImGui::DragScalar("Sprite id", ImGuiDataType_U16, &sprite_id, 1.0f);
    }
};

template <>
inline void emplace<Sprite, TextureEnum>(entt::registry &registry, entt::entity entity, const TextureEnum &id) {
    emplace<GlobalTransform>(registry, entity);
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
