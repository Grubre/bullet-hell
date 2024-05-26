#include "asset_manager.hpp"

void bh::AssetManager::register_texture(const Image &image, TextureEnum id) {
    const auto texture = LoadTextureFromImage(image);
    UnloadImage(image);
    textures.at(static_cast<size_t>(id)) = texture;
}
void bh::AssetManager::register_sound(const Sound &sound, bh::SoundEnum id) {
    sounds.at(static_cast<size_t>(id)) = sound;
}

auto bh::AssetManager::get_texture(TextureEnum id) -> Texture2D { return textures.at(static_cast<size_t>(id)); }
auto bh::AssetManager::get_sound(SoundEnum id) -> Sound { return sounds.at(static_cast<size_t>(id)); }
