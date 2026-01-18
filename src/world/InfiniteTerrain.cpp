#include <iostream>
#include <stb_image.h>
#include "InfiniteTerrain.h"
#include <glad/glad.h>
#include <cmath>
#include "../graphics/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <random>
#include <iostream>

// ✅ 在构造函数中初始化花丛网格
InfiniteTerrain::InfiniteTerrain(int chunkSize, int viewDistance)
    : m_ChunkSize(chunkSize), m_ViewDistance(viewDistance), 
      m_TreeVAO(0), m_TreeVBO(0), m_TreeVertexCount(0),
      m_CabinVAO(0), m_CabinVBO(0), m_CabinVertexCount(0),
      m_FlowerVAO(0), m_FlowerVBO(0), m_FlowerVertexCount(0),
      m_BoatVAO(0), m_BoatVBO(0), m_BoatVertexCount(0) { // ✅ 添加小船初始化
    InitializeTreeMesh();
    InitializeCabinMesh();
    InitializeFlowerMesh();
    InitializeBoatMesh(); // ✅ 初始化小船模型
}

// ✅ 在析构函数中清理花丛资源
InfiniteTerrain::~InfiniteTerrain() {
    for (auto& pair : m_Chunks) {
        glDeleteVertexArrays(1, &pair.second.VAO);
        glDeleteBuffers(1, &pair.second.VBO);
        glDeleteBuffers(1, &pair.second.EBO);
    }

    if (m_TreeVAO) glDeleteVertexArrays(1, &m_TreeVAO);
    if (m_TreeVBO) glDeleteBuffers(1, &m_TreeVBO);
    
    if (m_CabinVAO) glDeleteVertexArrays(1, &m_CabinVAO);
    if (m_CabinVBO) glDeleteBuffers(1, &m_CabinVBO);
    
    // ✅ 清理花丛资源
    if (m_FlowerVAO) glDeleteVertexArrays(1, &m_FlowerVAO);
    if (m_FlowerVBO) glDeleteBuffers(1, &m_FlowerVBO);

    if (m_BoatVAO) glDeleteVertexArrays(1, &m_BoatVAO);
    if (m_BoatVBO) glDeleteBuffers(1, &m_BoatVBO);
}

