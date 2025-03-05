#ifndef CLIENT_LEVEL_H
#define CLIENT_LEVEL_H
#include <atomic>
#include <unordered_map>
#include "chunk.h"
#include "position.h"

class AbstractTerrainGenerator;
class ClientMain;
class ClientLevel {
   public:
    ClientLevel(ClientMain* client);

    void syncChunks(const std::unordered_map<ChunkPos, LevelChunk>& data, const ChunkPos& camera);

    [[nodiscard]] std::unordered_map<ChunkPos, LevelChunk>& newestChunks() { return this->newest_chunks_; }
    auto newDataReceived() -> bool const { return has_new_data_; }
    int time() const;

   private:
    AbstractTerrainGenerator* debug_generator_;
    std::unordered_map<ChunkPos, LevelChunk> newest_chunks_;  // receive data from server
    std::atomic_bool has_new_data_{false};
    ClientMain* client_{nullptr};
};
#endif
