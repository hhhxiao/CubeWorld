#pragma once
#include <unordered_map>
#include "block.h"
#include "glad/glad.h"
#include "renderable.h"

class TexturePool;
class SubChunkMesh {
   public:
    ~SubChunkMesh();

   public:
    void AddFace(const BlockFaceInfo &face);

    void buildData();
    void sendData();
    void draw();

    std::unordered_map<int, std::unordered_map<int, BlockFaceInfo>> blocks_;
    GLuint VAO{0};
    std::vector<VertexAttribute> vertices_;
    std::vector<GLuint> indices_;
    std::unordered_map<GLuint, std::pair<size_t, size_t>> texture_mappings_;

    // 减少纹理重新绑定
};
