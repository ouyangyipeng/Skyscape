#include "Grid.h"
#include <glad/glad.h>
#include <vector>
#include "../graphics/Shader.h"
#include <glm/gtc/matrix_transform.hpp>

Grid::Grid(int size, int step) : m_Size(size), m_Step(step) {
    setupMesh();
}

void Grid::setupMesh() {
    std::vector<float> vertices;
    
    // Lines along X axis
    for (int z = -m_Size; z <= m_Size; z += m_Step) {
        vertices.push_back((float)-m_Size); vertices.push_back(0.0f); vertices.push_back((float)z); // Start
        vertices.push_back((float)m_Size);  vertices.push_back(0.0f); vertices.push_back((float)z); // End
    }

    // Lines along Z axis
    for (int x = -m_Size; x <= m_Size; x += m_Step) {
        vertices.push_back((float)x); vertices.push_back(0.0f); vertices.push_back((float)-m_Size); // Start
        vertices.push_back((float)x); vertices.push_back(0.0f); vertices.push_back((float)m_Size);  // End
    }

    m_VertexCount = vertices.size() / 3;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Grid::Draw(Shader& shader) {
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -20.0f, 0.0f)); // Lower grid
    shader.setMat4("model", model);
    shader.setVec3("objectColor", glm::vec3(0.5f, 0.5f, 0.5f)); // Grey grid

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, m_VertexCount);
    glBindVertexArray(0);
}
