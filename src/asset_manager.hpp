#pragma once
#include <raylib.h>
#include <array>
#include <entt.hpp>

namespace bh {

enum class TextureEnum { BULLET_TEXTURE = 0, PLAYER_TEXTURE, CNT };

class AssetManager {
  public:
    void register_texture(const Image &image, TextureEnum id);
    auto get_texture(TextureEnum id) -> Texture2D;

  private:
    std::array<Texture2D, static_cast<size_t>(TextureEnum::CNT)> textures;
};

/*
create_bullet() {
    emplace<Sprite>(registry, bullet, bullet_sprite);
}
*/

} // namespace bh