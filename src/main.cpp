#include "core/Window.h"
#include "graphics/Shader.h"
#include "graphics/Camera.h"
#include "world/InfiniteTerrain.h"
#include "world/Skybox.h"
#include "world/Plane.h"

#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Camera
Camera camera(glm::vec3(0.0f, 10.0f, 30.0f));
float lastX = 1280.0f / 2.0;
float lastY = 720.0f / 2.0;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void processInput(Window& window) {
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.getNativeWindow(), true);

    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

int main() {
    Window window(1280, 720, "Skyscape - Flight Simulator");

    // Callbacks
    glfwSetCursorPosCallback(window.getNativeWindow(), mouse_callback);
    glfwSetScrollCallback(window.getNativeWindow(), scroll_callback);
    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Enable Depth Test
    glEnable(GL_DEPTH_TEST);

    // Shaders
    Shader terrainShader("assets/shaders/infinite_terrain.vert", "assets/shaders/infinite_terrain.frag");
    Shader planeShader("assets/shaders/plane.vert", "assets/shaders/plane.frag");

    // Infinite Terrain (auto-generates as you fly)
    InfiniteTerrain terrain(64, 8); // chunk size 64, view distance 8 chunks
    
    // Skybox
    std::vector<std::string> faces = {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };
    Skybox skybox(faces);

    // Plane
    Plane plane;

    // Lighting - sun position high in the sky
    glm::vec3 lightPos(500.0f, 800.0f, 300.0f);

    // Increase camera speed for flight simulation feel
    camera.MovementSpeed = 150.0f;

    while (!window.shouldClose()) {
        // Time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);
        
        // Boost speed with Shift
        if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.MovementSpeed = 800.0f;
        else
            camera.MovementSpeed = 150.0f;

        window.pollEvents();

        // Render
        glClearColor(0.7f, 0.8f, 0.9f, 1.0f); // Light blue sky background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // View/Projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window.getWidth() / (float)window.getHeight(), 1.0f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Update and draw infinite terrain
        terrain.Update(camera.Position);
        
        terrainShader.use();
        terrainShader.setMat4("projection", projection);
        terrainShader.setMat4("view", view);
        terrainShader.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.9f)); // Warm sunlight
        terrainShader.setVec3("lightPos", lightPos);
        terrainShader.setVec3("viewPos", camera.Position);
        terrain.Draw(terrainShader);

        // 2. Draw Plane
        // Draw plane slightly in front of camera to simulate "cockpit" or 3rd person view
        glm::vec3 planePos = camera.Position + camera.Front * 10.0f - camera.Up * 2.0f;
        
        planeShader.use();
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", view);
        planeShader.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.9f));
        planeShader.setVec3("lightPos", lightPos);
        planeShader.setVec3("viewPos", camera.Position);

        plane.Draw(planeShader, planePos, camera.Front, 2.0f); // Larger plane

        // 3. Draw Skybox (last)
        skybox.Draw(view, projection);

        window.swapBuffers();
    }

    return 0;
}
