#include "ParticleSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

ParticleSystem::ParticleSystem(ParticleType type, int maxParticles)
    : m_Type(type), m_MaxParticles(maxParticles), m_LastUsedParticle(0)
{
    m_Particles.resize(maxParticles);
    
    // Set default properties based on type
    switch (type) {
        case ParticleType::Trail:
            m_EmissionRate = 20.0f;
            m_ParticleLife = 5.0f;
            m_ParticleSize = 15.0f;
            m_Gravity = glm::vec3(0.0f, -0.3f, 0.0f);
            break;
        case ParticleType::Rain:
            m_EmissionRate = 200.0f;
            m_ParticleLife = 5.0f;
            m_ParticleSize = 2.0f;
            m_Gravity = glm::vec3(0.0f, -20.0f, 0.0f);
            break;
        case ParticleType::Snow:
            m_EmissionRate = 100.0f;
            m_ParticleLife = 8.0f;
            m_ParticleSize = 4.0f;
            m_Gravity = glm::vec3(0.0f, -2.0f, 0.0f);
            break;
    }
    
    InitRenderData();
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void ParticleSystem::InitRenderData() {
    // Setup VAO/VBO for instanced rendering
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_MaxParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    
    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    
    // Size
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
    
    glBindVertexArray(0);
}

int ParticleSystem::FindUnusedParticle() {
    // Search from last used particle
    for (int i = m_LastUsedParticle; i < m_MaxParticles; i++) {
        if (m_Particles[i].life <= 0.0f) {
            m_LastUsedParticle = i;
            return i;
        }
    }
    
    // Search from beginning
    for (int i = 0; i < m_LastUsedParticle; i++) {
        if (m_Particles[i].life <= 0.0f) {
            m_LastUsedParticle = i;
            return i;
        }
    }
    
    m_LastUsedParticle = 0;
    return 0;
}

void ParticleSystem::Emit(const glm::vec3& position, const glm::vec3& direction, int count) {
    for (int i = 0; i < count; i++) {
        int idx = FindUnusedParticle();
        
        // Random spread (larger for trail)
        float spreadFactor = (m_Type == ParticleType::Trail) ? 100.0f : 500.0f;
        float spreadX = ((rand() % 100) - 50) / spreadFactor;
        float spreadY = ((rand() % 100) - 50) / spreadFactor;
        float spreadZ = ((rand() % 100) - 50) / spreadFactor;
        
        m_Particles[idx].position = position;
        m_Particles[idx].velocity = direction + glm::vec3(spreadX, spreadY, spreadZ);
        m_Particles[idx].life = m_ParticleLife;
        m_Particles[idx].size = m_ParticleSize;
        
        // Set color based on type
        switch (m_Type) {
            case ParticleType::Trail:
                // Softer blue-gray color like real contrails
                m_Particles[idx].color = glm::vec4(0.85f, 0.9f, 0.95f, 0.5f);
                break;
            case ParticleType::Rain:
                m_Particles[idx].color = glm::vec4(0.7f, 0.8f, 1.0f, 0.6f);
                break;
            case ParticleType::Snow:
                m_Particles[idx].color = glm::vec4(1.0f, 1.0f, 1.0f, 0.9f);
                break;
        }
    }
}

void ParticleSystem::Update(float deltaTime) {
    for (auto& particle : m_Particles) {
        if (particle.life > 0.0f) {
            particle.life -= deltaTime;
            particle.velocity += m_Gravity * deltaTime;
            particle.position += particle.velocity * deltaTime;
            
            // Fade out
            float lifeRatio = particle.life / m_ParticleLife;
            if (m_Type == ParticleType::Trail) {
                // Trail expands and fades gradually
                particle.color.a = lifeRatio * 0.5f;
                particle.size = m_ParticleSize * (1.0f + (1.0f - lifeRatio) * 0.8f);
            } else {
                particle.color.a = lifeRatio * 0.6f;
            }
            
            // Add some randomness for snow
            if (m_Type == ParticleType::Snow) {
                particle.velocity.x += sin(particle.life * 3.0f) * 0.5f * deltaTime;
                particle.velocity.z += cos(particle.life * 2.5f) * 0.5f * deltaTime;
            }
        }
    }
}

void ParticleSystem::Draw(unsigned int shaderProgram) {
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    
    // Enable point sprites
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_Particles.size() * sizeof(Particle), m_Particles.data());
    
    // Draw
    glUseProgram(shaderProgram);
    glBindVertexArray(m_VAO);
    
    int aliveCount = 0;
    for (const auto& p : m_Particles) {
        if (p.life > 0.0f) aliveCount++;
    }
    
    if (aliveCount > 0) {
        glDrawArrays(GL_POINTS, 0, m_Particles.size());
    }
    
    glBindVertexArray(0);
    
    // Restore state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);
}
