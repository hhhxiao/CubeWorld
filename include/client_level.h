#ifndef CLIENT_LEVEL_H
#define CLIENT_LEVEL_H
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "chunk.h"
#include "position.h"

class AbstractTerrainGenerator;

class ClientLevel {
   public:
    ClientLevel();

    void syncChunks(const std::vector<LevelChunk>& data);

    [[nodiscard]] auto fetchChunkData() {
        std::scoped_lock<std::mutex> lock{this->receive_data_lock_};
        return this->newest_chunks_;
    }

    auto newDataReceived() -> bool const { return has_new_data_; }

   private:
    AbstractTerrainGenerator* debug_generator_;

    std::mutex receive_data_lock_;
    std::unordered_map<ChunkPos, LevelChunk> newest_chunks_;  // receive data from server
    std::atomic_bool has_new_data_{false};
};
#endif
