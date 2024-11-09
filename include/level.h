#pragma once

#include <atomic>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <thread>
class Level {
   public:
    Level();

    void tick();

    void serverTick();

    std::unordered_map<std::string, std::string> getStats();

    ~Level();

    // world blocks

    //    public:
    //     inline void updatePlaeryPos(const glm::vec3 &pos) { this->player_->setPos(pos); }
    //     inline AsyncChunkCache &chunkBuilder() { return chunk_builder_; }
    //     inline Player *getPlayer() { return this->player_; }

    //    private:
    //     BlockPos playerPos;
    //     Player *player_;

    // server thread async
    volatile std::atomic_bool stop_{false};
    size_t tick_{0};
    std::thread *main_thread_{nullptr};
};
