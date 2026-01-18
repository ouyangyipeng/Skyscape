#include "core/Window.h"
#include "graphics/Shader.h"
#include "graphics/Camera.h"
#include "world/InfiniteTerrain.h"
#include "world/Skybox.h"
#include "world/Plane.h"
#include "world/ParticleSystem.h"
#include "world/Stars.h"

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
    std::cout << "[2/6] Loading shaders..." << std::endl;
    Shader terrainShader("assets/shaders/infinite_terrain.vert", "assets/shaders/infinite_terrain.frag");
    Shader planeShader("assets/shaders/plane.vert", "assets/shaders/plane.frag");
    Shader particleShader("assets/shaders/particle.vert", "assets/shaders/particle.frag");
    Shader starsShader("assets/shaders/stars.vert", "assets/shaders/stars.frag");
    std::cout << "[2/6] Shaders loaded" << std::endl;

    // Infinite Terrain (auto-generates as you fly)
    std::cout << "[3/6] Generating terrain..." << std::endl;
    InfiniteTerrain terrain(32, 5); // chunk size 32, view distance 5 chunks (optimized for performance)
    std::cout << "[3/6] Terrain generated" << std::endl;
    
    // Skybox
    std::cout << "[4/6] Loading skybox..." << std::endl;
    std::vector<std::string> faces = {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
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
        
        // Dynamic lighting based on time of day
        glm::vec3 lightColor;
        if (dayProgress > 0.0f) {
            // Day: warm sunlight
            lightColor = glm::vec3(1.0f, 0.95f, 0.9f);
        } else if (dayProgress > -0.2f) {
            // Twilight: orange light
            float t = (dayProgress + 0.2f) / 0.2f;
            lightColor = glm::mix(glm::vec3(0.3f, 0.3f, 0.5f), glm::vec3(1.0f, 0.6f, 0.4f), t);
        } else {
            // Night: moonlight (blue-ish)
            lightColor = glm::vec3(0.3f, 0.3f, 0.5f);
        }
        
        terrainShader.setVec3("lightColor", lightColor);
        terrainShader.setVec3("lightPos", lightPos);
        terrainShader.setVec3("viewPos", camera.Position);
        terrainShader.setFloat("iTime", (float)glfwGetTime());
        terrain.Draw(terrainShader);

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
