#include "level.h"
#include <cstdint>
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

Level::Level() { this->generator = new FlatTerrainGenerator(); }

Chunk *Level::generateNewChunk(const ChunkPos &pos) {
    auto *chunk = new Chunk(pos);
    chunk->init(this->generator);
    while (this->lru_.size() > Config::chunk_cache_size) {
        auto last = this->lru_.oldest();
        auto it = this->chunks_.find(last);
        if (it != this->chunks_.end()) {
            delete it->second;
            this->chunks_.erase(it);
        }
    }
    auto hash = pos.hash();
    lru_.insert(hash);
    this->chunks_[hash] = chunk;
    return chunk;
}

void Level::draw(TexturePool *pool, Shader *shader) {
    auto activeChunkList = getActiveChunkList(this->playerPos, Config::load_radius);
    for (auto &pos : activeChunkList) {
        auto hash = pos.hash();
        auto it = this->chunks_.find(hash);
        if (it == this->chunks_.end()) {
            auto *chunk = this->generateNewChunk(pos);
            chunk->draw(pool, shader);
        } else {
            this->lru_.promote(hash);
            it->second->draw(pool, shader);
        }
    }
}

void Level::updatePlayerPos(const BlockPos &pos) { this->playerPos = pos; }