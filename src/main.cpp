/// TODO
/// - Rendering
///     - Component Sprite(texture)
///     - Component AnimatedSprite(texture, texture_size, cell_size, sprite_id) 
///     - Component Visible(empty)

#include <imgui.h>
#include <fmt/printf.h>
#include <entt.hpp>
#include "asset_manager.hpp"
#include "assets_loader.hpp"
#include "raylib.h"
#include "rlImGui.h"
#include "keyinput.hpp"
#include "components/sprite.hpp"

void setup_raylib() {
    const auto display = GetCurrentMonitor();
    const int screen_width = GetMonitorWidth(display);
    const int screen_height = GetMonitorHeight(display);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // SetConfigFlags(FLAG_VSYNC_HINT);

    fmt::println("Resolution is: {}x{}", screen_width, screen_height);
    InitWindow(screen_width, screen_height, "Hello World");
}

auto main() -> int {
    setup_raylib();

    rlImGuiSetup(true);

    auto registry = entt::registry();

    bh::KeyManager manager{};

    const auto sub_id = manager.subscribe(KEY_W, []() { fmt::println("Pressed w!"); });
    manager.subscribe(KEY_S, [&]() { manager.unsubscribe(sub_id); });

    auto asset_manager = bh::AssetManager();

    auto image = bh::load_asset(LoadImage, "unknown.png");
    using TE = bh::TextureEnum;
    asset_manager.register_texture(image, TE::PLAYER_TEXTURE);
    
    registry.ctx().emplace<bh::AssetManager>(asset_manager);

    auto sprite = registry.create();

    bh::emplace_sprite(registry, sprite, TE::PLAYER_TEXTURE);

    while (!WindowShouldClose()) {
        bh::notify_keyboard_press_system(manager);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        rlImGuiBegin();
        ImGui::ShowDemoWindow();
        rlImGuiEnd();

        bh::render_sprites(registry);

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
