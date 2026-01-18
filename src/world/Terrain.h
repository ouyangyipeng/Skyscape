#pragma once
#include <vector>

class Terrain {
public:
    Terrain(int width, int height);
    void Draw(class Shader& shader);

    // ✅ 添加获取顶点数据的方法
    const std::vector<float>& getVertices() const { return m_Vertices; }
    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }
private:
    unsigned int VAO, VBO, EBO;
    int m_Width, m_Height;
    std::vector<float> m_Vertices;
    std::vector<unsigned int> m_Indices;
    
    void generateTerrain();
};
