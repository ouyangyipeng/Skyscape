#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <memory>

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

struct CabinData {
    glm::vec3 position;
    float rotation;
    float scale;
};


// ✅ 添加花丛数据结构
struct FlowerPatchData {
    glm::vec3 position;
    float rotation;
    float scale;
    glm::vec3 color; // 花朵颜色（红、黄、粉、紫、白）
};

// ✅ 添加小船数据结构
struct BoatData {
    glm::vec3 position;
    float rotation;
    float scale;
    float bobPhase; // 波浪起伏相位
};

struct TerrainChunk {
    unsigned int VAO, VBO, EBO;
    int indexCount;
    glm::vec3 worldPos;

    // ✅ 添加树木数据
    std::vector<glm::vec3> treePositions;
    std::vector<float> treeScales;
    std::vector<float> treeRotations;

    // ✅ 添加小屋数据
    std::vector<CabinData> cabins;

    // ✅ 花丛数据
    std::vector<FlowerPatchData> flowerPatches;

    // ✅ 小船数据
    std::vector<BoatData> boats;
};

class InfiniteTerrain {
public:
    InfiniteTerrain(int chunkSize = 64, int viewDistance = 5);
    ~InfiniteTerrain();
    
    void Update(glm::vec3 cameraPos);
    void Draw(class Shader& shader);
    void DrawTrees(class Shader& treeShader); // ✅ 添加此声明
    void DrawCabins(class Shader& cabinShader); 
    void DrawFlowers(class Shader& flowerShader);
    void DrawBoats(class Shader& boatShader, float time); 
    float GetHeight(float x, float z) const;

private:
    int m_ChunkSize;
    int m_ViewDistance;
    std::unordered_map<ChunkKey, TerrainChunk, ChunkKeyHash> m_Chunks;

    // ✅ 添加树木网格（共享，不用每棵树创建）
    unsigned int m_TreeVAO;
    unsigned int m_TreeVBO;
    int m_TreeVertexCount;

    // ✅ 小屋网格
    unsigned int m_CabinVAO;
    unsigned int m_CabinVBO;
    int m_CabinVertexCount;

    // ✅ 花丛网格
    unsigned int m_FlowerVAO;
    unsigned int m_FlowerVBO;
    int m_FlowerVertexCount;

    // ✅ 小船网格
    unsigned int m_BoatVAO;
    unsigned int m_BoatVBO;
    int m_BoatVertexCount;
    
    TerrainChunk GenerateChunk(int chunkX, int chunkZ);
    void GenerateTreesForChunk(TerrainChunk& chunk, int chunkX, int chunkZ); 
    void GenerateCabinsForChunk(TerrainChunk& chunk, int chunkX, int chunkZ); 
    void GenerateFlowersForChunk(TerrainChunk& chunk, int chunkX, int chunkZ); // ✅ 添加
    void GenerateBoatsForChunk(TerrainChunk& chunk, int chunkX, int chunkZ); 
    void InitializeTreeMesh(); 
    void InitializeCabinMesh(); // ✅ 新增
    void InitializeFlowerMesh(); // ✅ 添加
    void InitializeBoatMesh(); 
    float Noise(float x, float z) const;
    glm::vec3 GetTerrainColor(float height) const;
};
