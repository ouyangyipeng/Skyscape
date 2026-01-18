#include "Terrain.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <cmath>
#include <vector>

Terrain::Terrain(int width, int height) : m_Width(width), m_Height(height) {
    generateTerrain();
}

void Terrain::generateTerrain() {
    // ✅ 声明必要的容器
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<std::vector<float>> heightMap(m_Height, std::vector<float>(m_Width));

    float minHeight = std::numeric_limits<float>::max();
    float maxHeight = std::numeric_limits<float>::lowest();

    // ========== Step 1: Generate height map ==========
    for(int z = 0; z < m_Height; ++z) {
        for(int x = 0; x < m_Width; ++x) {
            float height = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            
            // 8 层噪声叠加
            for(int octave = 0; octave < 8; ++octave) {
                float nx = x * 0.008f * frequency;
                float nz = z * 0.008f * frequency;
                
                // 多层正弦波叠加
                float n = std::sin(nx) * std::cos(nz) + 
                         std::sin(nx * 2.7f + 4.1f) * std::cos(nz * 2.9f + 3.7f) * 0.5f +
                         std::sin(nx * 5.3f + 1.8f) * std::cos(nz * 4.7f + 2.3f) * 0.25f;
                
                height += n * amplitude;
                amplitude *= 0.45f;
                frequency *= 2.3f;
            }
            
            // 扩大高度范围
            height = height * 40.0f + 10.0f;
            
            // 添加山脉系统
            float ridgeX = std::abs(std::sin((x + z) * 0.015f)) * 30.0f;
            float ridgeZ = std::abs(std::cos((x - z) * 0.012f)) * 25.0f;
            height += ridgeX + ridgeZ;
            
            // 添加随机峰值
            float peakNoise = std::sin(x * 0.02f) * std::cos(z * 0.019f);
            if(peakNoise > 0.6f) {
                height += (peakNoise - 0.6f) * 80.0f;
            }
            
            heightMap[z][x] = height;
            positions.push_back(glm::vec3(static_cast<float>(x) * 2.0f, height, static_cast<float>(z) * 2.0f));
            // // ✅ 跟踪最小/最大高度
            // minHeight = std::min(minHeight, height);
            // maxHeight = std::max(maxHeight, height);
        }
    }

    // for(int z = 0; z < m_Height; ++z) {
    //     for(int x = 0; x < m_Width; ++x) {
    //         // ✅ 方法1：平移到 Y=0（推荐）
    //         heightMap[z][x] -= minHeight;  // 最低点变为 0
            
    //         // ✅ 或方法2：重新映射到 [0, desiredMaxHeight]
    //         // float normalizedHeight = (heightMap[z][x] - minHeight) / (maxHeight - minHeight);
    //         // heightMap[z][x] = normalizedHeight * 150.0f;  // 映射到 0-150 范围
            
    //         positions.push_back(glm::vec3(
    //             static_cast<float>(x) * 2.0f, 
    //             heightMap[z][x],  // 使用归一化后的高度
    //             static_cast<float>(z) * 2.0f
    //         ));
    //     }
    // }

    // ========== Step 2: Calculate smooth normals ==========
    for(int z = 0; z < m_Height; ++z) {
        for(int x = 0; x < m_Width; ++x) {
            float heightL = (x > 0) ? heightMap[z][x-1] : heightMap[z][x];
            float heightR = (x < m_Width-1) ? heightMap[z][x+1] : heightMap[z][x];
            float heightD = (z > 0) ? heightMap[z-1][x] : heightMap[z][x];
            float heightU = (z < m_Height-1) ? heightMap[z+1][x] : heightMap[z][x];
            
            glm::vec3 normal = glm::normalize(glm::vec3(
                (heightL - heightR) * 0.5f, 
                2.0f,
                (heightD - heightU) * 0.5f
            ));
            normals.push_back(normal);
        }
    }
    
    // ========== Step 3: Generate indices ==========
    for(int z = 0; z < m_Height - 1; ++z) {
        for(int x = 0; x < m_Width - 1; ++x) {
            int topLeft = z * m_Width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * m_Width + x;
            int bottomRight = bottomLeft + 1;
            
            // Triangle 1
            m_Indices.push_back(topLeft);
            m_Indices.push_back(bottomLeft);
            m_Indices.push_back(topRight);
            
            // Triangle 2
            m_Indices.push_back(topRight);
            m_Indices.push_back(bottomLeft);
            m_Indices.push_back(bottomRight);
        }
    }

    // ========== Step 4: Pack vertex data ==========
    for(size_t i = 0; i < positions.size(); ++i) {
        // Position
        m_Vertices.push_back(positions[i].x);
        m_Vertices.push_back(positions[i].y);
        m_Vertices.push_back(positions[i].z);
        
        // Normal
        m_Vertices.push_back(normals[i].x);
        m_Vertices.push_back(normals[i].y);
        m_Vertices.push_back(normals[i].z);
        
        // TexCoords
        m_Vertices.push_back(static_cast<float>(i % m_Width) / (m_Width - 1));
        m_Vertices.push_back(static_cast<float>(i / m_Width) / (m_Height - 1));
    }
    
    // ========== Step 5: Setup OpenGL buffers ==========
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 m_Vertices.size() * sizeof(float), 
                 m_Vertices.data(), 
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 m_Indices.size() * sizeof(unsigned int), 
                 m_Indices.data(), 
                 GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    std::cout << "[Terrain] Generated: " << m_Width << "x" << m_Height 
              << " (" << positions.size() << " vertices, " 
              << m_Indices.size() / 3 << " triangles)" << std::endl;
}

void Terrain::Draw(Shader& shader) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}