#ifndef LEVEL_MESH_H
#define LEVEL_MESH_H

#include "block.h"
#include <unordered_map>
#include "chunk_builder.h"
#include "drawable_object.h"
class TexturePool;
class AsyncChunkCache;
class LevelMesh {
   public:
    void rebuild(ChunkBuilder* builder);
    void sendData();
    void render();

   private:
    bool valid_{true};

    std::unordered_map<int, std::unordered_map<int, BlockFaceInfo>> blocks_;
    GLuint VAO{0};
    std::vector<VertexAttribute> vertices_;
    std::vector<GLuint> indices_;
    std::unordered_map<GLuint, std::pair<size_t, size_t>> texture_mappings_;
};

#endif
