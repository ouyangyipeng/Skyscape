#pragma once
#include <glm/glm.hpp>

class Grid {
public:
    Grid(int size = 100, int step = 10);
    void Draw(class Shader& shader);

private:
    unsigned int VAO, VBO;
    int m_Size;
    int m_Step;
    int m_VertexCount;
    void setupMesh();
};
