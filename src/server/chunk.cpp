#include "chunk.h"
#include <cstdint>
#include "block.h"
#include "config.h"
#include "position.h"
#include "terrain_generator.h"
#include <bitset>

LevelChunk::LevelChunk(const ChunkPos& pos) : pos_(pos.x, pos.z) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < Config::CHUNK_HEIGHT; y++) {
                this->data_[y][x][z] = air;
            }
        }
    }
}

void LevelChunk::setBlock(int cx, int y, int cz, BlockType type) {
    if (!posValid(cx, y, cz)) return;
    this->data_[y][cx][cz] = type;
}

// copy chunks
LevelChunk::LevelChunk(const LevelChunk& chunk) {
    this->pos_ = chunk.pos_;
    this->data_ = chunk.data_;
    this->tick_ = chunk.tick_;
    // this->dirty_ = chunk.dirty_;
}

void LevelChunk::tick(const tick_t ts) {
    this->tick_ = ts;
    // Noting todo
}

BlockType LevelChunk::getBlock(int cx, int y, int cz) {
    if (!posValid(cx, y, cz)) return air;
    return this->data_[y][cx][cz];
}
int LevelChunk::topY(int cx, int cz) const {
    for (int y = Config::CHUNK_HEIGHT - 1; y >= 0; y--) {
        if (this->data_[y][cx][cz] != air) {
            return y;
        }
    }
    return -1;
}

bool LevelChunk::posValid(int cx, int y, int cz) {
    return cx >= 0 && cx < 16 && cz >= 0 && cz < 16 && y >= 0 && y < Config::CHUNK_HEIGHT;
}
std::tuple<uint8_t, uint8_t> LevelChunk::adjacentMask(const ChunkPos& pos,
                                                      const std::unordered_map<ChunkPos, LevelChunk>& chunks) {
    std::bitset<4> e{0}, d{0};
    auto i1 = chunks.find(ChunkPos{pos.x - 1, pos.z});
    auto i2 = chunks.find(ChunkPos{pos.x + 1, pos.z});
    auto i3 = chunks.find(ChunkPos{pos.x, pos.z - 1});
    auto i4 = chunks.find(ChunkPos{pos.x, pos.z + 1});
    auto ie = chunks.end();
    e.set(0, i1 == ie);
    e.set(1, i2 == ie);
    e.set(2, i3 == ie);
    e.set(3, i4 == ie);
    // if (i1 != ie) d.set(0, i1->second.isDirty());
    // if (i2 != ie) d.set(1, i2->second.isDirty());
    // if (i3 != ie) d.set(2, i3->second.isDirty());
    // if (i4 != ie) d.set(3, i4->second.isDirty());
    return {static_cast<uint8_t>(e.to_ulong()), static_cast<uint8_t>(d.to_ulong())};
}
