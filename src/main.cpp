#include "core/Window.h"
#include "graphics/Shader.h"
#include "graphics/Camera.h"
#include "world/InfiniteTerrain.h"
#include "world/Skybox.h"
#include "world/Plane.h"
#include "world/Tree.h" 
#include "world/Terrain.h" 
#include "world/ParticleSystem.h"
#include "world/Stars.h"

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

// Day-Night cycle
float timeOfDay = 12.0f; // 0-24 hours, starts at noon
float timeSpeed = 1.0f; // 1 second = 1 minute in-game

// Weather control
enum class WeatherType { None, Rain, Snow };
WeatherType currentWeather = WeatherType::None;

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
    
    // Weather toggle keys
    static bool key1Pressed = false;
    static bool key2Pressed = false;
    static bool key3Pressed = false;
    
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_1) == GLFW_PRESS && !key1Pressed) {
        currentWeather = WeatherType::None;
        std::cout << "Weather: Clear" << std::endl;
        key1Pressed = true;
    }
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_1) == GLFW_RELEASE) key1Pressed = false;
    
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_2) == GLFW_PRESS && !key2Pressed) {
        currentWeather = WeatherType::Rain;
        std::cout << "Weather: Rain" << std::endl;
        key2Pressed = true;
    }
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_2) == GLFW_RELEASE) key2Pressed = false;
    
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_3) == GLFW_PRESS && !key3Pressed) {
        currentWeather = WeatherType::Snow;
        std::cout << "Weather: Snow" << std::endl;
        key3Pressed = true;
    }
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_3) == GLFW_RELEASE) key3Pressed = false;
    
    // Time control
    static bool keyTPressed = false;
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_T) == GLFW_PRESS && !keyTPressed) {
        timeSpeed = (timeSpeed == 1.0f) ? 60.0f : 1.0f;
        std::cout << "Time speed: " << (timeSpeed == 1.0f ? "Normal" : "Fast") << std::endl;
        keyTPressed = true;
    }
    if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_T) == GLFW_RELEASE) keyTPressed = false;
}

