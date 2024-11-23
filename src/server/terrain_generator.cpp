#include "terrain_generator.h"
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
}

void PerlinTerrainGeneratror::fill(LevelChunk* chunk) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            const auto xx = chunk->pos().x * 16 + x;
            const auto zz = chunk->pos().z * 16 + z;
            generartor_lock_.lock();
            const auto height = static_cast<int>(this->perlin_->octave2D_11(xx * 0.03, zz * 0.03, 4) * 16) + 64;
            // auto height = 65;
            generartor_lock_.unlock();
            chunk->setBlock(x, height, z, grass);
            for (int i = 64; i >= height; i--) {
                chunk->setBlock(x, i, z, water);
            }
            for (int i = height - 1; i >= 0; i--) {
                chunk->setBlock(x, i, z, dirt);
            }
        }
    }
}