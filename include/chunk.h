#pragma once

#include <array>
#include "config.h"
#include "position.h"
#include "block.h"
#include "utils.h"
class Shader;

class AbstractTerrainGenerator;

class LevelChunk {
   public:
    LevelChunk(const ChunkPos& pos);

    LevelChunk(const LevelChunk& chunk);

    void setBlock(int cx, int y, int cz, BlockType type);
    BlockType getBlock(int cx, int y, int cz);

    [[nodiscard]] ChunkPos pos() const { return pos_; }

    void tick(tick_t ts);
    [[nodiscard]] bool isDead(const tick_t now) const { return tick_ != -1 && now - tick_ > Config::CHUNK_LIFE_TICK; }
    [[nodiscard]] bool isDirty() const { return dirty_; }
    void clearDirty() { this->dirty_ = false; }
    void setDirty() { this->dirty_ = true; }

   private:
    bool posValid(int cx, int y, int cz);

   private:
    // data
    ChunkPos pos_{};
    std::array<std::array<std::array<BlockType, 16>, 16>, Config::CHUNK_HEIGHT> data_;
    // status
    tick_t tick_{-1};
    bool dirty_{false};
};
