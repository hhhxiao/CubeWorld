#pragma once

#include <array>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include "PerlinNoise.hpp"
#include "config.h"
#include "position.h"
#include "block.h"
#include "utils.h"
class Shader;

class AbstractTerrainGenerator;

class LevelChunk {
    struct ChunkBlock {
        BlockType type;
        // noise cache
        siv::PerlinNoise::value_type sea;
    };

   public:
    LevelChunk(const ChunkPos& pos);

    LevelChunk(const LevelChunk& chunk);

    BlockType getBlock(int cx, int y, int cz);
    ChunkBlock* getPosition(int cx, int y, int cz);
    [[nodiscard]] ChunkPos pos() const { return pos_; }
    void tick(tick_t ts);
    [[nodiscard]] bool isDead(const tick_t now) const { return tick_ != -1 && now - tick_ > Config::CHUNK_LIFE_TICK; }
    tick_t ts() const { return tick_; }

    // [[nodiscard]] bool isDirty() const { return dirty_; }
    // void clearDirty() { this->dirty_ = false; }
    // void setDirty() { this->dirty_ = true; }
    int topY(int cx, int cz) const;

   public:
    static std::tuple<uint8_t, uint8_t> adjacentMask(const ChunkPos& pos,
                                                     const std::unordered_map<ChunkPos, LevelChunk>& chunks);
    bool posValid(int cx, int y, int cz);

   private:
    // data
    ChunkPos pos_{};
    std::array<std::array<std::array<ChunkBlock, 16>, 16>, Config::CHUNK_HEIGHT> data_;
    // status
    tick_t tick_{-1};
    bool enable_valid_check_{false};
    // bool dirty_{false};
};
