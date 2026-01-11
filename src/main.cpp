// 2D CPU-based ray tracing demo
// demonstrates ray–circle intersection

#include <cmath>
#include <iostream>

#include "raylib.h"

#define WIDTH 1280
#define HEIGHT 800
#define RAYS_NUMBER 100  // optimal = 2500
#define RAY_LENGTH 2000.0f

struct Circle {
    float x;
    float y;
    float r;
};

struct Ray2D {
    Vector2 origin;
    Vector2 direction;  // must be normalized
};

void generate_rays(const Circle& emitter, struct Ray2D rays[]) {
    for (int i = 0; i < RAYS_NUMBER; i++) {
        float angle = (float)i * (2 * M_PI) / RAYS_NUMBER;
        Vector2 ray_origin = {emitter.x, emitter.y};

        Vector2 ray_direction = {cosf(angle), sinf(angle)};

        // normalize ray_direction
        float ray_len = sqrtf(ray_direction.x * ray_direction.x +
                              ray_direction.y * ray_direction.y);
        ray_direction.x /= ray_len;
        ray_direction.y /= ray_len;

        Ray2D ray = {ray_origin, ray_direction};
        rays[i] = ray;
    }
}

bool ray_circle_intersect(
    const Ray2D& ray,
    const Circle& obstacle_circle,
    float* distance_to_hit,
    Vector2& hit_point_ref,
    Vector2& surface_normal_ref) {
    // vector from ray.origin to obstacle_circle center
    Vector2 to_circle = {
        obstacle_circle.x - ray.origin.x,
        obstacle_circle.y - ray.origin.y};

    float obstacle_circle_radius = obstacle_circle.r;

    // dot product (distance from the ray origin to the point on the ray that is closest
    // to the circle center, measured along the ray. It is the projection of to_circle
    // onto dir.)
    float closest_along_ray = to_circle.x * ray.direction.x + to_circle.y * ray.direction.y;

    // negative distance implies that closest point is behind the ray, i.e., obstacle
    // circle is behind ray
    if (closest_along_ray < 0) return false;

    // using the above dot product, we find the point on the ray.
    Vector2 closest_point = {
        ray.origin.x + ray.direction.x * closest_along_ray,
        ray.origin.y + ray.direction.y * closest_along_ray};

    // vector from the closest point on the ray to the circle center, i.e., this vector points
    // straight from the ray to the circle at closest approach
    Vector2 perpendicular = {
        obstacle_circle.x - closest_point.x,
        obstacle_circle.y - closest_point.y};

    // magnitute of perpendicular squared (avoiding root)
    float distance_sq = perpendicular.x * perpendicular.x + perpendicular.y * perpendicular.y;

    // ray misses the circle (closest approach is outside the radius)
    if (distance_sq > obstacle_circle_radius * obstacle_circle_radius) return false;

    // distance along the ray from the closest point back to the circle surface.
    // this is half the length of the ray segment inside the circle (the full chord),
    // and is used to step back from the closest approach to the first hit point.
    float half_chord = sqrtf(obstacle_circle_radius * obstacle_circle_radius - distance_sq);

    // distance from ray origin to first contact with the obstacle circle
    float hit_distance = closest_along_ray - half_chord;

    *distance_to_hit = hit_distance;

    // hit point
    Vector2 hit_point = {
        ray.origin.x + ray.direction.x * hit_distance,
        ray.origin.y + ray.direction.y * hit_distance};

    hit_point_ref = hit_point;

    // surface normal (obstacle_circle's center to hit point)
    Vector2 surface_normal = {hit_point.x - obstacle_circle.x, hit_point.y - obstacle_circle.y};
    float surface_normal_magnitude = sqrtf(
        surface_normal.x * surface_normal.x + surface_normal.y * surface_normal.y);
    // normalize
    surface_normal.x /= surface_normal_magnitude;
    surface_normal.y /= surface_normal_magnitude;

    surface_normal_ref = surface_normal;

    return true;
}

void draw_rays(struct Ray2D rays[], const Circle& obstacle_circle) {
    for (int i = 0; i < RAYS_NUMBER; i++) {
        const Ray2D& ray = rays[i];

        float maxLen = RAY_LENGTH;
        float distance_to_hit;
        Vector2 hit_point;
        Vector2 surface_normal;

        if (ray_circle_intersect(ray, obstacle_circle, &distance_to_hit, hit_point, surface_normal)) {
            if (distance_to_hit > 0 && distance_to_hit < maxLen)
                maxLen = distance_to_hit;
        }

        Vector2 end = {
            ray.origin.x + ray.direction.x * maxLen,
            ray.origin.y + ray.direction.y * maxLen};

        Color yellow = {255, 255, 0, 20};
        DrawLineEx(ray.origin, end, 2.0f, yellow);

        DrawLineEx(hit_point,
                   {hit_point.x + surface_normal.x * 25, hit_point.y + surface_normal.y * 25},
                   1.0f, RED);
    }
}

bool is_mouse_on_circle(const Circle& circle) {
    Vector2 mouse_position = GetMousePosition();

    float dx = mouse_position.x - circle.x;
    float dy = mouse_position.y - circle.y;

    return (dx * dx) + (dy * dy) <= circle.r * circle.r;
}

void move_emitter_circle(Circle& emitter, Ray2D rays[]) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        is_mouse_on_circle(emitter)) {
        Vector2 pos = GetMousePosition();
        emitter.x = pos.x;
        emitter.y = pos.y;

        generate_rays(emitter, rays);
    }

    DrawCircle(emitter.x, emitter.y, emitter.r, WHITE);
}

void move_obstacle_circle(Circle& obstacle) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        is_mouse_on_circle(obstacle)) {
        Vector2 pos = GetMousePosition();
        obstacle.x = pos.x;
        obstacle.y = pos.y;
    }

    DrawCircle(obstacle.x, obstacle.y, obstacle.r, WHITE);
}

int main() {
    InitWindow(WIDTH, HEIGHT, "Ray Tracing Demo");
    SetTargetFPS(60);

    struct Circle circle = {200, 200, 40};
    struct Circle shadow_circle = {650, 300, 140};

    struct Ray2D rays[RAYS_NUMBER];
    generate_rays(circle, rays);  // need rays to be generated before circle is moved too

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GRAY);

        draw_rays(rays, shadow_circle);
        move_emitter_circle(circle, rays);
        move_obstacle_circle(shadow_circle);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
