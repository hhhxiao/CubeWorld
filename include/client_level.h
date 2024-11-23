#ifndef CLIENT_LEVEL_H
#define CLIENT_LEVEL_H
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "block.h"
#include "chunk.h"
#include "position.h"

class AbstractTerrainGenerator;

class ClientLevel {
   public:
    ClientLevel();

    BlockType getBlock(int x, int y, int z, const ChunkPos& src);

    void syncChunks(const std::vector<LevelChunk>& data);

    auto getAllChunks() const { return chunks_; }

    [[nodiscard]] bool hasChunk(const ChunkPos& pos) {
        block_lock_.lock();
        auto res = idxes_.find(pos) != idxes_.end();
        block_lock_.unlock();
        return res;
    }

    [[nodiscard]] int8_t neighborMask(const ChunkPos& pos) {
        block_lock_.lock();
        auto n1 = static_cast<int8_t>(idxes_.find({pos.x + 1, pos.z}) != idxes_.end());
        auto n2 = static_cast<int8_t>(idxes_.find({pos.x - 1, pos.z}) != idxes_.end());
        auto n3 = static_cast<int8_t>(idxes_.find({pos.x, pos.z + 1}) != idxes_.end());
        auto n4 = static_cast<int8_t>(idxes_.find({pos.x, pos.z - 1}) != idxes_.end());
        block_lock_.unlock();
        return n1 * 8 + n2 * 4 + n3 * 2 + n4;
    };

   private:
    std::unordered_map<ChunkPos, int> idxes_;
    std::vector<LevelChunk> chunks_;
    std::mutex block_lock_;
    AbstractTerrainGenerator* debug_generator_;
};
#endif
