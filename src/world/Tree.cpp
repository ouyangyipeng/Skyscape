#include "Tree.h"
#include "../graphics/Shader.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <iostream>

Tree::Tree() : VAO(0), VBO(0), instanceVBO(0), m_VertexCount(0) {
    generateTreeMesh();
}

Tree::~Tree() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);
}

void Tree::generateTreeMesh() {
    // 简单的树模型：圆柱形树干 + 圆锥形树冠
    std::vector<float> vertices;
    
    // ========== 树干 (Trunk) - 圆柱体 ==========
    int trunkSegments = 8;
    float trunkRadius = 0.2f;
    float trunkHeight = 2.0f;
    
    for(int i = 0; i <= trunkSegments; ++i) {
        float angle = (i / (float)trunkSegments) * 2.0f * 3.14159f;
        float x = cos(angle) * trunkRadius;
        float z = sin(angle) * trunkRadius;
        
        // 底部顶点
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
        vertices.push_back(0.4f); vertices.push_back(0.2f); vertices.push_back(0.1f); // 棕色
        
        // 顶部顶点
        vertices.push_back(x);
        vertices.push_back(trunkHeight);
        vertices.push_back(z);
        vertices.push_back(0.4f); vertices.push_back(0.2f); vertices.push_back(0.1f);
    }
    
    // ========== 树冠 (Foliage) - 圆锥体 ==========
    int foliageSegments = 8;
    float foliageRadius = 1.5f;
    float foliageHeight = 3.0f;
    float foliageStart = trunkHeight - 0.5f;
    
    // 顶点
    vertices.push_back(0.0f);
    vertices.push_back(foliageStart + foliageHeight);
    vertices.push_back(0.0f);
    vertices.push_back(0.1f); vertices.push_back(0.5f); vertices.push_back(0.1f); // 绿色
    
    // 底部圆圈
    for(int i = 0; i <= foliageSegments; ++i) {
        float angle = (i / (float)foliageSegments) * 2.0f * 3.14159f;
        float x = cos(angle) * foliageRadius;
        float z = sin(angle) * foliageRadius;
        
        vertices.push_back(x);
        vertices.push_back(foliageStart);
        vertices.push_back(z);
        vertices.push_back(0.1f); vertices.push_back(0.5f); vertices.push_back(0.1f);
    }
    
    m_MeshVertices = vertices;
    m_VertexCount = vertices.size() / 6;
    
    // Setup OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    std::cout << "[Tree] Mesh generated with " << m_VertexCount << " vertices" << std::endl;
}

float Tree::getTerrainHeight(float x, float z, const std::vector<float>& vertices, 
                            int terrainWidth, int terrainHeight) {
    // ✅ 修复：将世界坐标转换为网格坐标（除以2，因为地形顶点缩放了2倍）
    float gridX = x / 2.0f;
    float gridZ = z / 2.0f;
    
    // 检查边界
    if(gridX < 0.0f || gridX >= terrainWidth - 1 || 
       gridZ < 0.0f || gridZ >= terrainHeight - 1) {
        return 0.0f;
    }
    
    // 转换为整数索引
    int x0 = static_cast<int>(gridX);
    int z0 = static_cast<int>(gridZ);
    
    // 获取四个角的高度（每个顶点占8个float: pos(3) + normal(3) + texcoord(2)）
    int idx00 = (z0 * terrainWidth + x0) * 8;
    int idx10 = (z0 * terrainWidth + (x0 + 1)) * 8;
    int idx01 = ((z0 + 1) * terrainWidth + x0) * 8;
    int idx11 = ((z0 + 1) * terrainWidth + (x0 + 1)) * 8;
    
    // 确保索引有效
    if(idx11 + 1 >= static_cast<int>(vertices.size())) {
        return 0.0f;
    }
    
    float h00 = vertices[idx00 + 1]; // Y coordinate
    float h10 = vertices[idx10 + 1];
    float h01 = vertices[idx01 + 1];
    float h11 = vertices[idx11 + 1];
    
    // ✅ 双线性插值（更精确的高度）
    float tx = gridX - x0;
    float tz = gridZ - z0;
    
    float h0 = h00 * (1.0f - tx) + h10 * tx;
    float h1 = h01 * (1.0f - tx) + h11 * tx;
    float height = h0 * (1.0f - tz) + h1 * tz;
    
    return height;
}

void Tree::GenerateTrees(int count, int terrainWidth, int terrainHeight, 
                        const std::vector<float>& terrainVertices) {
    m_Instances.clear();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // ✅ 修复：生成范围考虑2倍缩放（世界坐标范围是 0 到 terrainWidth*2）
    float worldWidth = terrainWidth * 2.0f;
    float worldHeight = terrainHeight * 2.0f;
    
    std::uniform_real_distribution<float> posX(10.0f, worldWidth - 10.0f);
    std::uniform_real_distribution<float> posZ(10.0f, worldHeight - 10.0f);
    std::uniform_real_distribution<float> scale(0.8f, 1.5f);
    std::uniform_real_distribution<float> rotation(0.0f, 6.28318f);
    
    for(int i = 0; i < count; ++i) {
        TreeInstance tree;
        tree.position.x = posX(gen);
        tree.position.z = posZ(gen);
        tree.position.y = getTerrainHeight(tree.position.x, tree.position.z, 
                                          terrainVertices, terrainWidth, terrainHeight);
        
        // ✅ 调整高度范围（草地和森林区域，避免水下和高山）
        if(tree.position.y > 15.0f && tree.position.y < 100.0f) {
            tree.scale = scale(gen);
            tree.rotation = rotation(gen);
            m_Instances.push_back(tree);
        }
    }
    
    std::cout << "[Tree] Generated " << m_Instances.size() << " trees (from " 
              << count << " attempts)" << std::endl;
}
void Tree::Draw(Shader& shader) {
    if(m_Instances.empty()) return;
    
    shader.use();
    glBindVertexArray(VAO);
    
    for(const auto& tree : m_Instances) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, tree.position);
        model = glm::rotate(model, tree.rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(tree.scale));
        
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_VertexCount);
    }
    
    glBindVertexArray(0);
}