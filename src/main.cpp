#include <imgui.h>
#include "raylib.h"
#include "rlImGui.h"

void setup_raylib() {

    const auto display = GetCurrentMonitor();
    const int screen_width = GetMonitorWidth(display);
    const int screen_height = GetMonitorHeight(display);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(screen_width, screen_height, "Hello World");
}

auto main() -> int {
    setup_raylib();

    rlImGuiSetup(true);

    while(!WindowShouldClose()) {
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
