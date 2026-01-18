cmake_minimum_required(VERSION 3.15)
project(Skyscape VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dependencies
# We use local vendor folders to avoid path encoding issues with FetchContent

# GLFW
if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glfw/CMakeLists.txt")
    message(STATUS "Using local GLFW...")
    add_subdirectory(vendor/glfw)
else()
    message(FATAL_ERROR "GLFW not found in vendor/glfw. Please run setup_dependencies.ps1")
endif()

# GLM
if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/glm/glm/glm.hpp")
    message(STATUS "Using local GLM...")
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE "${CMAKE_SOURCE_DIR}/vendor/glm")
else()
    message(FATAL_ERROR "GLM not found in vendor/glm. Please run setup_dependencies.ps1")
endif()

# STB Image
if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/stb")
    message(STATUS "Using local STB...")
    set(stb_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/stb")
else()
    message(FATAL_ERROR "STB not found in vendor/stb. Please run setup_dependencies.ps1")
endif()

# GLAD (We assume the user will provide GLAD, or we can try to fetch a pre-generated one)
# For this example, we will assume GLAD sources are in vendor/glad
# If not present, we will warn.
set(GLAD_DIR "${CMAKE_SOURCE_DIR}/vendor/glad")
if(EXISTS "${GLAD_DIR}/src/glad.c")
    add_library(glad "${GLAD_DIR}/src/glad.c")
    target_include_directories(glad PUBLIC "${GLAD_DIR}/include")
else()
    message(WARNING "GLAD not found in vendor/glad. Please generate it at https://glad.dav1d.de/ and place it in vendor/glad.")
    # Fallback or error? For now, we'll define a dummy target to allow configuration to pass, but build will fail.
    add_library(glad INTERFACE)
endif()

# Main Executable
add_executable(Skyscape
    src/main.cpp
    src/core/Window.cpp
    src/core/Window.h
    src/core/stb_impl.cpp
    src/graphics/Shader.cpp
    src/graphics/Shader.h
    src/graphics/Camera.h
    src/graphics/Mesh.h
    src/world/Terrain.h
    src/world/Terrain.cpp
    src/world/Skybox.h
    src/world/Skybox.cpp
    src/world/Plane.h
    src/world/Plane.cpp
    src/world/Grid.h
    src/world/Grid.cpp
    src/world/InfiniteTerrain.h
    src/world/InfiniteTerrain.cpp
    src/world/Tree.h
    src/world/Tree.cpp
    src/world/Cabin.h
    src/world/Cabin.cpp
    src/world/ParticleSystem.h
    src/world/ParticleSystem.cpp
    src/world/Stars.h
    src/world/Stars.cpp
)

target_include_directories(Skyscape PUBLIC 
    src 
    ${stb_SOURCE_DIR}
)

target_link_libraries(Skyscape PRIVATE 
    glfw 
    glm 
    glad
)

# Copy assets to bin AND build root for flexibility
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
