#pragma once
#include <mutex>
#include <random>
#include "position.h"
#include "single/PerlinNoise.hpp"
class LevelChunk;
class AbstractTerrainGenerator {
   public:
    virtual void fill(LevelChunk *chunk) = 0;
    virtual ~AbstractTerrainGenerator() = default;
};

class FlatTerrainGenerator : public AbstractTerrainGenerator {
   public:
    void fill(LevelChunk *chunk) override;
    ~FlatTerrainGenerator() override = default;
};

class PerlinTerrainGeneratror : public AbstractTerrainGenerator {
   public:
    PerlinTerrainGeneratror(siv::PerlinNoise::seed_type seed);

    void fill(LevelChunk *chunk) override;

    void placeTree(LevelChunk *chunk, const BlockPos &pos, int height);

    ~PerlinTerrainGeneratror() override = default;

   private:
    siv::PerlinNoise *perlin_;
    std::mutex generartor_lock_;
    // random seed
    std::default_random_engine random_engine_;
};
