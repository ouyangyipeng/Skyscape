#include "Stars.h"
#include <cstdlib>
#include <ctime>

Stars::Stars(int count) : m_StarCount(count) {
    GenerateStars();
}

Stars::~Stars() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Stars::GenerateStars() {
    std::vector<float> starData;
    srand(12345); // Fixed seed for consistent star positions
    
    for (int i = 0; i < m_StarCount; i++) {
        // Random position on sphere
        float theta = ((float)rand() / RAND_MAX) * 3.14159f * 2.0f;
        float phi = ((float)rand() / RAND_MAX) * 3.14159f;
        
        float radius = 5000.0f;
        float x = radius * sin(phi) * cos(theta);
        float y = radius * cos(phi);
        float z = radius * sin(phi) * sin(theta);
        
        // Only stars above horizon (y > 0) for upper hemisphere
        if (y > 0) {
            starData.push_back(x);
            starData.push_back(y);
            starData.push_back(z);
            
            // Random brightness
            float brightness = 0.5f + ((float)rand() / RAND_MAX) * 0.5f;
            starData.push_back(brightness);
            
            // Random size
            float size = 1.0f + ((float)rand() / RAND_MAX) * 2.0f;
            starData.push_back(size);
        }
    }
    
    m_StarCount = starData.size() / 5;
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, starData.size() * sizeof(float), starData.data(), GL_STATIC_DRAW);
    
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    // Brightness
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // Size
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));
    
    glBindVertexArray(0);
}

void Stars::Draw(unsigned int shaderProgram, float visibility) {
    if (visibility <= 0.0f) return;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "starVisibility"), visibility);
    
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, m_StarCount);
    glBindVertexArray(0);
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);
}
