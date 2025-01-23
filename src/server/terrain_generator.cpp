#include "terrain_generator.h"
#include "position.h"
#include "single/PerlinNoise.hpp"
#include "block.h"
#include "chunk.h"

void FlatTerrainGenerator::fill(LevelChunk* chunk) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            chunk->setBlock(x, 0, z, bedrock);
            chunk->setBlock(x, 1, z, dirt);
            chunk->setBlock(x, 2, z, dirt);
            chunk->setBlock(x, 3, z, dirt);
            chunk->setBlock(x, 4, z, dirt);
            chunk->setBlock(x, 5, z, dirt);
            chunk->setBlock(x, 6, z, grass);
            for (int y = 7; y < Config::CHUNK_HEIGHT; y++) {
                chunk->setBlock(x, y, z, air);
            }
        }
    }
}

PerlinTerrainGeneratror::PerlinTerrainGeneratror(siv::PerlinNoise::seed_type seed) {
    this->perlin_ = new siv::PerlinNoise(seed);
    this->continents_ = new SplineCurvesNorse(seed, {{-1.0, -1.0}, {0.5, 0.6}, {1.0, 1.0}});
    this->mountains_ = new SplineCurvesNorse(seed + 1);
    this->plains_ = new SplineCurvesNorse(seed + 2, {{-1.0, -1.0}, {-0.05, -0.99}, {0.05, 0.99}, {1.0, 1.0}});
}

void PerlinTerrainGeneratror::fill(LevelChunk* chunk) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            const auto xx = chunk->pos().x * 16 + x;
            const auto zz = chunk->pos().z * 16 + z;
            //   generartor_lock_.lock();
            auto scale = 0.001;
            auto continent = this->continents_->noise(xx * scale * 4, zz * scale * 4);
            auto mountain = this->mountains_->noise(xx * scale * 20, zz * scale * 20);
            auto plain = this->plains_->noise(xx * scale, zz * scale);
            auto height = static_cast<int>((continent * 0.1 + mountain * 0.9) * 80 + 64);
            for (int i = 64; i >= height; i--) {
                chunk->setBlock(x, i - 1, z, water);
            }
            for (int i = height - 1; i >= 0; i--) {
                chunk->setBlock(x, i, z, stone);
            }
        }
    }

    // auto hasher = std::hash<ChunkPos>();
    // random_engine_.seed(static_cast<unsigned int>(hasher(chunk->pos())));
    // // tree
    // int try_place_tree = random_engine_() % 10;
    // for (int i = 0; i < try_place_tree; i++) {
    //     int dx = random_engine_() % 16;
    //     int dz = random_engine_() % 16;
    //     auto y = chunk->topY(dx, dz);
    //     if (chunk->getBlock(dx, y, dz) == grass) {
    //         this->placeTree(chunk, {dx, y, dz}, random_engine_() % 3 + 4);
    //     }
    // }
}

void PerlinTerrainGeneratror::placeTree(LevelChunk* chunk, const BlockPos& pos, int height) {
    if (pos.x < 2 || pos.x > 14 || pos.z < 2 || pos.z > 14) {
        return;
    }

    for (int i = 1; i <= height; i++) {
        chunk->setBlock(pos.x, pos.y + i, pos.z, oakLog);
    }

    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            chunk->setBlock(pos.x + i, pos.y + height, pos.z + j, oakLeaves);
            chunk->setBlock(pos.x + i, pos.y + height - 1, pos.z + j, oakLeaves);
        }
    }

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            chunk->setBlock(pos.x + i, pos.y + height + 1, pos.z + j, oakLeaves);
        }
    }
}
