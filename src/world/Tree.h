#pragma once
#include <glm/glm.hpp>
#include <vector>

class Shader;

class Tree {
public:
    Tree();
    ~Tree();
    
    void GenerateTrees(int count, int terrainWidth, int terrainHeight, 
                      const std::vector<float>& terrainVertices);
    void Draw(Shader& shader);
    
private:
    struct TreeInstance {
        glm::vec3 position;
        float scale;
        float rotation;
    };
    
    void generateTreeMesh();
    float getTerrainHeight(float x, float z, const std::vector<float>& vertices, 
                          int terrainWidth, int terrainHeight);
    
    unsigned int VAO, VBO, instanceVBO;
    std::vector<float> m_MeshVertices;
    std::vector<TreeInstance> m_Instances;
    int m_VertexCount;
};