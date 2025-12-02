# Copilot Instructions

This file guides GitHub Copilot to better understand the context and conventions of this project.

## Project Overview
- **Project Name**: Skyscape (Flight Simulation System)
- **Architecture**: Modern OpenGL Engine (C++)
- **Tech Stack**: C++17, OpenGL 3.3+, CMake
- **Key Components**:
  - `src/core/`: Window management (GLFW), Input handling (`Input.h`).
  - `src/graphics/`: Rendering engine (`Shader`, `Camera`).
  - `src/world/`: 
    - `InfiniteTerrain`: Chunk-based procedural terrain generation using Perlin noise.
    - `Plane`: F-22 fighter jet model loading and rendering.
  - `assets/`: 
    - `shaders/`: GLSL shaders (`infinite_terrain.*`, `plane.*`).
    - `models/`: OBJ models (`airplane.obj`).

## Critical Workflows
- **Build**: 
  ```bash
  mkdir build && cd build
  cmake ..
  cmake --build .
  ```
- **Run**: `./build/bin/Debug/Skyscape.exe` (Windows)
- **Dependency Management**: 
  - GLFW, GLM, STB are fetched via CMake `FetchContent`.
  - **GLAD** is located in `vendor/glad`.

## Coding Conventions
- **Style Guide**: Google C++ Style (mostly).
- **Naming**: PascalCase for Classes (`Window`), camelCase for variables (`m_Width`), snake_case for file names.
- **OpenGL**:
  - Use `glad` for loading.
  - Use `glm` for math.
  - Always check for shader compilation errors (handled in `Shader` class).
  - Use RAII for OpenGL resources (Buffers, Textures) where possible.

## Specific Implementation Details
- **Terrain**: The terrain is infinite, generated in chunks around the camera. It uses a procedural noise function (in `InfiniteTerrain.cpp`) to generate heightmaps.
- **Plane**: The player controls an F-22 aircraft. The model is loaded from `assets/models/airplane.obj` using a custom OBJ parser in `Plane.cpp`.
- **Camera**: The camera follows the plane but has a slight lag/smoothing for a better feel.
- **Shaders**:
  - `infinite_terrain.vert/frag`: Handles terrain coloring based on height (Water, Sand, Grass, Forest, Rock, Snow) and distance fog.
  - `plane.vert/frag`: Handles the aircraft rendering with basic lighting.

## Specific Instructions for AI Agents
- When adding new rendering features, create a new class in `src/graphics` or `src/world`.
- Always ensure shaders are placed in `assets/shaders` and loaded via relative paths or copied to bin.
- The `Window` class handles the OpenGL context; do not create another window.
- Use `glm::vec3` and `glm::mat4` for all 3D math.
