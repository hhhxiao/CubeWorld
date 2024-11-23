#include "chunk.h"
#include "block.h"
#include "config.h"
#include "position.h"
#include "terrain_generator.h"

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
    this->dirty_ = chunk.dirty_;
}
void LevelChunk::tick(tick_t ts) {
    this->tick_ = ts;
    // Noting todo
}
BlockType LevelChunk::getBlock(int cx, int y, int cz) {
    if (!posValid(cx, y, cz)) return BlockType::air;
    return this->data_[y][cx][cz];
}
bool LevelChunk::posValid(int cx, int y, int cz) {
    return cx >= 0 && cx < 16 && cz >= 0 && cz < 16 && y >= 0 && y < Config::CHUNK_HEIGHT;
}