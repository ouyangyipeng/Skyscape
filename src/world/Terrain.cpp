#include "Terrain.h"
#include <glad/glad.h>
#include <iostream>
#include <cmath>

Terrain::Terrain(int width, int height) : m_Width(width), m_Height(height) {
    generateTerrain();
}

void Terrain::generateTerrain() {
    // Generate vertices with more realistic mountainous terrain using multiple octaves of noise
    for(int z = 0; z < m_Height; ++z) {
        for(int x = 0; x < m_Width; ++x) {
            // Multi-octave noise for realistic mountains
            float scale = 0.02f;
            float y = 0.0f;
            
            // Large mountains
            y += sin(x * scale * 0.5f) * cos(z * scale * 0.5f) * 40.0f;
            y += sin(x * scale * 1.0f + 1.5f) * cos(z * scale * 1.2f + 0.8f) * 20.0f;
            // Medium hills
            y += sin(x * scale * 2.0f + 3.0f) * cos(z * scale * 2.5f + 2.0f) * 10.0f;
            // Small bumps
            y += sin(x * scale * 5.0f) * cos(z * scale * 5.0f) * 3.0f;
            
            m_Vertices.push_back((float)x);
            m_Vertices.push_back(y);
            m_Vertices.push_back((float)z);
            
            // Normals (Approximate)
            m_Vertices.push_back(0.0f);
            m_Vertices.push_back(1.0f);
            m_Vertices.push_back(0.0f);
            
            // TexCoords
            m_Vertices.push_back((float)x / (m_Width - 1));
            m_Vertices.push_back((float)z / (m_Height - 1));
        }
    }
    
    // Generate indices
    for(int z = 0; z < m_Height - 1; ++z) {
        for(int x = 0; x < m_Width - 1; ++x) {
            m_Indices.push_back(z * m_Width + x);
            m_Indices.push_back((z + 1) * m_Width + x);
            m_Indices.push_back(z * m_Width + (x + 1));
            
            m_Indices.push_back((z + 1) * m_Width + x);
            m_Indices.push_back((z + 1) * m_Width + (x + 1));
            m_Indices.push_back(z * m_Width + (x + 1));
        }
    }
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float), &m_Vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // TexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Terrain::Draw(class Shader& shader) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
