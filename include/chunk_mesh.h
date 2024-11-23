#ifndef LEVEL_MESH_H
#define LEVEL_MESH_H

#include "glad/glad.h"
#include "glm/detail/type_vec.hpp"
#include "position.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>
#include "block.h"
#include "utils.h"

class ClientLevel;
class RenderContext;

class ChunkMesh {
    friend class ChunkMeshMgr;
    struct V {
        // 坐标
        GLfloat x;
        GLfloat y;
        GLfloat z;
        // 颜色
        GLfloat r;
        GLfloat g;
        GLfloat b;
        // uv
        GLfloat u;
        GLfloat v;
        // 法线
        GLfloat nx;
        GLfloat ny;
        GLfloat nz;

        static V fromPos(const glm::vec3& pos) { return {pos.x, pos.y, pos.z}; }
    };

   public:
    explicit ChunkMesh(const ChunkPos& pos) : pos_(pos) {}
    ~ChunkMesh();

   public:
    void init();

    void buildMesh(ClientLevel* level);
    void render(RenderContext& ctx);

   public:                                  // lifecycle
    bool isDead() { return tick_ > 2400; }  // 2400帧率后析构
    bool renderTick() { tick_++; }

   public:
    int8_t neighbor_mask() const { return neighbor_mask_; }

   private:
    void addFace(const BlockFaceInfo& info);

   private:
    GLuint VBO;  // EBO is not needed here
    std::vector<ChunkMesh::V> vertices_;
    // textureID -> vert begin vert count
    std::unordered_map<GLuint, std::pair<size_t, size_t>> texture_mappings_;
    ChunkPos pos_;
    // for lifestype
    tick_t tick_{-1};
    // neighbor
    int8_t neighbor_mask_{0};
};

// Mesh Manager
class ChunkMeshMgr {
   public:
    void update(RenderContext& ctx, ClientLevel* level);
    void render(RenderContext& ctx);
    void foreachRenderChunk(RenderContext& ctx, const std::function<void(const ChunkPos&)>& f);

   private:
    std::unordered_map<ChunkPos, ChunkMesh*> meshes_;
};

#endif
