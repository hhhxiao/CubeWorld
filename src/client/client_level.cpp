#include "client_level.h"
#include <unordered_map>
#include <vector>
#include "block.h"
#include "chunk.h"
#include "position.h"
#include "terrain_generator.h"

ClientLevel::ClientLevel() : debug_generator_(nullptr) {}

void ClientLevel::syncChunks(const std::vector<LevelChunk>& data) {
    this->chunks_ = data;
    idxes_.clear();
    for (auto i = 0; i < chunks_.size(); i++) {
        idxes_[chunks_[i].pos()] = i;
    }
}

BlockType ClientLevel::getBlock(int x, int y, int z, const ChunkPos& src) {
    BlockPos p{x, y, z};
    const auto cp = p.toChunkPos();
    // if (cp != src) return air;  // for debug
    const auto [ox, _, oz] = cp.toBlockPos();
    const auto it = this->idxes_.find(cp);
    if (it == this->idxes_.end() || it->second < 0 || it->second >= this->chunks_.size()) {
        return air;
    }
    return chunks_[it->second].getBlock(x - ox, y, z - oz);
}
