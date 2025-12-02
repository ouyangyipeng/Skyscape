#include "Plane.h"
#include <glad/glad.h>
#include <vector>
#include "../graphics/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

Plane::Plane() {
    setupMesh();
}

void Plane::setupMesh() {
    std::vector<glm::vec3> temp_vertices;
    std::vector<float> vertices; // Final buffer: pos(3) + normal(3)

    std::ifstream file("assets/models/airplane.obj");
    if (!file.is_open()) {
        std::cerr << "Failed to open airplane.obj" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        } else if (prefix == "f") {
            std::vector<int> faceIndices;
            std::string segment;
            while (ss >> segment) {
                std::stringstream segmentSS(segment);
                std::string vStr;
                std::getline(segmentSS, vStr, '/');
                try {
                    faceIndices.push_back(std::stoi(vStr) - 1);
                } catch (...) { continue; }
            }

            if (faceIndices.size() >= 3) {
                for (size_t i = 1; i < faceIndices.size() - 1; ++i) {
                    glm::vec3 v0 = temp_vertices[faceIndices[0]];
                    glm::vec3 v1 = temp_vertices[faceIndices[i]];
                    glm::vec3 v2 = temp_vertices[faceIndices[i + 1]];

                    glm::vec3 edge1 = v1 - v0;
                    glm::vec3 edge2 = v2 - v0;
                    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

                    vertices.push_back(v0.x); vertices.push_back(v0.y); vertices.push_back(v0.z);
                    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

                    vertices.push_back(v1.x); vertices.push_back(v1.y); vertices.push_back(v1.z);
                    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

                    vertices.push_back(v2.x); vertices.push_back(v2.y); vertices.push_back(v2.z);
                    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);
                }
            }
        }
    }

    m_VertexCount = vertices.size() / 6;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Plane::Draw(Shader& shader, glm::vec3 position, glm::vec3 direction, float scale) {
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    // Simple rotation to face direction (assuming default is +Z)
    // For a real flight sim, we'd use quaternions or a full orientation matrix
    // Here we just align it to the camera front vector roughly
    
    // Calculate rotation based on direction
    glm::vec3 defaultDir(0.0f, 0.0f, -1.0f); // Our plane points -Z by default? No, vertices are +Z nose.
    // Vertices: Nose is (0,0,1). So it points +Z.
    // Camera Front is usually -Z. 
    // Let's rotate it to match direction.
    
    // Simplified: Just translate for now, maybe rotate later if we have orientation.
    // If we want it to follow the camera, we can just use the camera's position + offset.
    
    // Let's assume the plane is "the player" and we are looking at it, or we are inside it.
    // If we are inside, we don't draw it.
    // If we are 3rd person, we draw it in front of camera.
    
    // Let's just draw it at 'position' with 'scale'
    model = glm::scale(model, glm::vec3(scale));
    
    // Rotate to point forward (if direction is provided)
    // Calculate yaw and pitch from direction vector
    float yaw = atan2(direction.x, direction.z);
    float pitch = asin(-direction.y);

    model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    // Also rotate 180 degrees around Y because our model points +Z but camera looks -Z?
    // Actually, atan2(x, z) gives angle from +Z axis.
    // If direction is (0,0,-1), atan2(0, -1) = pi (180 deg).
    // So it rotates 180 deg.
    // Our model points +Z. Rotating 180 makes it point -Z. Correct.

    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 180); // 5 boxes * 36 vertices
    glBindVertexArray(0);
}