// ✅ InitializeTreeMesh（保持不变）
void InfiniteTerrain::InitializeTreeMesh() {
    std::vector<float> treeVertices;
    
    // 改进的树干（6面立方体，更立体）
    float trunk[] = {
        // 前面
        -0.3f, 0.0f,  0.3f,  0.45f, 0.25f, 0.12f,
         0.3f, 0.0f,  0.3f,  0.45f, 0.25f, 0.12f,
         0.3f, 3.0f,  0.3f,  0.40f, 0.22f, 0.10f,
        -0.3f, 0.0f,  0.3f,  0.45f, 0.25f, 0.12f,
         0.3f, 3.0f,  0.3f,  0.40f, 0.22f, 0.10f,
        -0.3f, 3.0f,  0.3f,  0.40f, 0.22f, 0.10f,
        
        // 后面
        -0.3f, 0.0f, -0.3f,  0.42f, 0.23f, 0.11f,
         0.3f, 3.0f, -0.3f,  0.38f, 0.20f, 0.09f,
         0.3f, 0.0f, -0.3f,  0.42f, 0.23f, 0.11f,
        -0.3f, 0.0f, -0.3f,  0.42f, 0.23f, 0.11f,
        -0.3f, 3.0f, -0.3f,  0.38f, 0.20f, 0.09f,
         0.3f, 3.0f, -0.3f,  0.38f, 0.20f, 0.09f,
        
        // 左侧
        -0.3f, 0.0f, -0.3f,  0.40f, 0.21f, 0.10f,
        -0.3f, 0.0f,  0.3f,  0.40f, 0.21f, 0.10f,
        -0.3f, 3.0f,  0.3f,  0.36f, 0.19f, 0.08f,
        -0.3f, 0.0f, -0.3f,  0.40f, 0.21f, 0.10f,
        -0.3f, 3.0f,  0.3f,  0.36f, 0.19f, 0.08f,
        -0.3f, 3.0f, -0.3f,  0.36f, 0.19f, 0.08f,
        
        // 右侧
         0.3f, 0.0f, -0.3f,  0.44f, 0.24f, 0.12f,
         0.3f, 3.0f,  0.3f,  0.39f, 0.21f, 0.09f,
         0.3f, 0.0f,  0.3f,  0.44f, 0.24f, 0.12f,
         0.3f, 0.0f, -0.3f,  0.44f, 0.24f, 0.12f,
         0.3f, 3.0f, -0.3f,  0.39f, 0.21f, 0.09f,
         0.3f, 3.0f,  0.3f,  0.39f, 0.21f, 0.09f,
    };
    
    // 改进的树冠（3层圆锥）
    float foliage[] = {
        // 第一层
         0.0f,  6.0f,  0.0f,  0.15f, 0.55f, 0.15f,
        -2.5f,  3.0f, -2.5f,  0.10f, 0.45f, 0.10f,
         2.5f,  3.0f, -2.5f,  0.10f, 0.45f, 0.10f,
         0.0f,  6.0f,  0.0f,  0.15f, 0.55f, 0.15f,
         2.5f,  3.0f, -2.5f,  0.10f, 0.45f, 0.10f,
         2.5f,  3.0f,  2.5f,  0.10f, 0.45f, 0.10f,
         0.0f,  6.0f,  0.0f,  0.15f, 0.55f, 0.15f,
         2.5f,  3.0f,  2.5f,  0.10f, 0.45f, 0.10f,
        -2.5f,  3.0f,  2.5f,  0.10f, 0.45f, 0.10f,
         0.0f,  6.0f,  0.0f,  0.15f, 0.55f, 0.15f,
        -2.5f,  3.0f,  2.5f,  0.10f, 0.45f, 0.10f,
        -2.5f,  3.0f, -2.5f,  0.10f, 0.45f, 0.10f,
        
        // 第二层
         0.0f,  7.5f,  0.0f,  0.18f, 0.60f, 0.18f,
        -1.8f,  4.5f, -1.8f,  0.12f, 0.50f, 0.12f,
         1.8f,  4.5f, -1.8f,  0.12f, 0.50f, 0.12f,
         0.0f,  7.5f,  0.0f,  0.18f, 0.60f, 0.18f,
         1.8f,  4.5f, -1.8f,  0.12f, 0.50f, 0.12f,
         1.8f,  4.5f,  1.8f,  0.12f, 0.50f, 0.12f,
         0.0f,  7.5f,  0.0f,  0.18f, 0.60f, 0.18f,
         1.8f,  4.5f,  1.8f,  0.12f, 0.50f, 0.12f,
        -1.8f,  4.5f,  1.8f,  0.12f, 0.50f, 0.12f,
         0.0f,  7.5f,  0.0f,  0.18f, 0.60f, 0.18f,
        -1.8f,  4.5f,  1.8f,  0.12f, 0.50f, 0.12f,
        -1.8f,  4.5f, -1.8f,  0.12f, 0.50f, 0.12f,
        
        // 第三层
         0.0f,  8.5f,  0.0f,  0.20f, 0.65f, 0.20f,
        -1.0f,  6.0f, -1.0f,  0.14f, 0.55f, 0.14f,
         1.0f,  6.0f, -1.0f,  0.14f, 0.55f, 0.14f,
         0.0f,  8.5f,  0.0f,  0.20f, 0.65f, 0.20f,
         1.0f,  6.0f, -1.0f,  0.14f, 0.55f, 0.14f,
         1.0f,  6.0f,  1.0f,  0.14f, 0.55f, 0.14f,
         0.0f,  8.5f,  0.0f,  0.20f, 0.65f, 0.20f,
         1.0f,  6.0f,  1.0f,  0.14f, 0.55f, 0.14f,
        -1.0f,  6.0f,  1.0f,  0.14f, 0.55f, 0.14f,
         0.0f,  8.5f,  0.0f,  0.20f, 0.65f, 0.20f,
        -1.0f,  6.0f,  1.0f,  0.14f, 0.55f, 0.14f,
        -1.0f,  6.0f, -1.0f,  0.14f, 0.55f, 0.14f,
    };
    
    treeVertices.insert(treeVertices.end(), trunk, trunk + 144);
    treeVertices.insert(treeVertices.end(), foliage, foliage + 216);
    m_TreeVertexCount = 60;
    
    glGenVertexArrays(1, &m_TreeVAO);
    glGenBuffers(1, &m_TreeVBO);
    
    glBindVertexArray(m_TreeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_TreeVBO);
    glBufferData(GL_ARRAY_BUFFER, treeVertices.size() * sizeof(float), 
                 treeVertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    std::cout << "[InfiniteTerrain] Tree mesh initialized" << std::endl;
}

// ✅ GenerateTreesForChunk（保持不变）
void InfiniteTerrain::GenerateTreesForChunk(TerrainChunk& chunk, int chunkX, int chunkZ) {
    std::mt19937 gen(static_cast<unsigned int>(chunkX * 73856093 ^ chunkZ * 19349663));
    std::uniform_real_distribution<float> offsetX(5.0f, static_cast<float>(m_ChunkSize - 5));
    std::uniform_real_distribution<float> offsetZ(5.0f, static_cast<float>(m_ChunkSize - 5));
    std::uniform_real_distribution<float> scaleRand(0.8f, 1.5f);
    std::uniform_real_distribution<float> rotRand(0.0f, 6.28318f);
    
    int treesPerChunk = 15;
    
    for (int i = 0; i < treesPerChunk; ++i) {
        float localX = offsetX(gen);
        float localZ = offsetZ(gen);
        float worldX = static_cast<float>(chunkX * m_ChunkSize) + localX;
        float worldZ = static_cast<float>(chunkZ * m_ChunkSize) + localZ;
        
        float height = Noise(worldX, worldZ);
        
        if (height > 5.0f && height < 50.0f) {
            chunk.treePositions.push_back(glm::vec3(worldX, height, worldZ));
            chunk.treeScales.push_back(scaleRand(gen));
            chunk.treeRotations.push_back(rotRand(gen));
        }
    }
}

float InfiniteTerrain::Noise(float x, float z) const {
    float height = 0.0f;
    height += std::sin(x * 0.005f) * std::cos(z * 0.005f) * 80.0f;
    height += std::sin(x * 0.01f + 1.5f) * std::cos(z * 0.012f + 0.8f) * 40.0f;
    height += std::sin(x * 0.025f + 3.0f) * std::cos(z * 0.028f + 2.0f) * 15.0f;
    height += std::sin(x * 0.08f) * std::cos(z * 0.08f) * 5.0f;
    height += std::sin(x * 0.15f + 0.5f) * std::cos(z * 0.15f + 0.3f) * 2.0f;
    return height;
}

glm::vec3 InfiniteTerrain::GetTerrainColor(float height) const {
    if (height > 60.0f) {
        return glm::vec3(0.95f, 0.95f, 0.97f);
    } else if (height > 40.0f) {
        float t = (height - 40.0f) / 20.0f;
        return glm::mix(glm::vec3(0.5f, 0.4f, 0.35f), glm::vec3(0.7f, 0.7f, 0.75f), t);
    } else if (height > 15.0f) {
        return glm::vec3(0.2f, 0.4f, 0.15f);
    } else if (height > 0.0f) {
        return glm::vec3(0.3f, 0.55f, 0.2f);
    } else if (height > -5.0f) {
        return glm::vec3(0.76f, 0.7f, 0.5f);
    } else {
        return glm::vec3(0.1f, 0.3f, 0.6f);
    }
}

float InfiniteTerrain::GetHeight(float x, float z) const {
    return Noise(x, z);
}

TerrainChunk InfiniteTerrain::GenerateChunk(int chunkX, int chunkZ) {
    TerrainChunk chunk;
    chunk.worldPos = glm::vec3(static_cast<float>(chunkX * m_ChunkSize), 0, 
                               static_cast<float>(chunkZ * m_ChunkSize));
    
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    float worldOffsetX = static_cast<float>(chunkX * m_ChunkSize);
    float worldOffsetZ = static_cast<float>(chunkZ * m_ChunkSize);
    
    for (int z = 0; z <= m_ChunkSize; ++z) {
        for (int x = 0; x <= m_ChunkSize; ++x) {
            float worldX = worldOffsetX + static_cast<float>(x);
            float worldZ = worldOffsetZ + static_cast<float>(z);
            float height = Noise(worldX, worldZ);
            
            vertices.push_back(worldX);
            vertices.push_back(height);
            vertices.push_back(worldZ);
            
            float hL = Noise(worldX - 1, worldZ);
            float hR = Noise(worldX + 1, worldZ);
            float hD = Noise(worldX, worldZ - 1);
            float hU = Noise(worldX, worldZ + 1);
            glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            
            glm::vec3 color = GetTerrainColor(height);
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);
        }
    }
    
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
    
    chunk.indexCount = static_cast<int>(indices.size());
    
    glGenVertexArrays(1, &chunk.VAO);
    glGenBuffers(1, &chunk.VBO);
    glGenBuffers(1, &chunk.EBO);
    
    glBindVertexArray(chunk.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, chunk.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 
                         (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    GenerateTreesForChunk(chunk, chunkX, chunkZ);
    GenerateCabinsForChunk(chunk, chunkX, chunkZ);
    GenerateFlowersForChunk(chunk, chunkX, chunkZ); 
    GenerateBoatsForChunk(chunk, chunkX, chunkZ);
    
    return chunk;
}

void InfiniteTerrain::Update(glm::vec3 cameraPos) {
    int camChunkX = static_cast<int>(std::floor(cameraPos.x / m_ChunkSize));
    int camChunkZ = static_cast<int>(std::floor(cameraPos.z / m_ChunkSize));
    
    for (int z = camChunkZ - m_ViewDistance; z <= camChunkZ + m_ViewDistance; ++z) {
        for (int x = camChunkX - m_ViewDistance; x <= camChunkX + m_ViewDistance; ++x) {
            ChunkKey key{x, z};
            if (m_Chunks.find(key) == m_Chunks.end()) {
                m_Chunks[key] = GenerateChunk(x, z);
            }
        }
    }
    
    std::vector<ChunkKey> toRemove;
    for (auto& pair : m_Chunks) {
        int dx = std::abs(pair.first.x - camChunkX);
        int dz = std::abs(pair.first.z - camChunkZ);
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

void InfiniteTerrain::DrawTrees(Shader& treeShader) {
    if (m_TreeVAO == 0) return;
    
    treeShader.use();
    glBindVertexArray(m_TreeVAO);
    
    for (auto& pair : m_Chunks) {
        for (size_t i = 0; i < pair.second.treePositions.size(); ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pair.second.treePositions[i]);
            model = glm::rotate(model, pair.second.treeRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(pair.second.treeScales[i]));
            
            treeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, m_TreeVertexCount);
        }
    }
    
    glBindVertexArray(0);
}

void InfiniteTerrain::InitializeCabinMesh() {
    std::vector<float> cabinVertices;
    glDisable(GL_CULL_FACE);
    
    // 颜色定义
    glm::vec3 woodDark(0.4f, 0.25f, 0.15f);
    glm::vec3 woodLight(0.5f, 0.35f, 0.2f);
    glm::vec3 roofColor(0.35f, 0.2f, 0.1f);
    glm::vec3 doorColor(0.3f, 0.15f, 0.08f);
    glm::vec3 windowColor(0.6f, 0.7f, 0.8f);
    glm::vec3 foundationColor(0.5f, 0.5f, 0.5f);
    
    float w = 4.0f, h = 3.0f, d = 5.0f;
    float roofHeight = h + 2.5f;
    
    // ========== 地基（石头围墙，略低于地面） ==========
    float foundationHeight = 0.3f;
    float foundationExtend = 0.5f;
    
    // 前地基（从外面看是逆时针）
    float foundationFront[] = {
        -(w + foundationExtend), -foundationHeight,  (d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
         (w + foundationExtend), -foundationHeight,  (d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
         (w + foundationExtend), 0.0f,  (d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
        -(w + foundationExtend), -foundationHeight,  (d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
         (w + foundationExtend), 0.0f,  (d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
        -(w + foundationExtend), 0.0f,  (d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
    };
    
    float foundationBack[] = {
         (w + foundationExtend), -foundationHeight, -(d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
        -(w + foundationExtend), -foundationHeight, -(d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
        -(w + foundationExtend), 0.0f, -(d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
         (w + foundationExtend), -foundationHeight, -(d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
        -(w + foundationExtend), 0.0f, -(d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
         (w + foundationExtend), 0.0f, -(d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
    };
    
    float foundationLeft[] = {
        -(w + foundationExtend), -foundationHeight, -(d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
        -(w + foundationExtend), -foundationHeight,  (d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
        -(w + foundationExtend), 0.0f,  (d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
        -(w + foundationExtend), -foundationHeight, -(d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
        -(w + foundationExtend), 0.0f,  (d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
        -(w + foundationExtend), 0.0f, -(d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
    };
    
    float foundationRight[] = {
         (w + foundationExtend), -foundationHeight,  (d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
         (w + foundationExtend), -foundationHeight, -(d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
         (w + foundationExtend), 0.0f, -(d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
         (w + foundationExtend), -foundationHeight,  (d + foundationExtend),  foundationColor.r, foundationColor.g, foundationColor.b,
         (w + foundationExtend), 0.0f, -(d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
         (w + foundationExtend), 0.0f,  (d + foundationExtend),  foundationColor.r * 0.9f, foundationColor.g * 0.9f, foundationColor.b * 0.9f,
    };
    
    // ========== 墙体（确保逆时针） ==========
    float leftWall[] = {
        -w, 0.0f, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w, 0.0f,  d,  woodLight.r, woodLight.g, woodLight.b,
        -w,    h,  d,  woodLight.r * 0.9f, woodLight.g * 0.9f, woodLight.b * 0.9f,
        -w, 0.0f, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w,    h,  d,  woodLight.r * 0.9f, woodLight.g * 0.9f, woodLight.b * 0.9f,
        -w,    h, -d,  woodLight.r * 0.9f, woodLight.g * 0.9f, woodLight.b * 0.9f,
    };
    
    // 右墙
    float rightWall[] = {
         w, 0.0f,  d,  woodDark.r, woodDark.g, woodDark.b,
         w,    h, -d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
         w, 0.0f, -d,  woodDark.r, woodDark.g, woodDark.b,
         w, 0.0f,  d,  woodDark.r, woodDark.g, woodDark.b,
         w,    h,  d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
         w,    h, -d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
    };
    
    
    float backWall[] = {
         w, 0.0f, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w, 0.0f, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w,    h, -d,  woodLight.r * 0.9f, woodLight.g * 0.9f, woodLight.b * 0.9f,
         w, 0.0f, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w,    h, -d,  woodLight.r * 0.9f, woodLight.g * 0.9f, woodLight.b * 0.9f,
         w,    h, -d,  woodLight.r * 0.9f, woodLight.g * 0.9f, woodLight.b * 0.9f,
    };
    
    // 前墙（分三块：左、上、右，中间留门）
    float frontLeft[] = {
        -w, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
        -1.5f, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
        -1.5f, h, d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
        -w, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
        -1.5f, h, d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
        -w, h, d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
    };
    
    float frontTop[] = {
        -1.5f, 2.2f, d,  woodDark.r, woodDark.g, woodDark.b,
         1.5f, 2.2f, d,  woodDark.r, woodDark.g, woodDark.b,
         1.5f, h, d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
        -1.5f, 2.2f, d,  woodDark.r, woodDark.g, woodDark.b,
         1.5f, h, d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
        -1.5f, h, d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
    };
    
    float frontRight[] = {
         1.5f, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
         w, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
         w, h, d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
         1.5f, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
         w, h, d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
         1.5f, h, d,  woodDark.r * 0.9f, woodDark.g * 0.9f, woodDark.b * 0.9f,
    };
    
    // 门
    float door[] = {
        -1.2f, 0.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
         1.2f, 0.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
         1.2f, 2.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
        -1.2f, 0.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
         1.2f, 2.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
        -1.2f, 2.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
    };
    
    // 窗户
    float leftWindow[] = {
        -w - 0.05f, 1.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 1.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 2.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 1.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 2.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 2.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
    };
    
    float rightWindow[] = {
         w + 0.05f, 1.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 2.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 1.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 1.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 2.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 2.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
    };
    
    // ========== 屋顶（✅ 修复顶点顺序） ==========
    // 前三角端面
    float roofFront[] = {
         0.0f, roofHeight,  d,  roofColor.r, roofColor.g, roofColor.b,
        -w,    h,  d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
         w,    h,  d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
    };
    // 后三角端面（从后面看逆时针）
    float roofBack[] = {
         w,    h, -d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
         0.0f, roofHeight, -d,  roofColor.r, roofColor.g, roofColor.b,
        -w,    h, -d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
    };
    
    // ✅ 左斜面（完整的四边形，2个三角形）
    float roofLeft[] = {
        // 三角形1
         0.0f, roofHeight,  d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
        -w,    h,  d,  roofColor.r * 0.75f, roofColor.g * 0.75f, roofColor.b * 0.75f,
        -w,    h, -d,  roofColor.r * 0.75f, roofColor.g * 0.75f, roofColor.b * 0.75f,
        // 三角形2
         0.0f, roofHeight,  d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
        -w,    h, -d,  roofColor.r * 0.75f, roofColor.g * 0.75f, roofColor.b * 0.75f,
         0.0f, roofHeight, -d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
    };

    // // ✅ 左斜面（从外面看逆时针）
    // float roofLeft[] = {
    //     // 三角形1
    //     -w,    h, -d,  roofColor.r * 0.75f, roofColor.g * 0.75f, roofColor.b * 0.75f,
    //      0.0f, roofHeight, -d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
    //      0.0f, roofHeight,  d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
    //     // 三角形2
    //     -w,    h, -d,  roofColor.r * 0.75f, roofColor.g * 0.75f, roofColor.b * 0.75f,
    //      0.0f, roofHeight,  d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
    //     -w,    h,  d,  roofColor.r * 0.75f, roofColor.g * 0.75f, roofColor.b * 0.75f,
    // };
    
    // ✅ 右斜面（完整的四边形，2个三角形）
    float roofRight[] = {
        // 三角形1
         0.0f, roofHeight,  d,  roofColor.r * 0.85f, roofColor.g * 0.85f, roofColor.b * 0.85f,
         w,    h, -d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
         w,    h,  d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
        // 三角形2
         0.0f, roofHeight,  d,  roofColor.r * 0.85f, roofColor.g * 0.85f, roofColor.b * 0.85f,
         0.0f, roofHeight, -d,  roofColor.r * 0.85f, roofColor.g * 0.85f, roofColor.b * 0.85f,
         w,    h, -d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
    };
    
    // ========== 组装所有部分 ==========
    cabinVertices.insert(cabinVertices.end(), foundationFront, foundationFront + 36);
    cabinVertices.insert(cabinVertices.end(), foundationBack, foundationBack + 36);
    cabinVertices.insert(cabinVertices.end(), foundationLeft, foundationLeft + 36);
    cabinVertices.insert(cabinVertices.end(), foundationRight, foundationRight + 36);
    
    cabinVertices.insert(cabinVertices.end(), leftWall, leftWall + 36);
    cabinVertices.insert(cabinVertices.end(), rightWall, rightWall + 36);
    cabinVertices.insert(cabinVertices.end(), backWall, backWall + 36);
    cabinVertices.insert(cabinVertices.end(), frontLeft, frontLeft + 36);
    cabinVertices.insert(cabinVertices.end(), frontTop, frontTop + 36);
    cabinVertices.insert(cabinVertices.end(), frontRight, frontRight + 36);
    
    cabinVertices.insert(cabinVertices.end(), door, door + 36);
    cabinVertices.insert(cabinVertices.end(), leftWindow, leftWindow + 36);
    cabinVertices.insert(cabinVertices.end(), rightWindow, rightWindow + 36);
    
    cabinVertices.insert(cabinVertices.end(), roofFront, roofFront + 18);
    cabinVertices.insert(cabinVertices.end(), roofBack, roofBack + 18);
    cabinVertices.insert(cabinVertices.end(), roofLeft, roofLeft + 36);
    cabinVertices.insert(cabinVertices.end(), roofRight, roofRight + 36);
    
    m_CabinVertexCount = cabinVertices.size() / 6;
    
    glGenVertexArrays(1, &m_CabinVAO);
    glGenBuffers(1, &m_CabinVBO);
    
    glBindVertexArray(m_CabinVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CabinVBO);
    glBufferData(GL_ARRAY_BUFFER, cabinVertices.size() * sizeof(float), 
                 cabinVertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    std::cout << "[InfiniteTerrain] Cabin mesh initialized with " 
              << m_CabinVertexCount << " vertices" << std::endl;
}

// ✅ GenerateCabinsForChunk
void InfiniteTerrain::GenerateCabinsForChunk(TerrainChunk& chunk, int chunkX, int chunkZ) {
    std::mt19937 gen(static_cast<unsigned int>((chunkX * 374761393) ^ (chunkZ * 668265263)));
    std::uniform_real_distribution<float> spawnChance(0.0f, 1.0f);
    std::uniform_real_distribution<float> offsetX(10.0f, static_cast<float>(m_ChunkSize - 10));
    std::uniform_real_distribution<float> offsetZ(10.0f, static_cast<float>(m_ChunkSize - 10));
    std::uniform_real_distribution<float> scaleRand(0.8f, 1.2f);
    std::uniform_real_distribution<float> rotRand(0.0f, 6.28318f);
    
    if (spawnChance(gen) < 0.6f) {
        int cabinCount = (spawnChance(gen) < 0.5f) ? 1 : 2;
        
        for (int i = 0; i < cabinCount; ++i) {
            float localX = offsetX(gen);
            float localZ = offsetZ(gen);
            float worldX = static_cast<float>(chunkX * m_ChunkSize) + localX;
            float worldZ = static_cast<float>(chunkZ * m_ChunkSize) + localZ;
            
            float height = Noise(worldX, worldZ);
            
            if (height > 10.0f && height < 35.0f) {
                float hL = Noise(worldX - 5.0f, worldZ);
                float hR = Noise(worldX + 5.0f, worldZ);
                float hD = Noise(worldX, worldZ - 5.0f);
                float hU = Noise(worldX, worldZ + 5.0f);
                glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 10.0f, hD - hU));
                
                if (normal.y > 0.85f) {
                    CabinData cabin;
                    cabin.position = glm::vec3(worldX, height, worldZ);
                    cabin.rotation = rotRand(gen);
                    cabin.scale = scaleRand(gen);
                    chunk.cabins.push_back(cabin);
                }
            }
        }
    }
}

// ✅ DrawCabins
void InfiniteTerrain::DrawCabins(Shader& cabinShader) {
    if (m_CabinVAO == 0) return;
    
    cabinShader.use();
    glBindVertexArray(m_CabinVAO);
    glDisable(GL_CULL_FACE);
    for (auto& pair : m_Chunks) {
        for (const auto& cabin : pair.second.cabins) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cabin.position);
            model = glm::rotate(model, cabin.rotation, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(cabin.scale));
            
            cabinShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, m_CabinVertexCount);
        }
    }
    
    glBindVertexArray(0);
}


// ✅ 初始化花丛网格（放在 DrawCabins 之后）
void InfiniteTerrain::InitializeFlowerMesh() {
    std::vector<float> flowerVertices;
    
    // 花丛由多个小花组成，每朵花是简单的菱形
    // 基础白色（实际颜色在实例化时乘以颜色值）
    glm::vec3 baseColor(1.0f, 1.0f, 1.0f);
    
    // 花丛中心有5朵花，排列成十字形
    struct FlowerOffset {
        float x, z;
        float size;
    };
    
    FlowerOffset flowers[] = {
        {0.0f, 0.0f, 1.0f},     // 中心
        {0.8f, 0.0f, 0.8f},     // 右
        {-0.8f, 0.0f, 0.8f},    // 左
        {0.0f, 0.8f, 0.9f},     // 前
        {0.0f, -0.8f, 0.9f},    // 后
        {0.6f, 0.6f, 0.7f},     // 右前
        {-0.6f, 0.6f, 0.7f},    // 左前
        {0.6f, -0.6f, 0.7f},    // 右后
        {-0.6f, -0.6f, 0.7f},   // 左后
    };
    
    for (const auto& flower : flowers) {
        float x = flower.x;
        float z = flower.z;
        float s = flower.size * 0.4f; // 花朵大小
        float h = 0.5f; // 花朵高度
        
        // 每朵花由两个交叉的菱形组成（4个三角形）
        // 菱形1（垂直，朝向相机）
        flowerVertices.insert(flowerVertices.end(), {
            // 三角形1
            x,         h,     z,      baseColor.r, baseColor.g, baseColor.b,
            x - s,     h/2,   z,      baseColor.r, baseColor.g, baseColor.b,
            x,         0.0f,  z,      baseColor.r * 0.7f, baseColor.g * 0.7f, baseColor.b * 0.7f,
            
            // 三角形2
            x,         h,     z,      baseColor.r, baseColor.g, baseColor.b,
            x,         0.0f,  z,      baseColor.r * 0.7f, baseColor.g * 0.7f, baseColor.b * 0.7f,
            x + s,     h/2,   z,      baseColor.r, baseColor.g, baseColor.b,
        });
        
        // 菱形2（垂直，朝向侧面，旋转90度）
        flowerVertices.insert(flowerVertices.end(), {
            // 三角形3
            x,         h,     z,      baseColor.r, baseColor.g, baseColor.b,
            x,         h/2,   z - s,  baseColor.r, baseColor.g, baseColor.b,
            x,         0.0f,  z,      baseColor.r * 0.7f, baseColor.g * 0.7f, baseColor.b * 0.7f,
            
            // 三角形4
            x,         h,     z,      baseColor.r, baseColor.g, baseColor.b,
            x,         0.0f,  z,      baseColor.r * 0.7f, baseColor.g * 0.7f, baseColor.b * 0.7f,
            x,         h/2,   z + s,  baseColor.r, baseColor.g, baseColor.b,
        });
        
        // 添加一些叶子（绿色，贴地）
        glm::vec3 leafColor(0.2f, 0.5f, 0.2f);
        flowerVertices.insert(flowerVertices.end(), {
            x - s*0.8f, 0.05f, z - s*0.8f,  leafColor.r, leafColor.g, leafColor.b,
            x + s*0.8f, 0.05f, z - s*0.8f,  leafColor.r, leafColor.g, leafColor.b,
            x + s*0.8f, 0.05f, z + s*0.8f,  leafColor.r, leafColor.g, leafColor.b,
            
            x - s*0.8f, 0.05f, z - s*0.8f,  leafColor.r, leafColor.g, leafColor.b,
            x + s*0.8f, 0.05f, z + s*0.8f,  leafColor.r, leafColor.g, leafColor.b,
            x - s*0.8f, 0.05f, z + s*0.8f,  leafColor.r, leafColor.g, leafColor.b,
        });
    }
    
    m_FlowerVertexCount = flowerVertices.size() / 6;
    
    glGenVertexArrays(1, &m_FlowerVAO);
    glGenBuffers(1, &m_FlowerVBO);
    
    glBindVertexArray(m_FlowerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_FlowerVBO);
    glBufferData(GL_ARRAY_BUFFER, flowerVertices.size() * sizeof(float), 
                 flowerVertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    std::cout << "[InfiniteTerrain] Flower mesh initialized with " 
              << m_FlowerVertexCount << " vertices" << std::endl;
}

// ✅ 为每个 chunk 生成花丛
void InfiniteTerrain::GenerateFlowersForChunk(TerrainChunk& chunk, int chunkX, int chunkZ) {
    std::mt19937 gen(static_cast<unsigned int>((chunkX * 982451653) ^ (chunkZ * 1453924613)));
    std::uniform_real_distribution<float> offsetX(3.0f, static_cast<float>(m_ChunkSize - 3));
    std::uniform_real_distribution<float> offsetZ(3.0f, static_cast<float>(m_ChunkSize - 3));
    std::uniform_real_distribution<float> scaleRand(0.6f, 1.4f);
    std::uniform_real_distribution<float> rotRand(0.0f, 6.28318f);
    std::uniform_int_distribution<int> colorChoice(0, 4);
    
    // 花朵颜色调色板
    glm::vec3 flowerColors[] = {
        glm::vec3(1.0f, 0.2f, 0.3f),   // 红色
        glm::vec3(1.0f, 0.8f, 0.2f),   // 黄色
        glm::vec3(1.0f, 0.4f, 0.8f),   // 粉色
        glm::vec3(0.6f, 0.3f, 0.9f),   // 紫色
        glm::vec3(0.95f, 0.95f, 1.0f), // 白色
    };
    
    int flowersPerChunk = 18; // 和树的密度相近（树是15个）
    
    for (int i = 0; i < flowersPerChunk; ++i) {
        float localX = offsetX(gen);
        float localZ = offsetZ(gen);
        float worldX = static_cast<float>(chunkX * m_ChunkSize) + localX;
        float worldZ = static_cast<float>(chunkZ * m_ChunkSize) + localZ;
        
        float height = Noise(worldX, worldZ);
        
        // ✅ 花丛主要生成在山坡和草地（高度 15-55）
        if (height > 15.0f && height < 55.0f) {
            // 检查坡度（花可以在稍微陡峭的地方生长）
            float hL = Noise(worldX - 3.0f, worldZ);
            float hR = Noise(worldX + 3.0f, worldZ);
            float hD = Noise(worldX, worldZ - 3.0f);
            float hU = Noise(worldX, worldZ + 3.0f);
            glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 6.0f, hD - hU));
            
            // 花可以在较陡的坡度生长（30度以内）
            if (normal.y > 0.7f) {
                FlowerPatchData flower;
                flower.position = glm::vec3(worldX, height, worldZ);
                flower.rotation = rotRand(gen);
                flower.scale = scaleRand(gen);
                flower.color = flowerColors[colorChoice(gen)];
                chunk.flowerPatches.push_back(flower);
            }
        }
    }
}

// ✅ 绘制所有花丛
void InfiniteTerrain::DrawFlowers(Shader& flowerShader) {
    if (m_FlowerVAO == 0) return;
    
    flowerShader.use();
    glBindVertexArray(m_FlowerVAO);
    
    // ✅ 禁用背面剔除（花朵从任何角度都可见）
    glDisable(GL_CULL_FACE);
    
    for (auto& pair : m_Chunks) {
        for (const auto& flower : pair.second.flowerPatches) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, flower.position);
            model = glm::rotate(model, flower.rotation, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(flower.scale));
            
            flowerShader.setMat4("model", model);
            flowerShader.setVec3("flowerColor", flower.color); // ✅ 传递花朵颜色
            
            glDrawArrays(GL_TRIANGLES, 0, m_FlowerVertexCount);
        }
    }
    
    glEnable(GL_CULL_FACE);
    glBindVertexArray(0);
}


void InfiniteTerrain::InitializeBoatMesh() {
    std::vector<float> boatVertices;
    
    // 颜色定义
    glm::vec3 hullColor(0.6f, 0.35f, 0.2f);      // 船体：深棕色
    glm::vec3 deckColor(0.7f, 0.5f, 0.3f);       // 甲板：浅棕色
    glm::vec3 sailColor(0.95f, 0.95f, 0.98f);    // 帆：白色
    glm::vec3 mastColor(0.5f, 0.3f, 0.15f);      // 桅杆：深棕色
    
    // 船体尺寸
    float length = 8.0f;   // 船长
    float width = 2.5f;    // 船宽
    float height = 1.5f;   // 船高
    
    // ========== 船底（V字形） ==========
    float bottom[] = {
        // 前三角（船头尖）
        0.0f, 0.0f, length * 0.5f,  hullColor.r, hullColor.g, hullColor.b,
        -width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.8f, hullColor.g * 0.8f, hullColor.b * 0.8f,
        width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.8f, hullColor.g * 0.8f, hullColor.b * 0.8f,
        
        // 后三角（船尾）
        -width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.8f, hullColor.g * 0.8f, hullColor.b * 0.8f,
        0.0f, -height * 0.2f, -length * 0.5f,  hullColor.r * 0.7f, hullColor.g * 0.7f, hullColor.b * 0.7f,
        width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.8f, hullColor.g * 0.8f, hullColor.b * 0.8f,
    };
    
    // ========== 船体侧面 ==========
    float leftSide[] = {
        // 左侧
        -width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.9f, hullColor.g * 0.9f, hullColor.b * 0.9f,
        -width * 0.4f, height * 0.3f, length * 0.4f,  hullColor.r, hullColor.g, hullColor.b,
        0.0f, 0.0f, length * 0.5f,  hullColor.r, hullColor.g, hullColor.b,
        
        -width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.9f, hullColor.g * 0.9f, hullColor.b * 0.9f,
        -width * 0.4f, height * 0.3f, -length * 0.4f,  hullColor.r, hullColor.g, hullColor.b,
        -width * 0.4f, height * 0.3f, length * 0.4f,  hullColor.r, hullColor.g, hullColor.b,
        
        -width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.9f, hullColor.g * 0.9f, hullColor.b * 0.9f,
        0.0f, -height * 0.2f, -length * 0.5f,  hullColor.r * 0.7f, hullColor.g * 0.7f, hullColor.b * 0.7f,
        -width * 0.4f, height * 0.3f, -length * 0.4f,  hullColor.r, hullColor.g, hullColor.b,
    };
    
    float rightSide[] = {
        // 右侧（镜像）
        width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.9f, hullColor.g * 0.9f, hullColor.b * 0.9f,
        0.0f, 0.0f, length * 0.5f,  hullColor.r, hullColor.g, hullColor.b,
        width * 0.4f, height * 0.3f, length * 0.4f,  hullColor.r, hullColor.g, hullColor.b,
        
        width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.9f, hullColor.g * 0.9f, hullColor.b * 0.9f,
        width * 0.4f, height * 0.3f, length * 0.4f,  hullColor.r, hullColor.g, hullColor.b,
        width * 0.4f, height * 0.3f, -length * 0.4f,  hullColor.r, hullColor.g, hullColor.b,
        
        width * 0.5f, -height * 0.3f, 0.0f,  hullColor.r * 0.9f, hullColor.g * 0.9f, hullColor.b * 0.9f,
        width * 0.4f, height * 0.3f, -length * 0.4f,  hullColor.r, hullColor.g, hullColor.b,
        0.0f, -height * 0.2f, -length * 0.5f,  hullColor.r * 0.7f, hullColor.g * 0.7f, hullColor.b * 0.7f,
    };
    
    // ========== 甲板 ==========
    float deck[] = {
        -width * 0.4f, height * 0.3f, length * 0.4f,  deckColor.r, deckColor.g, deckColor.b,
        width * 0.4f, height * 0.3f, length * 0.4f,  deckColor.r, deckColor.g, deckColor.b,
        width * 0.4f, height * 0.3f, -length * 0.4f,  deckColor.r, deckColor.g, deckColor.b,
        
        -width * 0.4f, height * 0.3f, length * 0.4f,  deckColor.r, deckColor.g, deckColor.b,
        width * 0.4f, height * 0.3f, -length * 0.4f,  deckColor.r, deckColor.g, deckColor.b,
        -width * 0.4f, height * 0.3f, -length * 0.4f,  deckColor.r, deckColor.g, deckColor.b,
    };
    
    // ========== 桅杆 ==========
    float mast[] = {
        // 前面
        -0.2f, height * 0.3f, 0.0f,  mastColor.r, mastColor.g, mastColor.b,
        0.2f, height * 0.3f, 0.0f,  mastColor.r, mastColor.g, mastColor.b,
        0.2f, height * 4.0f, 0.0f,  mastColor.r * 0.8f, mastColor.g * 0.8f, mastColor.b * 0.8f,
        -0.2f, height * 0.3f, 0.0f,  mastColor.r, mastColor.g, mastColor.b,
        0.2f, height * 4.0f, 0.0f,  mastColor.r * 0.8f, mastColor.g * 0.8f, mastColor.b * 0.8f,
        -0.2f, height * 4.0f, 0.0f,  mastColor.r * 0.8f, mastColor.g * 0.8f, mastColor.b * 0.8f,
        
        // 侧面
        0.0f, height * 0.3f, -0.2f,  mastColor.r, mastColor.g, mastColor.b,
        0.0f, height * 4.0f, -0.2f,  mastColor.r * 0.8f, mastColor.g * 0.8f, mastColor.b * 0.8f,
        0.0f, height * 4.0f, 0.2f,  mastColor.r * 0.8f, mastColor.g * 0.8f, mastColor.b * 0.8f,
        0.0f, height * 0.3f, -0.2f,  mastColor.r, mastColor.g, mastColor.b,
        0.0f, height * 4.0f, 0.2f,  mastColor.r * 0.8f, mastColor.g * 0.8f, mastColor.b * 0.8f,
        0.0f, height * 0.3f, 0.2f,  mastColor.r, mastColor.g, mastColor.b,
    };
    
    // ========== 三角帆 ==========
    float sail[] = {
        // 主帆
        0.0f, height * 4.0f, 0.0f,  sailColor.r, sailColor.g, sailColor.b,
        -width * 1.2f, height * 2.0f, 0.5f,  sailColor.r * 0.9f, sailColor.g * 0.9f, sailColor.b * 0.9f,
        0.0f, height * 0.5f, 0.0f,  sailColor.r * 0.85f, sailColor.g * 0.85f, sailColor.b * 0.85f,
        
        // 反面
        0.0f, height * 4.0f, 0.0f,  sailColor.r * 0.95f, sailColor.g * 0.95f, sailColor.b * 0.95f,
        0.0f, height * 0.5f, 0.0f,  sailColor.r * 0.8f, sailColor.g * 0.8f, sailColor.b * 0.8f,
        -width * 1.2f, height * 2.0f, 0.5f,  sailColor.r * 0.85f, sailColor.g * 0.85f, sailColor.b * 0.85f,
    };
    
    // 组装所有部分
    boatVertices.insert(boatVertices.end(), bottom, bottom + 36);
    boatVertices.insert(boatVertices.end(), leftSide, leftSide + 54);
    boatVertices.insert(boatVertices.end(), rightSide, rightSide + 54);
    boatVertices.insert(boatVertices.end(), deck, deck + 36);
    boatVertices.insert(boatVertices.end(), mast, mast + 72);
    boatVertices.insert(boatVertices.end(), sail, sail + 36);
    
    m_BoatVertexCount = boatVertices.size() / 6;
    
    glGenVertexArrays(1, &m_BoatVAO);
    glGenBuffers(1, &m_BoatVBO);
    
    glBindVertexArray(m_BoatVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_BoatVBO);
    glBufferData(GL_ARRAY_BUFFER, boatVertices.size() * sizeof(float), 
                 boatVertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    std::cout << "[InfiniteTerrain] Boat mesh initialized with " 
              << m_BoatVertexCount << " vertices" << std::endl;
}

// ✅ 为每个 chunk 生成小船
void InfiniteTerrain::GenerateBoatsForChunk(TerrainChunk& chunk, int chunkX, int chunkZ) {
    std::mt19937 gen(static_cast<unsigned int>((chunkX * 198491317) ^ (chunkZ * 664953457)));
    std::uniform_real_distribution<float> spawnChance(0.0f, 1.0f);
    std::uniform_real_distribution<float> offsetX(8.0f, static_cast<float>(m_ChunkSize - 10));
    std::uniform_real_distribution<float> offsetZ(8.0f, static_cast<float>(m_ChunkSize - 10));
    std::uniform_real_distribution<float> scaleRand(0.7f, 1.3f);
    std::uniform_real_distribution<float> rotRand(0.0f, 6.28318f);
    std::uniform_real_distribution<float> phaseRand(0.0f, 6.28318f);
    
    // 30% 概率生成小船
    if (spawnChance(gen) < 0.7f) {
        int boatCount = 3 + static_cast<int>(spawnChance(gen) * 4);
        
        for (int i = 0; i < boatCount; ++i) {
            float localX = offsetX(gen);
            float localZ = offsetZ(gen);
            float worldX = static_cast<float>(chunkX * m_ChunkSize) + localX;
            float worldZ = static_cast<float>(chunkZ * m_ChunkSize) + localZ;
            
            float height = Noise(worldX, worldZ);
            
            // ✅ 只在水面生成（高度 -10 到 0）
            if (height >= -15.0f && height <= 0.0f) {
                BoatData boat;
                boat.position = glm::vec3(worldX, height + 0.5f, worldZ); // 浮在水面上
                boat.rotation = rotRand(gen);
                boat.scale = scaleRand(gen);
                boat.bobPhase = phaseRand(gen); // 随机波浪相位
                chunk.boats.push_back(boat);
            }
        }
    }
}

// ✅ 绘制所有小船
void InfiniteTerrain::DrawBoats(Shader& boatShader, float time) {
    if (m_BoatVAO == 0) return;
    
    boatShader.use();
    glBindVertexArray(m_BoatVAO);
    
    for (auto& pair : m_Chunks) {
        for (const auto& boat : pair.second.boats) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, boat.position);
            
            // ✅ 波浪起伏动画（上下浮动 + 轻微摇摆）
            float bobOffset = std::sin(time * 0.5f + boat.bobPhase) * 0.3f;
            model = glm::translate(model, glm::vec3(0.0f, bobOffset, 0.0f));
            
            // ✅ 轻微摇摆（绕 Z 轴旋转）
            float rockAngle = std::sin(time * 0.7f + boat.bobPhase * 1.3f) * 0.1f;
            model = glm::rotate(model, rockAngle, glm::vec3(0.0f, 0.0f, 1.0f));
            
            model = glm::rotate(model, boat.rotation, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(boat.scale));
            
            boatShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, m_BoatVertexCount);
        }
    }
    
    glBindVertexArray(0);
}