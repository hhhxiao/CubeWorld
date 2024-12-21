#include "client_level.h"
#include <mutex>
#include <unordered_map>
#include <vector>
#include "chunk.h"
#include "terrain_generator.h"

ClientLevel::ClientLevel() : debug_generator_(nullptr) {}

// 同步来自服务器的数据
void ClientLevel::syncChunks(const std::vector<LevelChunk>& data) {
    std::scoped_lock sc{receive_data_lock_};
    newest_chunks_.clear();
    for (auto& chunk : data) {
        newest_chunks_.emplace(chunk.pos(), chunk);
    }
    has_new_data_ = true;
}
