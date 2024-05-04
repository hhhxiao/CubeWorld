#pragma once

#include <array>
#include "block.h"
#include "subchunk_mesh.h"
class Shader;

class AbstractTerrainGenerator;
struct ChunkPos {
    int x{0};
    int z{0};
};

// 16 * 16 * 256
class SubChunk {
   public:
    SubChunk(const ChunkPos& pos, int sub_chunk_index);
    void createMesh();
    BlockType getBlock(int sx, int sy, int sz);
    void setBlock(int sx, int sy, int sz, BlockType type);

    ~SubChunk() { delete this->mesh_; }

    void draw(TexturePool* pool, Shader* shader);

   private:
    bool hasBlock(int sx, int sy, int sz);
    void freeMesh();
    std::array<std::array<std::array<BlockType, 16>, 16>, 16> block_volume_;
    ChunkPos chunk_pos_;
    int sub_chunk_index_;
    SubChunkMesh* mesh_{nullptr};
    bool mesh_is_dirty_{false};  // 用于更新方块，暂时不管
};

class Chunk {
   public:
    Chunk(const ChunkPos& pos);

    void setBlock(int cx, int y, int cz, BlockType type);
    void init(AbstractTerrainGenerator* generator);
    ~Chunk();
    void draw(TexturePool* pool, Shader* shader);

   private:
    ChunkPos pos{};
    std::array<SubChunk*, 16> sub_chunks_;
};
