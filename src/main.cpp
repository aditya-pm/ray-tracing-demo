// 2D CPU-based ray tracing demo
// demonstrates ray–circle intersection

#include <cmath>
#include <iostream>

#include "raylib.h"

#define WIDTH 1280
#define HEIGHT 800
#define RAYS_NUMBER 5000
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
    }
}

bool ray_circle_intersect(
    Vector2 origin,
    Vector2 dir,
    Vector2 center,
    float radius,
    float* distance_to_hit) {
    // vector from ray start to circle (obstacle) center
    Vector2 to_circle = {
        center.x - origin.x,
        center.y - origin.y};

    // dot product (distance from the ray origin to the point on the ray that is closest
    // to the circle center, measured along the ray. It is the projection of to_circle
    // onto dir.)
    float closest_along_ray = to_circle.x * dir.x + to_circle.y * dir.y;

    // negative distance implies that closest point is behind the ray, i.e., obstacle
    // circle is behind ray
    if (closest_along_ray < 0) return false;

    // using the above dot product, we find the point on the ray.
    Vector2 closest_point = {
        origin.x + dir.x * closest_along_ray,
        origin.y + dir.y * closest_along_ray};

    // vector from the closest point on the ray to the circle center, i.e., this vector points
    // straight from the ray to the circle at closest approach
    Vector2 perpendicular = {
        center.x - closest_point.x,
        center.y - closest_point.y};

    // magnitute of perpendicular squared (avoiding root)
    float distance_sq = perpendicular.x * perpendicular.x + perpendicular.y * perpendicular.y;

    // ray misses the circle (closest approach is outside the radius)
    if (distance_sq > radius * radius) return false;

    // distance along the ray from the closest point back to the circle surface.
    // this is half the length of the ray segment inside the circle (the full chord),
    // and is used to step back from the closest approach to the first hit point.
    float half_chord = sqrtf(radius * radius - distance_sq);

    // distance from ray origin to first contact with the obstacle circle
    float hit_distance = closest_along_ray - half_chord;

    *distance_to_hit = hit_distance;
    return true;
}

void draw_rays(struct Ray2D rays[], struct Circle* obstacle) {
    Vector2 center = {obstacle->x, obstacle->y};
    float radius = obstacle->r;

    for (int i = 0; i < RAYS_NUMBER; i++) {
        Ray2D ray = rays[i];

        Vector2 origin = {ray.start_x, ray.start_y};

        Vector2 direction = {cosf(ray.angle), sinf(ray.angle)};
        float direction_magnitude = sqrtf(direction.x * direction.x + direction.y * direction.y);

        // normalize
        direction.x /= direction_magnitude;
        direction.y /= direction_magnitude;

        float maxLen = RAY_LENGTH;
        float distance_to_hit;

        if (ray_circle_intersect(origin, direction, center, radius, &distance_to_hit)) {
            if (distance_to_hit > 0 && distance_to_hit < maxLen)
                maxLen = distance_to_hit;
        }

        Vector2 end = {
            origin.x + direction.x * maxLen,
            origin.y + direction.y * maxLen};

        Color yellow = {255, 255, 0, 20};
        DrawLineEx(origin, end, 2.0f, yellow);
    }
}

bool is_mouse_on_circle(struct Circle* circle) {
    Vector2 mouse_position = GetMousePosition();

    float dx = mouse_position.x - circle->x;
    float dy = mouse_position.y - circle->y;

    return (dx * dx) + (dy * dy) <= circle->r * circle->r;
}

void move_emitter_circle(struct Circle* emitter, Ray2D rays[]) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        is_mouse_on_circle(emitter)) {
        Vector2 pos = GetMousePosition();
        emitter->x = pos.x;
        emitter->y = pos.y;

        generate_rays(emitter, rays);
    }

    DrawCircle(emitter->x, emitter->y, emitter->r, WHITE);
}

void move_obstacle_circle(struct Circle* obstacle) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        is_mouse_on_circle(obstacle)) {
        Vector2 pos = GetMousePosition();
        obstacle->x = pos.x;
        obstacle->y = pos.y;
    }

    DrawCircle(obstacle->x, obstacle->y, obstacle->r, WHITE);
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
        move_emitter_circle(&circle, rays);
        move_obstacle_circle(&shadow_circle);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
