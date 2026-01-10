#include <cmath>
#include <iostream>

#include "raylib.h"

#define WIDTH 1280
#define HEIGHT 800
#define RAYS_NUMBER 1000

struct Circle {
    float x;
    float y;
    float r;
};

struct Ray2D {
    float start_x, start_y;
    float angle;
};

void generate_rays(struct Circle* circle, struct Ray2D rays[]) {
    for (int i = 0; i < RAYS_NUMBER; i++) {
        float angle = (float)i * (2 * M_PI) / RAYS_NUMBER;
        struct Ray2D ray = {circle->x, circle->y, angle};
        rays[i] = ray;
        std::cout << angle << std::endl;
    }
}

void draw_rays(struct Ray2D rays[]) {
    for (int i = 0; i < RAYS_NUMBER; i++) {
        struct Ray2D ray = rays[i];

        float dx = cosf(ray.angle);
        float dy = sinf(ray.angle);

        float length = 1000.0f;

        float x2 = ray.start_x + dx * length;
        float y2 = ray.start_y + dy * length;

        DrawLineEx(
            (Vector2){ray.start_x, ray.start_y},
            (Vector2){x2, y2},
            3.0f,
            YELLOW);
    }
}

bool is_mouse_on_circle(struct Circle* circle) {
    Vector2 mouse_position = GetMousePosition();

    float dx = mouse_position.x - circle->x;
    float dy = mouse_position.y - circle->y;

    return (dx * dx) + (dy * dy) <= circle->r * circle->r;
}

void draw_circle_at_mouse(struct Circle* circle, struct Ray2D rays[], Color color) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && is_mouse_on_circle(circle)) {
        Vector2 pos = GetMousePosition();
        circle->x = pos.x;
        circle->y = pos.y;
        generate_rays(circle, rays);
    }
    DrawCircle(circle->x, circle->y, circle->r, color);
}

int main() {
    InitWindow(WIDTH, HEIGHT, "Ray Tracing Demo");
    SetTargetFPS(60);

    struct Circle circle = {200, 200, 80};
    struct Circle shadow_circle = {650, 300, 140};

    struct Ray2D rays[RAYS_NUMBER];
    generate_rays(&circle, rays);  // need rays to be generated before circle is moved too
    draw_rays(rays);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        draw_rays(rays);
        draw_circle_at_mouse(&circle, rays, WHITE);
        draw_circle_at_mouse(&shadow_circle, rays, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
