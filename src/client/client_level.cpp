#include "client_level.h"
#include <unordered_map>
#include "chunk.h"
#include "client_main.h"
#include "position.h"
#include "Remotery.h"

ClientLevel::ClientLevel(ClientMain* client) : debug_generator_(nullptr), client_(client) {}
// 同步来自服务器的数据
void ClientLevel::syncChunks(const std::unordered_map<ChunkPos, LevelChunk>& data, const ChunkPos& camera) {
    rmt_ScopedCPUSample(syncChunks, 0);
    auto vd2 = (Config::VIEW_DISTANCE + 1) * (Config::VIEW_DISTANCE + 1);

    for (auto it = newest_chunks_.begin(); it != newest_chunks_.end();) {
        if (!data.contains(it->first)) {
            it = newest_chunks_.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& kv : data) {
        auto& chunk = kv.second;
        if (camera.dis2(chunk.pos()) > vd2) continue;
        if (!newest_chunks_.contains(chunk.pos())) {
            newest_chunks_.emplace(chunk.pos(), chunk);
        }
    }
    has_new_data_ = true;
}

int ClientLevel::time() const { return client_->time(); }
