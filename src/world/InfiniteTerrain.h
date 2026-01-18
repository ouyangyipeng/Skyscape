#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

// Simple hash for chunk coordinates
struct ChunkKey {
    int x, z;
    bool operator==(const ChunkKey& other) const {
        return x == other.x && z == other.z;
    }
};

struct ChunkKeyHash {
    size_t operator()(const ChunkKey& k) const {
        return std::hash<int>()(k.x) ^ (std::hash<int>()(k.z) << 16);
    }
};

struct TerrainChunk {
    unsigned int VAO, VBO, EBO;
    int indexCount;
    glm::vec3 worldPos;
};

class InfiniteTerrain {
public:
    InfiniteTerrain(int chunkSize = 64, int viewDistance = 5);
    ~InfiniteTerrain();
    void Update(glm::vec3 cameraPos);
    void Draw(class Shader& shader);
    float GetHeight(float x, float z) const;
private:
    int m_ChunkSize;
    int m_ViewDistance;
    std::unordered_map<ChunkKey, TerrainChunk, ChunkKeyHash> m_Chunks;
    
    // Texture IDs
    unsigned int m_SnowTex = 0;
    unsigned int m_RockTex = 0;
    unsigned int m_WaterTex = 0;
    
    TerrainChunk GenerateChunk(int chunkX, int chunkZ);
    float Noise(float x, float z) const;
    glm::vec3 GetTerrainColor(float height) const;
    void LoadTerrainTextures();
};
