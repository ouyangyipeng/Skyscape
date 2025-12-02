#pragma once
#include <glm/glm.hpp>

class Plane {
public:
    Plane();
    void Draw(class Shader& shader, glm::vec3 position, glm::vec3 direction, float scale = 1.0f);

private:
    unsigned int VAO, VBO;
    int m_VertexCount = 0;
    void setupMesh();
};
