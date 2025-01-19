#include "client_level.h"
#include <unordered_map>
#include <vector>
#include "chunk.h"
#include "position.h"
#include "Remotery.h"

ClientLevel::ClientLevel() : debug_generator_(nullptr) {}

// 同步来自服务器的数据
void ClientLevel::syncChunks(const std::vector<LevelChunk>& data, const ChunkPos& camera) {
    rmt_ScopedCPUSample(syncChunks, 0);
    auto vd2 = (Config::VIEW_DISTANCE + 1) * (Config::VIEW_DISTANCE + 1);
    newest_chunks_.clear();
    for (auto& chunk : data) {
        if (camera.dis2(chunk.pos()) > vd2) continue;
        newest_chunks_.emplace(chunk.pos(), chunk);
    }
    has_new_data_ = true;
}
