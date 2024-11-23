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

    BlockType getBlock(int x, int y, int z, const ChunkPos &src);

    void syncChunks(const std::vector<LevelChunk> &data);

    const auto getAllChunks() const { return chunks_; }

    bool hasChunk(const ChunkPos &pos) const { return idxes_.find(pos) != idxes_.end(); }

    int8_t neighborMask(const ChunkPos &pos) const {
        auto n1 = (int8_t)hasChunk(ChunkPos{pos.x + 1, pos.z});
        auto n2 = (int8_t)hasChunk(ChunkPos{pos.x - 1, pos.z});
        auto n3 = (int8_t)hasChunk(ChunkPos{pos.x, pos.z + 1});
        auto n4 = (int8_t)hasChunk(ChunkPos{pos.x, pos.z - 1});
        return n1 * 8 + n2 * 4 + n3 * 2 + n4;
    };

   private:
    std::unordered_map<ChunkPos, int> idxes_;
    std::vector<LevelChunk> chunks_;
    std::mutex block_lock_;
    AbstractTerrainGenerator *debug_generator_;
};
#endif
