#pragma once

#include <atomic>
#include <cstddef>
#include "bridge.h"
#include "chunk_builder.h"
#include "glm/detail/type_vec.hpp"
#include "player.h"
#include "utils.h"
#include "time_counter.h"

class LevelServer {
   public:
    explicit LevelServer(DataBridge* bridge);

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

   public:  // commands
    inline void setTime(size_t time) { this->time_ = time; }

   private:
    Player* player_;

    std::atomic_bool stop_{false};
    tick_t tick_{0};
    ChunkBuilder* chunk_builder_{nullptr};
    DataBridge* bridge_{nullptr};
    // current time
    size_t time_{0};
    // statics
    PeriodTimer mspt_timer_;
};
