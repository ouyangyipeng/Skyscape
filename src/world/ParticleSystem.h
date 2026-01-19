#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float life;      // Remaining life in seconds
    float size;
};

enum class ParticleType {
    Trail,    // Airplane contrail
    Rain,     // Rain drops
    Snow      // Snowflakes
};

class ParticleSystem {
public:
    ParticleSystem(ParticleType type, int maxParticles = 1000);
    ~ParticleSystem();

    void Update(float deltaTime);
    void Emit(const glm::vec3& position, const glm::vec3& direction, int count = 1);
    void Draw(unsigned int shaderProgram);
    
    void SetEmissionRate(float particlesPerSecond) { m_EmissionRate = particlesPerSecond; }
    void SetParticleLife(float life) { m_ParticleLife = life; }
    void SetParticleSize(float size) { m_ParticleSize = size; }
    void SetGravity(const glm::vec3& gravity) { m_Gravity = gravity; }

    ParticleType GetType() const { return m_Type; }
    
private:
    void InitRenderData();
    int FindUnusedParticle();
    
    ParticleType m_Type;
    std::vector<Particle> m_Particles;
    int m_MaxParticles;
    int m_LastUsedParticle;
    
    // Emitter properties
    float m_EmissionRate;
    float m_ParticleLife;
    float m_ParticleSize;
    glm::vec3 m_Gravity;
    
    // Rendering
    unsigned int m_VAO, m_VBO;
};
