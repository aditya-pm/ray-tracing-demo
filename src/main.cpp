#include <iostream>

#include "raylib.h"

int main() {
    InitWindow(1280, 800, "Ray Tracing Demo");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
