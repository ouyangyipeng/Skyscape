#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Stars {
public:
    Stars(int count = 2000);
    ~Stars();
    void Draw(unsigned int shaderProgram, float visibility);

private:
    unsigned int m_VAO, m_VBO;
    int m_StarCount;
    void GenerateStars();
};
