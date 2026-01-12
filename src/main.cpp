// 2D CPU-based ray tracing demo
// demonstrates ray–circle intersection

#include <cmath>
#include <iostream>

#include "raylib.h"

#define WIDTH 1280
#define HEIGHT 800
#define EMITTER_RAYS_NUMBER 1000
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

void generate_emitter_rays(const Circle& emitter, struct Ray2D rays[]) {
    for (int i = 0; i < EMITTER_RAYS_NUMBER; i++) {
        float angle = (float)i * (2 * M_PI) / EMITTER_RAYS_NUMBER;
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

void render_scene(struct Ray2D emitter_rays[], const Circle& obstacle_circle, Vector2 light_direction, bool debug) {
    for (int i = 0; i < EMITTER_RAYS_NUMBER; i++) {
        const Ray2D& emitter_ray = emitter_rays[i];

        float max_len = RAY_LENGTH;
        float distance_to_hit;
        Vector2 hit_point;
        Vector2 surface_normal;

        bool hit = ray_circle_intersect(
            emitter_ray,
            obstacle_circle,
            &distance_to_hit,
            hit_point,
            surface_normal);

        if (hit && distance_to_hit > 0 && distance_to_hit < max_len) {
            max_len = distance_to_hit;

            float brightness = surface_normal.x * light_direction.x +
                               surface_normal.y * light_direction.y;

            // brightness = fmaxf(0.0f, fminf(brightness, 1.0f));
            if (brightness < 0) brightness = 0;
            if (brightness > 1) brightness = 1;

            unsigned char intensity = (unsigned char)(brightness * 255);
            Color shaded = {intensity, intensity, intensity, 255};

            DrawCircleV(hit_point, 5.0f, shaded);

            if (debug) {
                // draw surface normal only if debug and ray hit
                DrawLineEx(hit_point,
                           {hit_point.x + surface_normal.x * 25, hit_point.y + surface_normal.y * 25},
                           1.0f, RED);
            }
        }

        Vector2 emitter_ray_end = {
            emitter_ray.origin.x + emitter_ray.direction.x * max_len,
            emitter_ray.origin.y + emitter_ray.direction.y * max_len};

        if (debug) {
            // draw emitter rays
            Color yellow = {255, 255, 0, 20};
            DrawLineEx(emitter_ray.origin, emitter_ray_end, 2.0f, yellow);
        }
    }
}

bool is_mouse_on_circle(const Circle& circle) {
    Vector2 mouse_position = GetMousePosition();

    float dx = mouse_position.x - circle.x;
    float dy = mouse_position.y - circle.y;

    return (dx * dx) + (dy * dy) <= circle.r * circle.r;
}

void move_emitter_circle(Circle& emitter, Ray2D rays[], bool debug) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        is_mouse_on_circle(emitter)) {
        Vector2 pos = GetMousePosition();
        emitter.x = pos.x;
        emitter.y = pos.y;

        generate_emitter_rays(emitter, rays);
    }

    DrawCircle(emitter.x, emitter.y, emitter.r, WHITE);
    if (debug)
        DrawText("Emitter", emitter.x - MeasureText("Emitter", 20) / 2.0, emitter.y - 20 / 2.0, 20, RED);
}

void move_obstacle_circle(Circle& obstacle, bool debug) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        is_mouse_on_circle(obstacle)) {
        Vector2 pos = GetMousePosition();
        obstacle.x = pos.x;
        obstacle.y = pos.y;
    }

    if (debug) {
        // DrawCircle(obstacle.x, obstacle.y, obstacle.r, WHITE);
        DrawCircleLines(obstacle.x, obstacle.y, obstacle.r, GREEN);
        DrawText("Obstacle", obstacle.x - MeasureText("Obstacle", 20) / 2.0, obstacle.y - 20 / 2.0, 20, RED);
    }
}

void draw_light_direction(Vector2 light_direction, bool debug) {
    // arrow body
    Vector2 start = {80.0f, 80.0f};
    Vector2 end = {
        start.x + light_direction.x * 60.0f,
        start.y + light_direction.y * 60.0f};

    // arrow head
    Vector2 perp = {-light_direction.y, light_direction.x};

    Vector2 arrow_left = {
        end.x - light_direction.x * 10.0f + perp.x * 6.0f,
        end.y - light_direction.y * 10.0f + perp.y * 6.0f};

    Vector2 arrow_right = {
        end.x - light_direction.x * 10.0f - perp.x * 6.0f,
        end.y - light_direction.y * 10.0f - perp.y * 6.0f};

    if (debug) {
        DrawLineEx(start, end, 3.0f, RED);
        DrawLineEx(end, arrow_left, 3.0f, RED);
        DrawLineEx(end, arrow_right, 3.0f, RED);
        DrawText("Light Direction (not photon, photon = opposite)", 80, 40, 16, RED);
    }
}

void toggle_debug(bool& debug) {
    if (IsKeyPressed(KEY_D)) debug = !debug;
}

int main() {
    InitWindow(WIDTH, HEIGHT, "Ray Tracing Demo");
    SetTargetFPS(60);

    struct Circle circle = {200, 200, 50};
    struct Circle shadow_circle1 = {650, 300, 140};

    struct Ray2D emitter_rays[EMITTER_RAYS_NUMBER];
    generate_emitter_rays(circle, emitter_rays);  // need rays to be generated before circle is moved too

    // lambert lighting convention: light_direction is surface to light source.
    // actual photons go from source to surface
    // here light_direction is 'direction to travel to reach light'
    Vector2 light_direction = {-1.0f, -1.0f};
    float light_direction_magnitude = sqrtf(
        light_direction.x * light_direction.x + light_direction.y * light_direction.y);
    light_direction.x /= light_direction_magnitude;
    light_direction.y /= light_direction_magnitude;

    bool debug = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        toggle_debug(debug);
        draw_light_direction(light_direction, debug);
        render_scene(emitter_rays, shadow_circle1, light_direction, debug);
        move_emitter_circle(circle, emitter_rays, debug);
        move_obstacle_circle(shadow_circle1, debug);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}