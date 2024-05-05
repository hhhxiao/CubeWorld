#include "level.h"
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include "block.h"
#include "chunk.h"
#include "config.h"
#include "subchunk_mesh.h"
#include "utils.h"

namespace {

    std::vector<ChunkPos> getActiveChunkList(BlockPos player, int radius) {
        std::vector<ChunkPos> list;
        auto cx = player.x < 0 ? player.x - 15 : player.x;
        auto cz = player.z < 0 ? player.z - 15 : player.z;
        cx /= 16;
        cz /= 16;
        for (int i = -radius; i <= radius; i++) {
            for (int j = -radius; j <= radius; j++) {
                if (i * i + j * j < radius * radius) {
                    list.push_back({cx + i, cz + j});
                }
            }
        }
        return list;
    }
}  // namespace

Level::Level() {}

void Level::draw(Shader *shader) {
    auto activeChunkList = getActiveChunkList(this->playerPos, Config::load_radius);
    for (auto &pos : activeChunkList) {
        auto *chunk = this->chunk_cache_.getChunk(pos);
        if (!chunk) continue;
        chunk->trySendData();
        chunk->draw(shader);
    }
}

void Level::updatePlayerPos(const BlockPos &pos) { this->playerPos = pos; }

std::unordered_map<std::string, size_t> Level::getChunkStats() {
    std::unordered_map<std::string, size_t> data;
    data["Active chunks"] = this->chunk_cache_.ActiveChunkCount();
    data["Chunks in queue"] = this->chunk_cache_.ChunkInTaskCount();
    return data;
}

Level::~Level() {}