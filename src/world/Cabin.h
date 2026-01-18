#pragma once
#include <glm/glm.hpp>
#include <vector>

class Shader;

struct CabinInstance {
    glm::vec3 position;
    float rotation;
    float scale;
};

class Cabin {
public:
    Cabin();
    ~Cabin();
    
    void Draw(Shader& shader, const std::vector<CabinInstance>& instances);
    
private:
    void generateCabinMesh();
    
    unsigned int m_VAO;
    unsigned int m_VBO;
    int m_VertexCount;
};