#pragma once
#include <vector>

class Terrain {
public:
    Terrain(int width, int height);
    void Draw(class Shader& shader);
private:
    unsigned int VAO, VBO, EBO;
    int m_Width, m_Height;
    std::vector<float> m_Vertices;
    std::vector<unsigned int> m_Indices;
    
    void generateTerrain();
};
