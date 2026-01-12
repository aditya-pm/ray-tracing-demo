# 2D CPU Ray Tracing Demo

A small **2D ray tracing experiment** written in C++, focused on visualizing how rays interact with simple geometry and how surface orientation affects lighting. This project is intended as a learning and experimentation tool rather than a full renderer.

---

## Overview

The scene consists of:

* A **ray emitter** that emits rays uniformly in all directions
* A **single circular obstacle** used for intersection testing
* A **directional light** (sunlight-style) used only for shading

Rays are used to determine **visibility and geometry interaction**, while lighting is applied separately using a fixed light direction.

---

## Demo

<p align="center">
  <img src="demo/demo.gif" alt="Ray Tracing Demo" width="800">
</p>

A short recording showing emitter movement, obstacle movement, lighting response, and debug visualization.

---

## Controls

* **Left Mouse Button:** Drag emitter or obstacle
* **D:** Toggle debug mode

---

## Working (High Level)

1. Rays are emitted from the emitter at evenly spaced angles
2. Each ray is tested against the obstacle for intersection
3. If a hit occurs, the nearest intersection is selected
4. A surface normal is computed at the hit point
5. Diffuse lighting is calculated using a fixed light direction
6. The hit point is rendered with brightness based on lighting

---

## Scene Elements

### Ray Emitter

* Represented as a circle
* Emits rays only to **probe the scene**
* Used for visibility and intersection testing
* **Not a light source**

### Obstacle

* Represented as a circle
* Acts as the only piece of geometry in the scene

### Directional Light

* Defined by a normalized direction vector
* Used only for shading (Lambert diffuse lighting)
* Independent of the emitter position

---

## Concepts Used

* **Ray:** Half-line defined by origin and normalized direction
* **Uniform angular sampling:** Even distribution of rays over (2\pi)
* **Ray–circle intersection:** Analytical geometric intersection test
* **Closest-hit selection:** Only the nearest valid intersection is used
* **Surface normal:** Perpendicular direction at the point of contact
* **Directional light:** Global light with constant direction
* **Lambertian diffuse lighting:** Brightness proportional to surface orientation
* **Dot product:** Used for projection and lighting calculations

---

## Debug Mode

When enabled, debug mode visualizes:

* All emitted rays
* Surface normals at hit points
* Directional light vector
* Labels for emitter and obstacle

---

## What This Project Does Not Do

This demo intentionally avoids advanced features:

* No shadow rays
* No reflections or refractions
* No multiple bounces
* No global illumination
* No specular lighting
* No textures or materials
* No pixel-based rendering
* No camera or image buffer
* No acceleration structures

---

## Limitations

* Geometry is limited to a single circle
* Rays are emitted by angle, not per pixel
* Lighting is evaluated only at intersection points
* Performance scales linearly with ray count

---

## Possible Extensions

* Shadow ray implementation
* Multiple obstacles
* Reflection rays
* Pixel-based ray casting
* Material system
* Acceleration structures
* Multi-threading

---

## Build & Run

This project uses **CMake** and depends on **raylib**.

### Requirements

* CMake ≥ 3.12
* C++17 compatible compiler
* raylib installed on the system

### Build Steps

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Run

```bash
./RayTracing
```

On Windows:

```bash
RayTracing.exe
```

---

## Purpose

This project serves as a **minimal, transparent foundation** for understanding how ray tracing works at a geometric and algorithmic level, with all logic visible and easy to modify.

It is well-suited as a starting point for experimenting with more advanced ray tracing techniques.
