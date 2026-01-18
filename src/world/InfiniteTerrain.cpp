#include <iostream>
#include <stb_image.h>
#include "InfiniteTerrain.h"
#include <glad/glad.h>
#include <cmath>
#include "../graphics/Shader.h"
#include <glm/gtc/matrix_transform.hpp>

InfiniteTerrain::InfiniteTerrain(int chunkSize, int viewDistance)
    : m_ChunkSize(chunkSize), m_ViewDistance(viewDistance) {
    LoadTerrainTextures();
}

// 加载贴图工具
void InfiniteTerrain::LoadTerrainTextures() {
    std::cout << "[InfiniteTerrain] LoadTerrainTextures() called" << std::endl;
    int w, h, ch;
    stbi_set_flip_vertically_on_load(0);
    std::cout << "[InfiniteTerrain] Loading snow texture..." << std::endl;
    // Snow - force load as RGB (convert grayscale to RGB)
    unsigned char* snowData = stbi_load("assets/textures/snow/Snow009C_1K-PNG_AmbientOcclusion.png", &w, &h, &ch, 3);
    std::cout << "[InfiniteTerrain] stbi_load returned: " << (snowData ? "SUCCESS" : "FAILED") << std::endl;
    glGenTextures(1, &m_SnowTex);
    std::cout << "[InfiniteTerrain] glGenTextures snow: " << m_SnowTex << std::endl;
    glBindTexture(GL_TEXTURE_2D, m_SnowTex);
    std::cout << "[InfiniteTerrain] glBindTexture snow OK" << std::endl;
    if (snowData) {
        std::cout << "[InfiniteTerrain] Snow texture size: " << w << "x" << h << " forced RGB" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, snowData);
        std::cout << "[InfiniteTerrain] glTexImage2D snow OK" << std::endl;
        stbi_image_free(snowData);
        std::cout << "[InfiniteTerrain] Snow texture loaded successfully." << std::endl;
    } else {
        std::cerr << "[InfiniteTerrain] Failed to load snow texture! Using fallback white." << std::endl;
        unsigned char white[4] = {255,255,255,255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    // Rock - force RGB
    stbi_set_flip_vertically_on_load(0);
    unsigned char* rockData = stbi_load("assets/textures/rock/aerial_rocks_04_diff_4k.jpg", &w, &h, &ch, 3);
    glGenTextures(1, &m_RockTex);
    glBindTexture(GL_TEXTURE_2D, m_RockTex);
    if (rockData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, rockData);
        stbi_image_free(rockData);
        std::cout << "[InfiniteTerrain] Rock texture loaded successfully." << std::endl;
    } else {
        std::cerr << "[InfiniteTerrain] Failed to load rock texture! Using fallback gray." << std::endl;
        unsigned char gray[4] = {128,128,128,255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, gray);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    // Water - force RGB
    std::cout << "[InfiniteTerrain] Loading water texture..." << std::endl;
    unsigned char* waterData = stbi_load("assets/textures/river/clear-ocean-water-texture.jpg", &w, &h, &ch, 3);
    std::cout << "[InfiniteTerrain] Water stbi_load result: " << (waterData ? "SUCCESS" : "FAILED") << std::endl;
    if (!waterData) {
        std::cerr << "[InfiniteTerrain] STB Error: " << stbi_failure_reason() << std::endl;
    }
    glGenTextures(1, &m_WaterTex);
    glBindTexture(GL_TEXTURE_2D, m_WaterTex);
    if (waterData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, waterData);
        stbi_image_free(waterData);
        std::cout << "[InfiniteTerrain] Water texture loaded successfully." << std::endl;
    } else {
        std::cerr << "[InfiniteTerrain] Failed to load water texture! Creating procedural water." << std::endl;
        // Create a 64x64 procedural water texture with variation
        const int texSize = 64;
        std::vector<unsigned char> waterTex(texSize * texSize * 3);
        for (int y = 0; y < texSize; y++) {
            for (int x = 0; x < texSize; x++) {
                int idx = (y * texSize + x) * 3;
                float variation = (sin(x * 0.3f) + cos(y * 0.3f)) * 20.0f;
                int r = int(25 + variation);
                int g = int(85 + variation);
                int b = int(160 + variation);
                waterTex[idx + 0] = r < 0 ? 0 : (r > 255 ? 255 : r);
                waterTex[idx + 1] = g < 0 ? 0 : (g > 255 ? 255 : g);
                waterTex[idx + 2] = b < 0 ? 0 : (b > 255 ? 255 : b);
            }
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, waterTex.data());
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    std::cout << "[InfiniteTerrain] Texture loading complete." << std::endl;
}

InfiniteTerrain::~InfiniteTerrain() {
    for (auto& pair : m_Chunks) {
        glDeleteVertexArrays(1, &pair.second.VAO);
        glDeleteBuffers(1, &pair.second.VBO);
        glDeleteBuffers(1, &pair.second.EBO);
    }
}

// --- 更仿真的分形布朗运动（fbm）噪声 ---
// 基础伪随机hash
static float hash(float n) {
    return std::fmod(std::sin(n) * 43758.5453f, 1.0f);
}

// 2D value noise
static float valueNoise(float x, float z) {
    int ix = (int)std::floor(x);
    int iz = (int)std::floor(z);
    float fx = x - ix;
    float fz = z - iz;
    // 四角hash
    float v00 = hash(ix * 49632 + iz * 325176);
    float v10 = hash((ix+1) * 49632 + iz * 325176);
    float v01 = hash(ix * 49632 + (iz+1) * 325176);
    float v11 = hash((ix+1) * 49632 + (iz+1) * 325176);
    // 双线性插值
    float u = fx * fx * (3.0f - 2.0f * fx);
    float v = fz * fz * (3.0f - 2.0f * fz);
    float a = v00 * (1-u) + v10 * u;
    float b = v01 * (1-u) + v11 * u;
    return a * (1-v) + b * v;
}

// fbm多层叠加
float InfiniteTerrain::Noise(float x, float z) const {
    float amplitude = 1.0f;
    float frequency = 0.005f;
    float maxAmp = 0.0f;
    float sum = 0.0f;
    for (int i = 0; i < 6; ++i) {
        sum += valueNoise(x * frequency, z * frequency) * amplitude;
        maxAmp += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }
    // 控制地形起伏
    return sum / maxAmp * 100.0f - 10.0f;
}

glm::vec3 InfiniteTerrain::GetTerrainColor(float height) const {
    // 更真实的分层与颜色，增加坡度影响
    // 这里不直接用坡度，但为shader细节做准备
    if (height > 70.0f) {
        // 雪山顶
        return glm::vec3(0.97f, 0.97f, 0.99f);
    } else if (height > 55.0f) {
        // 雪与岩石过渡
        float t = (height - 55.0f) / 15.0f;
        return glm::mix(glm::vec3(0.7f, 0.7f, 0.75f), glm::vec3(0.97f, 0.97f, 0.99f), t);
    } else if (height > 40.0f) {
        // 岩石山体
        float t = (height - 40.0f) / 15.0f;
        return glm::mix(glm::vec3(0.5f, 0.4f, 0.35f), glm::vec3(0.7f, 0.7f, 0.75f), t);
    } else if (height > 25.0f) {
        // 森林（深绿）
        float t = (height - 25.0f) / 15.0f;
        return glm::mix(glm::vec3(0.18f, 0.32f, 0.13f), glm::vec3(0.2f, 0.4f, 0.15f), t);
    } else if (height > 8.0f) {
        // 草地（鲜绿）
        float t = (height - 8.0f) / 17.0f;
        return glm::mix(glm::vec3(0.36f, 0.7f, 0.22f), glm::vec3(0.18f, 0.32f, 0.13f), t);
    } else if (height > 0.0f) {
        // 低地草原
        return glm::vec3(0.36f, 0.7f, 0.22f);
    } else if (height > -4.0f) {
        // 沙滩
        return glm::vec3(0.85f, 0.8f, 0.55f);
    } else {
        // 水体
        return glm::vec3(0.13f, 0.32f, 0.65f);
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
    // 绑定贴图到纹理单元0/1/2，并传递给shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SnowTex);
    shader.setInt("snowTex", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_RockTex);
    shader.setInt("rockTex", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_WaterTex);
    shader.setInt("waterTex", 2);
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    for (auto& pair : m_Chunks) {
        glBindVertexArray(pair.second.VAO);
        glDrawElements(GL_TRIANGLES, pair.second.indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
    // 解绑
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
}
