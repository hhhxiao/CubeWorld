#include "level.h"
#include <cstddef>
#include <string>
#include <unordered_map>
#include "block.h"
#include "chunk.h"
#include "chunk_builder.h"
#include "player.h"

namespace {

    // std::vector<ChunkPos> getActiveChunkList(BlockPos player, int radius) {
    //     std::vector<ChunkPos> list;
    //     auto cx = player.x < 0 ? player.x - 15 : player.x;
    //     auto cz = player.z < 0 ? player.z - 15 : player.z;
    //     cx /= 16;
    //     cz /= 16;
    //     for (int i = -radius; i <= radius; i++) {
    //         for (int j = -radius; j <= radius; j++) {
    //             if (i * i + j * j < radius * radius) {
    //                 list.push_back({cx + i, cz + j});
    //             }
    //         }
    //     }
    //     return list;
    // }
}  // namespace

Level::Level() {}

void Level::tick() { this->player_ = new Player(); }
void Level::draw(Shader *shader) {
    auto *chunk = chunk_builder_.postGetChunk({0, 0});
    static bool sent{false};
    if (chunk) {
        if (!sent) {
            chunk->trySendData();
            sent = true;
        }
        chunk->draw(shader);
    }

    // auto activeChunkList = getActiveChunkList(this->playerPos, Config::load_radius);
    // this->chunk_count_ = 0;
    // for (auto &pos : activeChunkList) {
    //     auto *chunk = this->chunk_cache_.getChunk(pos);
    //     if (!chunk) continue;
    //     this->chunk_count_++;
    //     chunk->trySendData();
    //     chunk->draw(shader);
    // }
}

std::unordered_map<std::string, std::string> Level::getStats() {
    std::unordered_map<std::string, size_t> data;
    return {{"Active chunks", std::to_string(chunk_builder_.ActiveChunkCount())},
            {"Chunks in queue", std::to_string(chunk_builder_.ChunkInTaskCount())},
            {"Rendered chunks", std::to_string(this->chunk_count_)}};
}

Level::~Level() { delete player_; }
