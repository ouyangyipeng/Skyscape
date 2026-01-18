#include "Cabin.h"
#include "../graphics/Shader.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Cabin::Cabin() : m_VAO(0), m_VBO(0), m_VertexCount(0) {
    generateCabinMesh();
}

Cabin::~Cabin() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
}

void Cabin::generateCabinMesh() {
    std::vector<float> vertices;
    
    // 木材颜色定义
    glm::vec3 woodDark(0.4f, 0.25f, 0.15f);   // 深色木材
    glm::vec3 woodLight(0.5f, 0.35f, 0.2f);   // 浅色木材
    glm::vec3 roofColor(0.35f, 0.2f, 0.1f);   // 屋顶深棕色
    glm::vec3 doorColor(0.3f, 0.15f, 0.08f);  // 门深色
    glm::vec3 windowColor(0.6f, 0.7f, 0.8f);  // 窗户浅蓝色
    
    // ========== 1. 房屋主体（立方体） ==========
    float w = 4.0f, h = 3.0f, d = 5.0f; // 宽、高、深
    
    // 前墙（带门和窗）
    // 左侧墙面
    vertices.insert(vertices.end(), {
        -w, 0.0f,  d,  woodLight.r, woodLight.g, woodLight.b,
        -w, 0.0f, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w,    h, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w, 0.0f,  d,  woodLight.r, woodLight.g, woodLight.b,
        -w,    h, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w,    h,  d,  woodLight.r, woodLight.g, woodLight.b,
    });
    
    // 右侧墙面
    vertices.insert(vertices.end(), {
         w, 0.0f,  d,  woodDark.r, woodDark.g, woodDark.b,
         w,    h, -d,  woodDark.r, woodDark.g, woodDark.b,
         w, 0.0f, -d,  woodDark.r, woodDark.g, woodDark.b,
         w, 0.0f,  d,  woodDark.r, woodDark.g, woodDark.b,
         w,    h,  d,  woodDark.r, woodDark.g, woodDark.b,
         w,    h, -d,  woodDark.r, woodDark.g, woodDark.b,
    });
    
    // 后墙
    vertices.insert(vertices.end(), {
        -w, 0.0f, -d,  woodLight.r, woodLight.g, woodLight.b,
         w, 0.0f, -d,  woodLight.r, woodLight.g, woodLight.b,
         w,    h, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w, 0.0f, -d,  woodLight.r, woodLight.g, woodLight.b,
         w,    h, -d,  woodLight.r, woodLight.g, woodLight.b,
        -w,    h, -d,  woodLight.r, woodLight.g, woodLight.b,
    });
    
    // 前墙（分段绘制，为门窗留空间）
    // 左下部分
    vertices.insert(vertices.end(), {
        -w, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
        -1.5f, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
        -1.5f, h, d,  woodDark.r, woodDark.g, woodDark.b,
        -w, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
        -1.5f, h, d,  woodDark.r, woodDark.g, woodDark.b,
        -w, h, d,  woodDark.r, woodDark.g, woodDark.b,
    });
    
    // 门上方
    vertices.insert(vertices.end(), {
        -1.5f, 2.2f, d,  woodDark.r, woodDark.g, woodDark.b,
         1.5f, 2.2f, d,  woodDark.r, woodDark.g, woodDark.b,
         1.5f, h, d,  woodDark.r, woodDark.g, woodDark.b,
        -1.5f, 2.2f, d,  woodDark.r, woodDark.g, woodDark.b,
         1.5f, h, d,  woodDark.r, woodDark.g, woodDark.b,
        -1.5f, h, d,  woodDark.r, woodDark.g, woodDark.b,
    });
    
    // 右下部分
    vertices.insert(vertices.end(), {
         1.5f, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
         w, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
         w, h, d,  woodDark.r, woodDark.g, woodDark.b,
         1.5f, 0.0f, d,  woodDark.r, woodDark.g, woodDark.b,
         w, h, d,  woodDark.r, woodDark.g, woodDark.b,
         1.5f, h, d,  woodDark.r, woodDark.g, woodDark.b,
    });
    
    // ========== 2. 门 ==========
    vertices.insert(vertices.end(), {
        -1.2f, 0.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
         1.2f, 0.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
         1.2f, 2.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
        -1.2f, 0.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
         1.2f, 2.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
        -1.2f, 2.0f, d + 0.1f,  doorColor.r, doorColor.g, doorColor.b,
    });
    
    // ========== 3. 窗户（左右各一个） ==========
    // 左窗
    vertices.insert(vertices.end(), {
        -w - 0.05f, 1.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 1.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 2.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 1.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 2.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
        -w - 0.05f, 2.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
    });
    
    // 右窗
    vertices.insert(vertices.end(), {
         w + 0.05f, 1.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 2.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 1.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 1.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 2.5f,  2.0f,  windowColor.r, windowColor.g, windowColor.b,
         w + 0.05f, 2.5f,  0.5f,  windowColor.r, windowColor.g, windowColor.b,
    });
    
    // ========== 4. 三角形屋顶（两个三角形） ==========
    float roofHeight = h + 2.5f;
    
    // 前三角形
    vertices.insert(vertices.end(), {
         0.0f, roofHeight,  d,  roofColor.r, roofColor.g, roofColor.b,
        -w,    h,  d,  roofColor.r, roofColor.g, roofColor.b,
         w,    h,  d,  roofColor.r, roofColor.g, roofColor.b,
    });
    
    // 后三角形
    vertices.insert(vertices.end(), {
         0.0f, roofHeight, -d,  roofColor.r, roofColor.g, roofColor.b,
         w,    h, -d,  roofColor.r, roofColor.g, roofColor.b,
        -w,    h, -d,  roofColor.r, roofColor.g, roofColor.b,
    });
    
    // 左斜面
    vertices.insert(vertices.end(), {
         0.0f, roofHeight,  d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
        -w,    h,  d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
        -w,    h, -d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
         0.0f, roofHeight,  d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
        -w,    h, -d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
         0.0f, roofHeight, -d,  roofColor.r * 0.8f, roofColor.g * 0.8f, roofColor.b * 0.8f,
    });
    
    // 右斜面
    vertices.insert(vertices.end(), {
         0.0f, roofHeight,  d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
         w,    h, -d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
         w,    h,  d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
         0.0f, roofHeight,  d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
         0.0f, roofHeight, -d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
         w,    h, -d,  roofColor.r * 0.9f, roofColor.g * 0.9f, roofColor.b * 0.9f,
    });
    
    m_VertexCount = vertices.size() / 6;
    
    // 设置 OpenGL 缓冲
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_STATIC_DRAW);
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    std::cout << "[Cabin] Mesh generated with " << m_VertexCount 
              << " vertices (" << m_VertexCount / 3 << " triangles)" << std::endl;
}

void Cabin::Draw(Shader& shader, const std::vector<CabinInstance>& instances) {
    if (instances.empty() || m_VAO == 0) return;
    
    shader.use();
    glBindVertexArray(m_VAO);
    
    for (const auto& cabin : instances) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cabin.position);
        model = glm::rotate(model, cabin.rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(cabin.scale));
        
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    }
    
    glBindVertexArray(0);
}