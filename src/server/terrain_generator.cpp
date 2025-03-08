#include "terrain_generator.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include "config.h"
#include "position.h"
#include "single/PerlinNoise.hpp"
#include "block.h"
#include "chunk.h"

void FlatTerrainGenerator::fill(LevelChunk* chunk) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < Config::CHUNK_HEIGHT; y++) {
                auto is_stone = (y <= 4 || (x == 8 && y == 8 && z == 8)) && chunk->pos() == ChunkPos{0, 0};
                chunk->getPosition(x, y, z)->type = is_stone ? stone : air;
            }
        }
    }
}

PerlinTerrainGeneratror::PerlinTerrainGeneratror(siv::PerlinNoise::seed_type seed) {
    this->sea_ =
        new SplineCurvesNorse(seed, {{-1.0, 0.1}, {-0.4, 0.15}, {-0.1, 0.2}, {0.1, 0.8}, {0.2, 1.0}, {1.0, 1.0}});
    this->plain_ = new SplineCurvesNorse(seed);
    this->forest_ = new SplineCurvesNorse(seed + 1, {{-1.0, 0}, {1.0, 1.0}});
    this->mountain_ =
        new SplineCurvesNorse(seed, {{-1.0, 0.5}, {-0.3, 0.02}, {0, 0.05}, {0.4, 0.03}, {0.7, 0.4}, {1.0, 1.0}});
    this->river_ = new SplineCurvesNorse(
        seed, {{-1.0, 0.0}, {-0.08, 0.0}, {-0.02, 0.9}, {0, 1.0}, {0.02, 0.9}, {0.08, 0.0}, {1.0, 0.0}});
}

void PerlinTerrainGeneratror::fill(LevelChunk* chunk) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            const auto xx = chunk->pos().x * 16 + x;
            const auto zz = chunk->pos().z * 16 + z;
            auto sea = sea_->octave2D(xx * SCALE * 2, zz * SCALE * 2, 5);
            auto plain = plain_->octave2D(xx * SCALE * 16, zz * SCALE * 16, 2);
            auto mountain = mountain_->octave2D(xx * SCALE * 10, zz * SCALE * 10, 4);
            auto river = river_->octave2D(xx * SCALE * 5, zz * SCALE * 4, 5);

            auto sea_height = static_cast<int>(sea * 42) + 20;
            auto plain_height = static_cast<int>(plain * 5);
            auto mountain_height = static_cast<int>(mountain * 60);

            auto total_height = static_cast<int>(sea_height + plain_height + mountain_height);

            for (int i = 0; i <= total_height; i++) {
                chunk->getPosition(x, i, z)->type = stone;
            }
            // water
            for (int i = total_height + 1; i < 62; i++) {
                chunk->getPosition(x, i, z)->type = water;
            }
            if ((sea >= 0.8 && sea < 1.0) || river > 0.1) {
                chunk->getPosition(x, total_height, z)->type = sand;
            }
            if (sea >= 1.0) {
                chunk->getPosition(x, total_height, z)->type = total_height > 60 ? grass : dirt;
                chunk->getPosition(x, total_height - 1, z)->type = dirt;
            }
            // fill mountain
            if (mountain >= 0.1) {
                for (int i = total_height; i >= 0; i--) {
                    chunk->getPosition(x, i, z)->type = stone;
                }

                auto about_height = total_height + mountain_height * 1;
                if (about_height > 62 && about_height < 120) {  // grass not in water, if too height , keey stone
                    chunk->getPosition(x, total_height, z)->type = total_height > 60 ? grass : dirt;
                    chunk->getPosition(x, total_height - 1, z)->type = dirt;
                }
            }

            placeSurface(chunk, x, z, total_height);
        }
    }
}

void PerlinTerrainGeneratror::placeSurface(LevelChunk* chunk, int x, int z, int height) {
    auto pos = chunk->pos().toBlockPos() + BlockPos(x, z, height);
    random_engine_.seed(static_cast<uint32_t>(std::hash<BlockPos>()(pos)));
    const auto xx = chunk->pos().x * 16 + x;
    const auto zz = chunk->pos().z * 16 + z;
    auto forest = forest_->octave2D(xx * SCALE * 5, zz * SCALE * 4, 5);
    if (forest > 0.8 || forest <= 0.2) {
        // foest
        if (random_engine_() % 16 == 0) placeTree(chunk, x, z, height);
    } else {
        if (random_engine_() % 1000 == 0) placeTree(chunk, x, z, height);
    }
}

void PerlinTerrainGeneratror::placeTree(LevelChunk* chunk, int x, int z, int height) {
    if (chunk->getBlock(x, height, z) != grass) return;
    if (x <= 1 || x >= 14 || z <= 1 || z >= 14) return;
    for (int i = 1; i <= 4; i++) {
        chunk->getPosition(x, height + i, z)->type = oakLog;
    }

    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            if (abs(i) != 2 || abs(j) != 2) chunk->getPosition(x + i, 5 + height, z + j)->type = oakLeaves;
            chunk->getPosition(x + i, 4 + height, z + j)->type = oakLeaves;
            chunk->getPosition(x + i, 3 + height, z + j)->type = oakLeaves;
        }
    }
}