#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include "bridge.h"
#include "chunk_builder.h"
#include "glm/detail/type_vec.hpp"
#include "player.h"
#include "utils.h"

class LevelServer {
   public:
    LevelServer(DataBridge* bridge);

    void handleClientRequest(const glm::vec3& cameraPos);

    void tick();

    void start();
    void stop();

    ~LevelServer();

   private:
    void syncRead();
    void syncWrite();

    // ticking
    void tickChunks();

   private:
    // world blocks
    //    public:
    //     inline void updatePlaeryPos(const glm::vec3 &pos) { this->player_->setPos(pos); }
    //     inline AsyncChunkCache &chunkBuilder() { return chunk_builder_; }
    //     inline Player *getPlayer() { return this->player_; }

    //    private:
    //     BlockPos playerPos;

    Player* player_;

    std::atomic_bool stop_{false};
    tick_t tick_{0};
    ChunkBuilder* chunk_builder_{nullptr};
    DataBridge* bridge_{nullptr};
};
