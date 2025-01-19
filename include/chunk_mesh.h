#ifndef LEVEL_MESH_H
#define LEVEL_MESH_H
#include "chunk.h"
#include "glad/glad.h"
#include "glm/detail/type_vec.hpp"
#include "position.h"
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "block.h"
#include "buffer.h"

class LevelRenderer;
class RenderContext;
class ChunkMeshOld {
    enum MeshStatus { Init = 0, HashGenBuffer, HasBuildMesh, HasBufferData };
    static constexpr auto GRANULARYTY = 128;
    struct V {
        // 坐标
        GLfloat x;
        GLfloat y;
        GLfloat z;
        // 颜色
        GLfloat r{1.0};
        GLfloat g{1.0};
        GLfloat b{1.0};
        GLfloat a{1.0};
        // uv
        GLfloat u;
        GLfloat v;
        // 法线
        GLfloat nx{0.0};
        GLfloat ny{0.0};
        GLfloat nz{0.0};
        void setUV(GLfloat u, GLfloat v) {
            this->u = u;
            this->v = v;
        }
        void setNormal(GLfloat nx, GLfloat ny, GLfloat nz) {
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
        }
        static V fromPos(const glm::vec3& pos) { return {pos.x, pos.y, pos.z}; }
    };

   public:
    explicit ChunkMeshOld(const ChunkPos& pos) : VBO(0), pos_(pos) {}

    ~ChunkMeshOld();

   private:
    void genBuffer();
    void buildMesh(LevelRenderer* level);
    void bufferData();

   public:
    bool stepInit(LevelRenderer* level);
    void render(RenderContext& ctx);
    bool hasInited() const { return status_ == HasBufferData; };

    [[nodiscard]] ChunkPos pos() const { return pos_; }

   private:
    void addFace(const BlockFaceInfo& info);

   private:
    GLuint VBO;
    std::vector<ChunkMeshOld::V> vertices_;
    // textureID -> vert begin vert count
    std::unordered_map<GLuint, std::pair<size_t, size_t>> texture_mappings_;
    ChunkPos pos_;
    std::vector<BlockFaceInfo> fis_;
    //
    int current_build_height_{0};
    MeshStatus status_{ChunkMeshOld::MeshStatus::Init};
};

class ChunkMesh {
    friend class ChunkRenderer;

   public:
    ChunkMesh(const ChunkPos& pos);
    void freeBuckect(ChunkRenderer& chunk_renderer);
    void build(ChunkRenderer& chunk_renderer);
    inline ChunkPos pos() { return pos_; }

   private:
    inline bool buckectFull() {
        return this->buckets_.empty() || ChunkBuffer::BUCKET_SIZE - this->buckets_.back().size < 6;
    }
    void addFace(const BlockFaceInfo& info, ChunkRenderer& chunk_render);
    std::vector<ChunkBuffer::BucketInstance> buckets_;
    const ChunkPos pos_;
};

class ChunkRenderer {
   public:
    void init();
    void updateMesh(LevelRenderer& level_render, RenderContext& ctx);
    void render(RenderContext& ctx);

    inline size_t mesh_size() { return chunk_meshes_.size(); }
    inline size_t mesh_queue_size() { return mesh_building_queue_.size(); }
    inline const std::unordered_map<ChunkPos, ChunkMesh*>& meshs() const { return chunk_meshes_; }
    inline ChunkBuffer& buffer() { return buffer_; }

    inline LevelChunk* getChunk(const ChunkPos& p) {
        auto it = data_.find(p);
        return it == data_.end() ? nullptr : &it->second;
    }

    ~ChunkRenderer();

   private:
    ChunkBuffer buffer_;
    std::unordered_map<ChunkPos, ChunkMesh*> chunk_meshes_;
    ChunkMesh* demo_mesh_;
    // mesh building
    std::unordered_set<ChunkPos> mesh_building_queue_;
    std::unordered_map<ChunkPos, LevelChunk> data_;
    std::unordered_map<ChunkPos, std::tuple<uint8_t, uint8_t>> masks_;

    // stat
   public:
    size_t last_vertices_count;
    size_t last_chunk_count;
};

#endif
