#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include "block.h"
#include "chunk.h"
#include "chunk_builder.h"
#include "glm/detail/type_vec.hpp"
#include "player.h"
#include "simple_shader.h"

class Level {
   public:
    Level();

    void tick();

    void draw(Shader *shader);

    std::unordered_map<std::string, std::string> getStats();

    ~Level();

    // world blocks

   public:
    inline void updatePlaeryPos(const glm::vec3 &pos) { this->player_->setPos(pos); }
    inline AsyncChunkCache &chunkBuilder() { return chunk_builder_; }
    inline Player *getPlayer() { return this->player_; }

   private:
    BlockPos playerPos;
    AsyncChunkCache chunk_builder_;
    size_t chunk_count_{0};
    Player *player_;
};
