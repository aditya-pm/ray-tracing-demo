#include <cmath>
#include <iostream>

#include "raylib.h"

#define WIDTH 1280
#define HEIGHT 800
#define RAYS_NUMBER 1500
#define RAY_LENGTH 2000.0f

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

bool ray_circle_intersect(
    Vector2 origin,
    Vector2 dir,
    Vector2 center,
    float radius,
    float* distance_to_hit) {
    Vector2 oc = {
        origin.x - center.x,
        origin.y - center.y};

    float a = dir.x * dir.x + dir.y * dir.y;
    float b = 2.0f * (oc.x * dir.x + oc.y * dir.y);
    float c = (oc.x * oc.x + oc.y * oc.y) - radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;

    float t = (-b - sqrtf(discriminant)) / (2.0f * a);
    if (t < 0) return false;

    *distance_to_hit = t;
    return true;
}

void draw_rays(struct Ray2D rays[], struct Circle* obstacle) {
    Vector2 center = {obstacle->x, obstacle->y};
    float radius = obstacle->r;

    for (int i = 0; i < RAYS_NUMBER; i++) {
        Ray2D ray = rays[i];

        Vector2 origin = {ray.start_x, ray.start_y};
        Vector2 dir = {cosf(ray.angle), sinf(ray.angle)};

        float maxLen = RAY_LENGTH;
        float distance_to_hit;

        if (ray_circle_intersect(origin, dir, center, radius, &distance_to_hit)) {
            if (distance_to_hit > 0 && distance_to_hit < maxLen)
                maxLen = distance_to_hit;
        }

        Vector2 end = {
            origin.x + dir.x * maxLen,
            origin.y + dir.y * maxLen};

        DrawLineEx(origin, end, 2.0f, YELLOW);
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
    draw_rays(rays, &shadow_circle);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GRAY);

        draw_rays(rays, &shadow_circle);
        draw_circle_at_mouse(&circle, rays, WHITE);
        draw_circle_at_mouse(&shadow_circle, rays, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
