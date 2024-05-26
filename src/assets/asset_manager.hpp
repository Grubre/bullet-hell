#pragma once
#include <raylib.h>
#include <array>
#include <entt.hpp>

namespace bh {

enum class TextureEnum { BULLET_TEXTURE = 0, PLAYER_TEXTURE, CNT };
enum class SoundEnum { WIN = 0 , CNT};
class AssetManager {
  public:
    void register_texture(const Image &image, const TextureEnum id);
    auto get_texture(const TextureEnum id) -> Texture2D;
    void register_sound(const Sound &sound, const SoundEnum id);
    auto get_sound(const SoundEnum id) -> Sound;
  private:
    std::array<Texture2D, static_cast<size_t>(TextureEnum::CNT)> textures;
    std::array<Sound,static_cast<size_t>(SoundEnum::CNT)> sounds;
};

/*
create_bullet() {
    emplace<Sprite>(registry, bullet, bullet_sprite);
}
*/

} // namespace bh