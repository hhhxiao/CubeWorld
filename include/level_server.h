#pragma once

#include <atomic>
#include <cstddef>
#include "bridge.h"
#include "glm/detail/type_vec.hpp"
#include "player.h"

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
    size_t tick_{0};
    DataBridge* bridge_{nullptr};
};
