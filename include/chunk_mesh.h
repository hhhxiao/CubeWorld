#ifndef LEVEL_MESH_H
#define LEVEL_MESH_H
#include "chunk.h"
#include "glad/glad.h"
#include "position.h"
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "block.h"
#include "buffer.h"

class LevelRenderer;
class RenderContext;

class ChunkMesh {
    friend class ChunkRenderer;

   public:
    ChunkMesh(const ChunkPos& pos);
    void freeBuckect(ChunkRenderer& chunk_renderer);
    void build(ChunkRenderer& chunk_renderer);
    inline ChunkPos pos() { return pos_; }

   private:
    inline bool buckectFull(const std::vector<ChunkBuffer::BucketInstance>& buckets) {
        return buckets.empty() || ChunkBuffer::BUCKET_SIZE - buckets.back().size < 6;
    }
    void addFace(const BlockFaceInfo& info, ChunkBuffer& buffer, bool solid);
    std::vector<ChunkBuffer::BucketInstance> solid_buckets_;
    std::vector<ChunkBuffer::BucketInstance> translucent_buckets_;
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
    inline ChunkBuffer& solid_buffer() { return solid_buffer_; }
    inline ChunkBuffer& translucent_buffer() { return translucent_buffer_; }
    inline LevelChunk* getChunk(const ChunkPos& p) {
        auto it = data_.find(p);
        return it == data_.end() ? nullptr : &it->second;
    }

    ~ChunkRenderer();

   private:
    // buffer
    ChunkBuffer solid_buffer_;
    ChunkBuffer translucent_buffer_;
    // mesh building
    std::unordered_map<ChunkPos, ChunkMesh*> chunk_meshes_;
    std::unordered_set<ChunkPos> mesh_building_queue_;
    std::unordered_map<ChunkPos, LevelChunk> data_;
    std::unordered_map<ChunkPos, std::tuple<uint8_t, uint8_t>> masks_;

    // stat
   public:
    size_t last_vertices_count;
    size_t last_chunk_count;
};

#endif
