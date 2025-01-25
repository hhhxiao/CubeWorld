#include "terrain_generator.h"
#include <algorithm>
#include <cstdlib>
#include "position.h"
#include "single/PerlinNoise.hpp"
#include "block.h"
#include "chunk.h"

void FlatTerrainGenerator::fill(LevelChunk* chunk) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
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
            auto scale = 1.f / 1024;

            auto sea = sea_->octave2D(xx * scale * 2, zz * scale * 2, 5);
            auto plain = plain_->octave2D(xx * scale * 16, zz * scale * 16, 2);
            auto mountain = mountain_->octave2D(xx * scale * 10, zz * scale * 10, 4);
            auto river = river_->octave2D(xx * scale * 5, zz * scale * 4, 5);

            auto sea_height = static_cast<int>(sea * 42) + 20;
            auto plain_height = static_cast<int>(plain * 5);
            auto mountain_height = static_cast<int>(mountain * 60);
            auto river_height = static_cast<int>(river * 10);
            bool is_river = river > 0 && mountain < 0.1 && sea > 0.99;

            if (!is_river) river_height = 0;  // no river in sea and mountain

            auto total_height = static_cast<int>(sea_height + plain_height + mountain_height - river_height);

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
                chunk->getPosition(x, total_height, z)->type = grass;
                chunk->getPosition(x, total_height - 1, z)->type = dirt;
            }
            // fill mountain
            if (mountain >= 0.1) {
                for (int i = total_height; i >= 0; i--) {
                    chunk->getPosition(x, i, z)->type = stone;
                }

                auto about_height = total_height + mountain_height * 1;
                if (about_height > 62 && about_height < 120) {  // grass not in water, if too height , keey stone
                    chunk->getPosition(x, total_height, z)->type = grass;
                    chunk->getPosition(x, total_height - 1, z)->type = dirt;
                }
            }

            if (is_river) {
                chunk->getPosition(x, total_height, z)->type = sand;
            }
            // placeSurface(chunk, xx, zz, total_height);
        }
    }
}

void PerlinTerrainGeneratror::placeSurface(LevelChunk* chunk, int xx, int zz, int height) {
    // tree

    auto scale = 1.f / 1024;
    auto forest = forest_->octave2D(xx * scale * 32, zz * scale * 32, 4);
    if (forest > 0.5) {
        if (!(xx < 2 || xx > 14 || zz < 2 || zz > 14)) {
            for (int i = 1; i <= 4; i++) {
                chunk->getPosition(xx, height + i, zz)->type = oakLog;
            }

            for (int i = -2; i <= 2; i++) {
                for (int j = -2; j <= 2; j++) {
                    if (i == 0 && j == 0) {
                        continue;
                    }
                    chunk->getPosition(xx + i, 5 + height, zz + j)->type = oakLeaves;
                    chunk->getPosition(xx + i, 4 + height, zz + j)->type = oakLeaves;
                    chunk->getPosition(xx + i, 3 + height, zz + j)->type = oakLeaves;
                }
            }
        }
    }
}
