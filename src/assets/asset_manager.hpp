#pragma once
#include <cstdint>
#include <raylib.h>
#include <array>
#include <entt.hpp>

namespace bh {

enum class TextureEnum { BULLET_TEXTURE = 0, PLAYER_TEXTURE, CNT };

struct TextureAsset {
  Texture2D texture;
  uint16_t cell_size_x;
  uint16_t cell_size_y;
};

class AssetManager {
  public:
    void register_texture(const Image &image, TextureEnum id);
    void register_texture(const Image &image, TextureEnum id, uint16_t cell_size_x, uint16_t cell_size_y);
    auto get_texture(TextureEnum id) -> TextureAsset;

  private:
    std::array<TextureAsset, static_cast<size_t>(TextureEnum::CNT)> textures;
};

/*
create_bullet() {
    emplace<Sprite>(registry, bullet, bullet_sprite);
}
*/

} // namespace bh