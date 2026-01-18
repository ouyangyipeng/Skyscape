#pragma once
#include <glm/glm.hpp>
#include <vector>

class Plane {
public:
    Plane();
    void Update(float deltaTime, glm::vec3 velocity, glm::vec3 targetDirection);
    void Draw(class Shader& shader, glm::vec3 position, glm::vec3 direction, float scale = 1.0f);
    
    // Get positions for contrail emission (wingtips and engines)
    std::vector<glm::vec3> GetTrailPositions() const;

private:
    unsigned int VAO, VBO;
    int m_VertexCount = 0;
    void setupMesh();
    
    // Animation state
    float m_Time = 0.0f;
    float m_RollAngle = 0.0f;
    float m_PitchAngle = 0.0f;
    glm::vec3 m_LastVelocity = glm::vec3(0.0f);
    
    // Physics state for realistic turning
    glm::vec3 m_CurrentDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    float m_CurrentSpeed = 0.0f;
    
    // Cached for trail emission
    glm::vec3 m_CurrentPosition = glm::vec3(0.0f);
    float m_CurrentScale = 1.0f;
};
