#pragma once

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>
#include "block.h"
enum Face { nx = 0, px = 1, ny = 2, py = 3, nz = 4, pz = 5 };

class TexturePool;

class SubChunkMesh {
    // 方块一个面的信息
    struct BlockFaceInfo {
        Face face;
        BlockType type;
        BlockPos pos;
    };

   public:
    ~SubChunkMesh();

   public:
    void AddFace(const BlockFaceInfo &face);

    void buildData();
    void sendData();
    void draw(TexturePool *pool);

    std::unordered_map<int, std::unordered_map<int, BlockFaceInfo>> blocks_;
    GLuint VAO{0};
    std::vector<VertexAttribute> vertices_;
    std::vector<GLuint> indices_;
    // 减少纹理重新绑定
    std::unordered_map<BlockType, std::unordered_map<Face, std::vector<size_t>>> texture_map_;
};