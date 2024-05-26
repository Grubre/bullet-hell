/// TODO
/// - Rendering
///     - Component Sprite(texture)
///     - Component AnimatedSprite(texture, texture_size, cell_size, sprite_id) 
///     - Component Visible(empty)

#include <imgui.h>
#include <fmt/printf.h>
#include <entt.hpp>
#include "fmt/base.h"
#include "raylib.h"
#include "rlImGui.h"
#include "keyinput.hpp"

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

    bh::KeyManager manager{};

    const auto sub_id = manager.subscribe(KEY_W, []() { fmt::println("Pressed w!"); });
    manager.subscribe(KEY_S, [&]() { manager.unsubscribe(sub_id); });

    

    while (!WindowShouldClose()) {
        bh::notify_keyboard_press_system(manager);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        rlImGuiBegin();
        ImGui::ShowDemoWindow();
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
