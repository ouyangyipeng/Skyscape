#include "core/Window.h"
#include "graphics/Shader.h"
#include "graphics/Camera.h"
#include "world/InfiniteTerrain.h"
#include "world/Skybox.h"
#include "world/Plane.h"
#include "world/Tree.h" 
#include "world/Terrain.h" 

#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// ✅ 辅助函数：获取地形高度
float getTerrainHeightAt(float x, float z, const std::vector<float>& vertices, 
                         int terrainWidth, int terrainHeight) {
    float gridX = x / 2.0f;
    float gridZ = z / 2.0f;
    
    if(gridX < 0.0f || gridX >= terrainWidth - 1 || 
       gridZ < 0.0f || gridZ >= terrainHeight - 1) {
        return 50.0f; // 默认高度
    }
    
    int x0 = static_cast<int>(gridX);
    int z0 = static_cast<int>(gridZ);
    
    int idx00 = (z0 * terrainWidth + x0) * 8;
    int idx10 = (z0 * terrainWidth + (x0 + 1)) * 8;
    int idx01 = ((z0 + 1) * terrainWidth + x0) * 8;
    int idx11 = ((z0 + 1) * terrainWidth + (x0 + 1)) * 8;
    
    if(idx11 + 1 >= static_cast<int>(vertices.size())) {
        return 50.0f;
    }
    
    float h00 = vertices[idx00 + 1];
    float h10 = vertices[idx10 + 1];
    float h01 = vertices[idx01 + 1];
    float h11 = vertices[idx11 + 1];
    
    float tx = gridX - x0;
    float tz = gridZ - z0;
    
    float h0 = h00 * (1.0f - tx) + h10 * tx;
    float h1 = h01 * (1.0f - tx) + h11 * tx;
    
    return h0 * (1.0f - tz) + h1 * tz;
}

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
    Shader terrainShader("assets/shaders/terrain.vert", "assets/shaders/terrain.frag");
    Shader planeShader("assets/shaders/plane.vert", "assets/shaders/plane.frag");
    Shader treeShader("assets/shaders/tree.vert", "assets/shaders/tree.frag"); // ✅ 树木着色器
    Shader cabinShader("assets/shaders/tree.vert", "assets/shaders/tree.frag"); // ✅ 复用树木着色器
    Shader flowerShader("assets/shaders/flower.vert", "assets/shaders/flower.frag");
    Shader boatShader("assets/shaders/tree.vert", "assets/shaders/tree.frag");
    // // Infinite Terrain (auto-generates as you fly)
    InfiniteTerrain terrain(70, 30); // chunk size 64, view distance 8 chunks
    
    // 使用静态地形
    //Terrain terrain(1200, 1200);

    // // ✅ 设置相机初始位置（地形中心上方）
    // float startX = 600.0f; // 地形中心 (1200x1200 网格 * 2.0 缩放 = 2400x2400 世界单位)
    // float startZ = 600.0f;
    // float terrainHeight = getTerrainHeightAt(startX, startZ, terrain.getVertices(), 1200, 1200);
    // camera.Position = glm::vec3(startX, terrainHeight + 50.0f, startZ); // ✅ 地面上方 50 单位
    // ✅ 设置相机初始位置（基于无限地形的高度）
    float startX = 0.0f;      // 世界中心 X
    float startZ = 0.0f;      // 世界中心 Z
    float terrainHeight = terrain.GetHeight(startX, startZ);  // ✅ 使用 InfiniteTerrain 的 GetHeight
    float spawnHeight = terrainHeight + 100.0f;  // ✅ 地形上方 100 单位
    
    camera.Position = glm::vec3(startX, spawnHeight, startZ);
    // std::cout << "[Main] Camera spawned at (" 
    //           << camera.Position.x << ", " 
    //           << camera.Position.y << ", " 
    //           << camera.Position.z << ")" << std::endl;
    // std::cout << "[Main] Terrain height at spawn: " << terrainHeight << std::endl;
    
    // // 生成树木
    // Tree trees;
    // trees.GenerateTrees(500, 800, 800, terrain.getVertices());
    // Skybox
    // std::vector<std::string> faces = {
    //     "assets/textures/skybox/right.jpg",
    //     "assets/textures/skybox/left.jpg",
    //     "assets/textures/skybox/top.jpg",
    //     "assets/textures/skybox/bottom.jpg",
    //     "assets/textures/skybox/front.jpg",
    //     "assets/textures/skybox/back.jpg"
    // };

    std::vector<std::string> faces = {
        "assets/textures/skybox/Daylight Box_Right.bmp",
        "assets/textures/skybox/Daylight Box_Left.bmp",
        "assets/textures/skybox/Daylight Box_Top.bmp",
        "assets/textures/skybox/Daylight Box_Bottom.bmp",
        "assets/textures/skybox/Daylight Box_Front.bmp",
        "D:\\rolling\\ComputerGraphics\\final\\assets\\textures\\skybox\\Daylight Box_Back.bmp"
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
        //terrainShader.setMat4("model", glm::mat4(1.0f));  // ✅ 添加这一行
        terrainShader.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.9f)); // Warm sunlight
        terrainShader.setVec3("lightPos", lightPos);
        terrainShader.setVec3("viewPos", camera.Position);
        //terrainShader.setVec3("objectColor", glm::vec3(0.3f, 0.6f, 0.2f));  // ✅ 添加这一行（草绿色）
        terrain.Draw(terrainShader);

        // ✅ 2. Draw Trees
        treeShader.use();
        treeShader.setMat4("projection", projection);
        treeShader.setMat4("view", view);
        treeShader.setVec3("viewPos", camera.Position);
        treeShader.setVec3("lightPos", lightPos);
        terrain.DrawTrees(treeShader);

        // ✅ Draw Cabins
        cabinShader.use();
        cabinShader.setMat4("projection", projection);
        cabinShader.setMat4("view", view);
        cabinShader.setVec3("viewPos", camera.Position);
        cabinShader.setVec3("lightPos", lightPos);
        terrain.DrawCabins(cabinShader);

        // ✅ Draw Flowers
        flowerShader.use();
        flowerShader.setMat4("projection", projection);
        flowerShader.setMat4("view", view);
        flowerShader.setVec3("viewPos", camera.Position);
        flowerShader.setVec3("lightPos", lightPos);
        terrain.DrawFlowers(flowerShader);

        // ✅ Draw Boats
        boatShader.use();
        boatShader.setMat4("projection", projection);
        boatShader.setMat4("view", view);
        boatShader.setVec3("viewPos", camera.Position);
        boatShader.setVec3("lightPos", lightPos);
        terrain.DrawBoats(boatShader, currentFrame);

        // 2. Draw Plane
        // Draw plane slightly in front of camera to simulate "cockpit" or 3rd person view
        //glm::vec3 planePos = camera.Position + camera.Front * 10.0f - camera.Up * 2.0f;
        // 飞机位置固定在相机前方
        glm::vec3 planePos = camera.Position + camera.Front * 20.0f;

        // ✅ 修改：相机位置在机尾后方，看向机头
        glm::vec3 planeForward = glm::normalize(camera.Front); // 飞机朝向
        glm::vec3 cameraOffset = -planeForward * 5.0f  // 后退到机尾
                        + camera.Up * 30.0f;      // 稍微抬高视角

        glm::vec3 thirdPersonCamPos = planePos + cameraOffset;

        // 创建从机尾看向机头的视角
        glm::mat4 thirdPersonView = glm::lookAt(
            thirdPersonCamPos,                    // 相机位置：机尾后方
            planePos + planeForward * 5.0f,      // 看向：机头方向（飞机前方5米处）
            camera.Up                             // 上方向
        );


        planeShader.use();
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", thirdPersonView);
        planeShader.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.9f));
        planeShader.setVec3("lightPos", lightPos);
        planeShader.setVec3("viewPos", thirdPersonCamPos);

        plane.Draw(planeShader, planePos, camera.Front, 1.0f); // Larger plane

        // 3. Draw Skybox (last)
        skybox.Draw(view, projection);

        window.swapBuffers();
    }

    return 0;
}