int main() {
    std::cout << "=== Skyscape Starting ===" << std::endl;
    std::cout << "[1/6] Initializing window..." << std::endl;
    Window window(1280, 720, "Skyscape - Flight Simulator");
    std::cout << "[1/6] Window initialized" << std::endl;

    // Callbacks
    glfwSetCursorPosCallback(window.getNativeWindow(), mouse_callback);
    glfwSetScrollCallback(window.getNativeWindow(), scroll_callback);
    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Enable Depth Test
    glEnable(GL_DEPTH_TEST);
    // Enable backface culling for better performance
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    

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

    std::cout << "[4/6] Loading skybox..." << std::endl;
    std::vector<std::string> faces = {
        "assets/textures/skybox/Daylight Box_Right.bmp",
        "assets/textures/skybox/Daylight Box_Left.bmp",
        "assets/textures/skybox/Daylight Box_Top.bmp",
        "assets/textures/skybox/Daylight Box_Bottom.bmp",
        "assets/textures/skybox/Daylight Box_Front.bmp",
        "D:\\rolling\\ComputerGraphics\\final\\assets\\textures\\skybox\\Daylight Box_Back.bmp"
    };
    Skybox skybox(faces);
    std::cout << "[4/6] Skybox loaded" << std::endl;
    
    // Stars
    std::cout << "[4.5/6] Generating stars..." << std::endl;
    Stars stars(2000);
    std::cout << "[4.5/6] Stars generated" << std::endl;

    // Plane
    std::cout << "[5/6] Loading plane model..." << std::endl;
    Plane plane;
    std::cout << "[5/6] Plane loaded" << std::endl;
    
    // Particle Systems
    std::cout << "[6/6] Initializing particle systems..." << std::endl;
    // ParticleSystem trailSystem(ParticleType::Trail, 2000);
    ParticleSystem weatherSystem(ParticleType::Rain, 5000);
    float weatherEmissionTimer = 0.0f;
    std::cout << "[6/6] Particle systems initialized" << std::endl;

    // Lighting - sun position high in the sky
    glm::vec3 lightPos(500.0f, 800.0f, 300.0f);

    // Increase camera speed for flight simulation feel
    camera.MovementSpeed = 100.0f;

    std::cout << "[Initialization complete! Starting render loop]" << std::endl;
    std::cout << "Controls: WASD = Move, Mouse = Look, Shift = Boost, T = Speed Time" << std::endl;
    std::cout << "Weather: 1 = Clear, 2 = Rain, 3 = Snow, ESC = Exit" << std::endl;

    // Track camera velocity for plane animation
    glm::vec3 lastCameraPos = camera.Position;
    glm::vec3 cameraVelocity(0.0f);

    while (!window.shouldClose()) {
        // Time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Update time of day
        timeOfDay += (deltaTime / 60.0f) * timeSpeed;
        if (timeOfDay >= 24.0f) timeOfDay -= 24.0f;
        
        // Calculate sun position based on time
        float sunAngle = (timeOfDay / 24.0f) * 2.0f * 3.14159f;
        float sunHeight = sin(sunAngle);
        glm::vec3 lightPos(
            cos(sunAngle) * 1000.0f,
            sunHeight * 800.0f + 200.0f,
            sin(sunAngle) * 1000.0f
        );

        // Input
        processInput(window);
        
        // Boost speed with Shift
        if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.MovementSpeed = 800.0f;
        else
            camera.MovementSpeed = 150.0f;

        window.pollEvents();

        // Calculate camera velocity
        cameraVelocity = (camera.Position - lastCameraPos) / deltaTime;
        
        // Ground collision detection
        float groundHeight = terrain.GetHeight(camera.Position.x, camera.Position.z);
        float minHeight = groundHeight + 5.0f; // 5 units above ground
        if (camera.Position.y < minHeight) {
            camera.Position.y = minHeight;
            cameraVelocity.y = glm::max(0.0f, cameraVelocity.y);
        }
        
        lastCameraPos = camera.Position;
        
        // Update plane animation with realistic turning
        plane.Update(deltaTime, cameraVelocity, camera.Front);
        
        // Update particle systems
        // trailSystem.Update(deltaTime);
        
        // Emit contrails from plane engines
        // auto trailPositions = plane.GetTrailPositions();
        // for (const auto& pos : trailPositions) {
        //     trailSystem.Emit(pos, -camera.Front * 5.0f, 1);
        // }
        
        // Update weather system
        if (currentWeather != WeatherType::None) {
            // Switch weather type if needed
            ParticleType newWeatherType = (currentWeather == WeatherType::Rain) ? ParticleType::Rain : ParticleType::Snow;
            
            weatherSystem.Update(deltaTime);
            
            // Emit weather particles around camera
            weatherEmissionTimer += deltaTime;
            if (weatherEmissionTimer > 0.016f) { // ~60 times per second
                float emitRate = (currentWeather == WeatherType::Rain) ? 200.0f : 100.0f;
                int particlesToEmit = static_cast<int>(emitRate * weatherEmissionTimer);
                
                for (int i = 0; i < particlesToEmit; i++) {
                    // Random position in a box above and around camera
                    float x = camera.Position.x + ((rand() % 200) - 100);
                    float y = camera.Position.y + 50.0f + (rand() % 20);
                    float z = camera.Position.z + ((rand() % 200) - 100);
                    
                    glm::vec3 emitPos(x, y, z);
                    glm::vec3 velocity(0, -10, 0);
                    
                    if (currentWeather == WeatherType::Snow) {
                        velocity.y = -2.0f;
                    }
                    
                    weatherSystem.Emit(emitPos, velocity, 1);
                }
                weatherEmissionTimer = 0.0f;
            }
        }

        // Render
        // Dynamic sky color based on time of day
        float dayProgress = sunHeight; // -1 to 1
        glm::vec3 skyColor;
        
        if (dayProgress > 0.3f) {
            // Day time (bright blue)
            skyColor = glm::vec3(0.5f, 0.7f, 0.9f);
        } else if (dayProgress > 0.0f) {
            // Sunrise/Sunset (orange-pink gradient)
            float t = dayProgress / 0.3f;
            glm::vec3 sunsetColor = glm::vec3(1.0f, 0.5f, 0.3f);
            glm::vec3 dayColor = glm::vec3(0.5f, 0.7f, 0.9f);
            skyColor = glm::mix(sunsetColor, dayColor, t);
        } else if (dayProgress > -0.3f) {
            // Dusk (deep orange to dark blue)
            float t = (dayProgress + 0.3f) / 0.3f;
            glm::vec3 nightColor = glm::vec3(0.02f, 0.02f, 0.08f);
            glm::vec3 duskColor = glm::vec3(0.8f, 0.3f, 0.2f);
            skyColor = glm::mix(nightColor, duskColor, t);
        } else {
            // Night (dark blue-black with stars)
            skyColor = glm::vec3(0.02f, 0.02f, 0.08f);
        }
        
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // View/Projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)window.getWidth() / (float)window.getHeight(), 1.0f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Calculate plane position and third person view
        glm::vec3 planePos = camera.Position + camera.Front * 20.0f;
        glm::vec3 planeForward = glm::normalize(camera.Front);
        glm::vec3 cameraOffset = -planeForward * 5.0f + camera.Up * 30.0f;
        glm::vec3 thirdPersonCamPos = planePos + cameraOffset;
        glm::mat4 thirdPersonView = glm::lookAt(
            thirdPersonCamPos,
            planePos + planeForward * 5.0f,
            camera.Up
        );

        // 1. Draw terrain
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
        planeShader.use();
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", thirdPersonView);
        planeShader.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.9f));
        planeShader.setVec3("lightPos", lightPos);
        planeShader.setVec3("viewPos", thirdPersonCamPos);
        plane.Draw(planeShader, planePos, camera.Front, 1.0f);
        
        // 3. Draw Particle Systems
        particleShader.use();
        particleShader.setMat4("view", thirdPersonView);
        particleShader.setMat4("projection", projection);
        // trailSystem.Draw(particleShader.ID);
        if (currentWeather != WeatherType::None) {
            weatherSystem.Draw(particleShader.ID);
        }
        
        // 4. Draw Stars (at night)
        float starVisibility = glm::clamp(-dayProgress * 3.0f, 0.0f, 1.0f);
        if (starVisibility > 0.0f) {
            starsShader.use();
            starsShader.setMat4("view", view);
            starsShader.setMat4("projection", projection);
            stars.Draw(starsShader.ID, starVisibility);
        }

        // 5. Draw Skybox (last)
        skybox.Draw(view, projection);

        window.swapBuffers();
    }

    return 0;
}
