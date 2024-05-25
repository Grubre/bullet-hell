#include <imgui.h>
#include <print>
#include <entt.hpp>
#include "raylib.h"
#include "rlImGui.h"
#include "keyinput.hpp"

void setup_raylib() {
    const auto display = GetCurrentMonitor();
    const int screen_width = GetMonitorWidth(display);
    const int screen_height = GetMonitorHeight(display);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // SetConfigFlags(FLAG_VSYNC_HINT);

    std::println("Resolution is: {}x{}", screen_width, screen_height);
    InitWindow(screen_width, screen_height, "Hello World");
}

auto main() -> int {
    setup_raylib();

    rlImGuiSetup(true);

    bh::KeyManager manager{};

    const auto sub_id = manager.subscribe(KEY_W, []() { std::println("Pressed w!"); });
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

    CloseWindow();
    rlImGuiShutdown();
    return 0;
}
