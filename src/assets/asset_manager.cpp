#include "asset_manager.hpp"

void bh::AssetManager::register_texture(const Image &image, TextureEnum id) {
    const auto texture = LoadTextureFromImage(image);
    UnloadImage(image);
    textures.at(static_cast<size_t>(id)) = TextureAsset { texture, static_cast<uint16_t>(texture.width), static_cast<uint16_t>(texture.height) };
}

void bh::AssetManager::register_texture(const Image &image, TextureEnum id, uint16_t cell_size_x, uint16_t cell_size_y) {
    const auto texture = LoadTextureFromImage(image);
    UnloadImage(image);
    textures.at(static_cast<size_t>(id)) = TextureAsset { texture, cell_size_x, cell_size_y};
}


auto bh::AssetManager::get_texture(TextureEnum id) -> TextureAsset { return textures.at(static_cast<size_t>(id)); }
