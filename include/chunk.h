#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "config.h"
#include "position.h"
#include "block.h"
#include "subchunk_mesh.h"
#include "utils.h"
class Shader;

class AbstractTerrainGenerator;

// // 16 * 16 * 256
// class SubChunk {
//    public:
//     SubChunk(const ChunkPos& pos, int sub_chunk_index);
//     void createMesh();
//     BlockType getBlock(int sx, int sy, int sz);
//     void setBlock(int sx, int sy, int sz, BlockType type);

//     ~SubChunk() { delete this->mesh_; }

//     void draw(Shader* shader);

//     void trySendData();

//    private:
//     bool hasBlock(int sx, int sy, int sz);
//     std::array<std::array<std::array<BlockType, 16>, 16>, 16> block_volume_;
//     ChunkPos chunk_pos_;
//     int sub_chunk_index_;
//     SubChunkMesh* mesh_{nullptr};
//     bool has_send_data_{false};
// };

// class Chunk {
//    public:
//     Chunk(const ChunkPos& pos, AbstractTerrainGenerator* generator);

//     void setBlock(int cx, int y, int cz, BlockType type);

//     void draw(Shader* shader);
//     void trySendData();
//     inline ChunkPos pos() const { return this->pos_; }

//     ~Chunk();

//    private:
//     void init(AbstractTerrainGenerator* generator);

//     ChunkPos pos_{};
//     std::array<SubChunk*, 16> sub_chunks_;
// };

class LevelChunk {
   public:
    LevelChunk(const ChunkPos& pos);
    void setBlock(int cx, int y, int cz, BlockType type);
    BlockType getBlock(int cx, int y, int cz);

    void tick(tick_t ts);
    bool isDead(tick_t now) { return tick_ != -1 && now - tick_ > Config::CHUNK_LIFE_TICK; }

   private:
    bool posValid(int cx, int y, int cz);

   private:
    tick_t tick_{-1};
    ChunkPos pos_{};
    bool dirty{false};
    std::array<std::array<std::array<BlockType, 16>, 16>, Config::CHUNK_HEIGHT> data_;
};
