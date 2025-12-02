#include "InfiniteTerrain.h"
#include <glad/glad.h>
#include <cmath>
#include "../graphics/Shader.h"
#include <glm/gtc/matrix_transform.hpp>

InfiniteTerrain::InfiniteTerrain(int chunkSize, int viewDistance)
    : m_ChunkSize(chunkSize), m_ViewDistance(viewDistance) {
}

InfiniteTerrain::~InfiniteTerrain() {
    for (auto& pair : m_Chunks) {
        glDeleteVertexArrays(1, &pair.second.VAO);
        glDeleteBuffers(1, &pair.second.VBO);
        glDeleteBuffers(1, &pair.second.EBO);
    }
}

// Simple noise function (Perlin-like)
float InfiniteTerrain::Noise(float x, float z) const {
    // Multi-octave noise for realistic terrain
    float height = 0.0f;
    
    // Large mountains
    height += sin(x * 0.005f) * cos(z * 0.005f) * 80.0f;
    height += sin(x * 0.01f + 1.5f) * cos(z * 0.012f + 0.8f) * 40.0f;
    // Medium hills
    height += sin(x * 0.025f + 3.0f) * cos(z * 0.028f + 2.0f) * 15.0f;
    // Small details
    height += sin(x * 0.08f) * cos(z * 0.08f) * 5.0f;
    height += sin(x * 0.15f + 0.5f) * cos(z * 0.15f + 0.3f) * 2.0f;
    
    return height;
}

glm::vec3 InfiniteTerrain::GetTerrainColor(float height) const {
    // Height-based coloring for realistic terrain
    if (height > 60.0f) {
        // Snow caps
        return glm::vec3(0.95f, 0.95f, 0.97f);
    } else if (height > 40.0f) {
        // Rocky mountains (gray/brown)
        float t = (height - 40.0f) / 20.0f;
        return glm::mix(glm::vec3(0.5f, 0.4f, 0.35f), glm::vec3(0.7f, 0.7f, 0.75f), t);
    } else if (height > 15.0f) {
        // Forest (dark green)
        return glm::vec3(0.2f, 0.4f, 0.15f);
    } else if (height > 0.0f) {
        // Grassland (green)
        return glm::vec3(0.3f, 0.55f, 0.2f);
    } else if (height > -5.0f) {
        // Beach/sand
        return glm::vec3(0.76f, 0.7f, 0.5f);
    } else {
        // Water (blue)
        return glm::vec3(0.1f, 0.3f, 0.6f);
    }
}

float InfiniteTerrain::GetHeight(float x, float z) const {
    return Noise(x, z);
}

TerrainChunk InfiniteTerrain::GenerateChunk(int chunkX, int chunkZ) {
    TerrainChunk chunk;
    chunk.worldPos = glm::vec3(chunkX * m_ChunkSize, 0, chunkZ * m_ChunkSize);
    
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    float worldOffsetX = chunkX * m_ChunkSize;
    float worldOffsetZ = chunkZ * m_ChunkSize;
    
    // Generate vertices
    for (int z = 0; z <= m_ChunkSize; ++z) {
        for (int x = 0; x <= m_ChunkSize; ++x) {
            float worldX = worldOffsetX + x;
            float worldZ = worldOffsetZ + z;
            float height = Noise(worldX, worldZ);
            
            // Position
            vertices.push_back(worldX);
            vertices.push_back(height);
            vertices.push_back(worldZ);
            
            // Calculate normal using central differences
            float hL = Noise(worldX - 1, worldZ);
            float hR = Noise(worldX + 1, worldZ);
            float hD = Noise(worldX, worldZ - 1);
            float hU = Noise(worldX, worldZ + 1);
            glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            
            // Color based on height
            glm::vec3 color = GetTerrainColor(height);
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);
        }
    }
    
    // Generate indices
    int vertsPerRow = m_ChunkSize + 1;
    for (int z = 0; z < m_ChunkSize; ++z) {
        for (int x = 0; x < m_ChunkSize; ++x) {
            unsigned int topLeft = z * vertsPerRow + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * vertsPerRow + x;
            unsigned int bottomRight = bottomLeft + 1;
            
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    chunk.indexCount = indices.size();
    
    glGenVertexArrays(1, &chunk.VAO);
    glGenBuffers(1, &chunk.VBO);
    glGenBuffers(1, &chunk.EBO);
    
    glBindVertexArray(chunk.VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, chunk.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Color (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    return chunk;
}

void InfiniteTerrain::Update(glm::vec3 cameraPos) {
    int camChunkX = (int)floor(cameraPos.x / m_ChunkSize);
    int camChunkZ = (int)floor(cameraPos.z / m_ChunkSize);
    
    // Generate chunks around camera
    for (int z = camChunkZ - m_ViewDistance; z <= camChunkZ + m_ViewDistance; ++z) {
        for (int x = camChunkX - m_ViewDistance; x <= camChunkX + m_ViewDistance; ++x) {
            ChunkKey key{x, z};
            if (m_Chunks.find(key) == m_Chunks.end()) {
                m_Chunks[key] = GenerateChunk(x, z);
            }
        }
    }
    
    // Remove far chunks to save memory
    std::vector<ChunkKey> toRemove;
    for (auto& pair : m_Chunks) {
        int dx = abs(pair.first.x - camChunkX);
        int dz = abs(pair.first.z - camChunkZ);
        if (dx > m_ViewDistance + 2 || dz > m_ViewDistance + 2) {
            toRemove.push_back(pair.first);
        }
    }
    for (auto& key : toRemove) {
        glDeleteVertexArrays(1, &m_Chunks[key].VAO);
        glDeleteBuffers(1, &m_Chunks[key].VBO);
        glDeleteBuffers(1, &m_Chunks[key].EBO);
        m_Chunks.erase(key);
    }
}

void InfiniteTerrain::Draw(Shader& shader) {
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    
    for (auto& pair : m_Chunks) {
        glBindVertexArray(pair.second.VAO);
        glDrawElements(GL_TRIANGLES, pair.second.indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}
