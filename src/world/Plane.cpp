#include "Plane.h"
#include <glad/glad.h>
#include <vector>
#include "../graphics/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

Plane::Plane() {
    setupMesh();
}

void Plane::Update(float deltaTime, glm::vec3 velocity, glm::vec3 targetDirection) {
    m_Time += deltaTime;
    
    // Calculate current speed
    m_CurrentSpeed = glm::length(velocity);
    
    // Realistic turning with radius constraint
    // Normalize target direction
    if (glm::length(targetDirection) > 0.01f) {
        targetDirection = glm::normalize(targetDirection);
        
        // Calculate turn rate based on speed (faster = wider turns)
        float maxTurnRate = 1.2f / (1.0f + m_CurrentSpeed * 0.001f); // radians per second
        
        // Smoothly interpolate direction with turn rate limit
        glm::vec3 toTarget = targetDirection - m_CurrentDirection;
        float turnAngle = glm::length(toTarget);
        
        if (turnAngle > 0.01f) {
            float actualTurn = (turnAngle < maxTurnRate * deltaTime) ? turnAngle : (maxTurnRate * deltaTime);
            m_CurrentDirection = glm::normalize(m_CurrentDirection + toTarget * (actualTurn / turnAngle));
        }
    }
    
    // Calculate roll based on horizontal velocity change (turning)
    glm::vec3 velocityChange = velocity - m_LastVelocity;
    float lateralAccel = velocityChange.x / (deltaTime + 0.001f);
    
    // Enhanced roll based on turn rate
    glm::vec3 right = glm::cross(m_CurrentDirection, glm::vec3(0, 1, 0));
    float turnInfluence = glm::dot(velocityChange, right) * 0.5f;
    
    float targetRoll = glm::clamp((lateralAccel * -0.15f) + turnInfluence, -35.0f, 35.0f);
    m_RollAngle = glm::mix(m_RollAngle, targetRoll, deltaTime * 2.0f);
    
    // Calculate pitch based on vertical velocity change
    float verticalAccel = velocityChange.y / (deltaTime + 0.001f);
    float targetPitch = glm::clamp(verticalAccel * -0.1f, -15.0f, 15.0f);
    m_PitchAngle = glm::mix(m_PitchAngle, targetPitch, deltaTime * 1.5f);
    
    m_LastVelocity = velocity;
}

