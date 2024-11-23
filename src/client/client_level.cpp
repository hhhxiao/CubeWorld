#include "client_level.h"
#include <unordered_map>
#include <vector>
#include "block.h"
#include "chunk.h"
#include "position.h"
#include "terrain_generator.h"

ClientLevel::ClientLevel() : debug_generator_(nullptr) {}

void ClientLevel::syncChunks(const std::vector<LevelChunk>& data) {
    block_lock_.lock();
    this->chunks_ = data;
    idxes_.clear();
    for (auto i = 0; i < chunks_.size(); i++) {
        idxes_[chunks_[i].pos()] = i;
    }
    block_lock_.unlock();
}

BlockType ClientLevel::getBlock(int x, int y, int z, const ChunkPos& src) {
    BlockPos p{x, y, z};
    const auto cp = p.toChunkPos();
    const auto [ox, _, oz] = cp.toBlockPos();
    block_lock_.lock();
    const auto it = this->idxes_.find(cp);
    const bool found = it != this->idxes_.end() && it->second >= 0 && it->second < this->chunks_.size();
    auto* chunk = found ? &this->chunks_[it->second] : nullptr;
    const auto block = found ? chunk->getBlock(x - ox, y, z - oz) : air;
    block_lock_.unlock();
    return block;
}