void Plane::setupMesh() {
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<float> vertices; // Final buffer: pos(3) + normal(3)

    
    std::ifstream file("assets/models/airplane4.obj");
    if (!file.is_open()) {
        std::cerr << "Failed to open airplane.obj" << std::endl;
        return;
    }

    std::string line;
     int validFaces = 0;
    int invalidFaces = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }else if (prefix == "vn") {
            glm::vec3 normal;
            if (ss >> normal.x >> normal.y >> normal.z) {
                temp_normals.push_back(normal);
            }
        } 
        
        else if (prefix == "f") {
            std::vector<int> faceIndices;
            std::string segment;
            while (ss >> segment) {
                std::stringstream segmentSS(segment);
                std::string vStr;
                std::getline(segmentSS, vStr, '/');
                try {
                    int idx = std::stoi(vStr) - 1; // OBJ索引从1开始
                    
                    // 关键修复：验证索引是否有效
                    if (idx >= 0 && idx < static_cast<int>(temp_vertices.size())) {
                        faceIndices.push_back(idx);
                    } else {
                        std::cerr << "[Plane] Invalid vertex index: " << (idx + 1) 
                                  << " (max: " << temp_vertices.size() << ")" << std::endl;
                        faceIndices.clear(); // 清空无效面
                        break;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "[Plane] Parse error: " << e.what() << std::endl;
                    faceIndices.clear();
                    break;
                }
            }

            if (faceIndices.size() >= 3) {
                validFaces++;
                for (size_t i = 1; i < faceIndices.size() - 1; ++i) {
                    glm::vec3 v0 = temp_vertices[faceIndices[0]];
                    glm::vec3 v1 = temp_vertices[faceIndices[i]];
                    glm::vec3 v2 = temp_vertices[faceIndices[i + 1]];

                    glm::vec3 edge1 = v1 - v0;
                    glm::vec3 edge2 = v2 - v0;
                    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

                    vertices.push_back(v0.x); vertices.push_back(v0.y); vertices.push_back(v0.z);
                    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

                    vertices.push_back(v1.x); vertices.push_back(v1.y); vertices.push_back(v1.z);
                    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

                    vertices.push_back(v2.x); vertices.push_back(v2.y); vertices.push_back(v2.z);
                    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);
                }
            }
        }
    }

    m_VertexCount = vertices.size() / 6;

    // 详细的调试输出
    std::cout << "[Plane] ========== OBJ Load Summary ==========" << std::endl;
    std::cout << "[Plane] Source vertices: " << temp_vertices.size() << std::endl;
    std::cout << "[Plane] Source normals: " << temp_normals.size() << std::endl;
    std::cout << "[Plane] Valid faces: " << validFaces << std::endl;
    std::cout << "[Plane] Invalid faces: " << invalidFaces << std::endl;
    std::cout << "[Plane] Final vertex count: " << m_VertexCount << std::endl;
    std::cout << "[Plane] Triangle count: " << m_VertexCount / 3 << std::endl;
    std::cout << "[Plane] ======================================" << std::endl;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Plane::Draw(Shader& shader, glm::vec3 position, glm::vec3 direction, float scale) {
    // Cache for trail emission
    m_CurrentPosition = position;
    m_CurrentDirection = direction;
    m_CurrentScale = scale;
    
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    // Simple rotation to face direction (assuming default is +Z)
    // For a real flight sim, we'd use quaternions or a full orientation matrix
    // Here we just align it to the camera front vector roughly
    
    // Calculate rotation based on direction
    glm::vec3 defaultDir(0.0f, 0.0f, -1.0f); // Our plane points -Z by default? No, vertices are +Z nose.
    // Vertices: Nose is (0,0,1). So it points +Z.
    // Camera Front is usually -Z. 
    // Let's rotate it to match direction.
    
    // Simplified: Just translate for now, maybe rotate later if we have orientation.
    // If we want it to follow the camera, we can just use the camera's position + offset.
    
    // Let's assume the plane is "the player" and we are looking at it, or we are inside it.
    // If we are inside, we don't draw it.
    // If we are 3rd person, we draw it in front of camera.
    
    // Let's just draw it at 'position' with 'scale'
    model = glm::scale(model, glm::vec3(scale));
    
    // Base rotation to make plane horizontal first
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    
    // Calculate yaw and pitch from direction vector
    float yaw = atan2(direction.x, direction.z);
    float basePitch = asin(-direction.y);
    
    // Apply yaw rotation (turn left/right)
    model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Apply pitch with animation
    float animatedPitch = basePitch + glm::radians(m_PitchAngle);
    // Add very subtle bobbing effect
    animatedPitch += sin(m_Time * 1.5f) * 0.01f;
    model = glm::rotate(model, animatedPitch, glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Apply roll with animation
    float animatedRoll = glm::radians(m_RollAngle);
    // Add very subtle swaying effect
    animatedRoll += sin(m_Time * 1.2f) * 0.015f;
    model = glm::rotate(model, animatedRoll, glm::vec3(0.0f, 0.0f, 1.0f));

    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    glBindVertexArray(0);
}

std::vector<glm::vec3> Plane::GetTrailPositions() const {
    // Calculate trail emission points (engine/wingtips)
    // Approximate positions for F-22: two engines at the back
    
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(m_CurrentDirection, up));
    
    // Two engine positions behind the plane
    float engineOffset = 2.0f * m_CurrentScale;
    float engineSeparation = 3.0f * m_CurrentScale;
    
    std::vector<glm::vec3> positions;
    positions.push_back(m_CurrentPosition - m_CurrentDirection * engineOffset + right * engineSeparation);
    positions.push_back(m_CurrentPosition - m_CurrentDirection * engineOffset - right * engineSeparation);
    
    return positions;
}
