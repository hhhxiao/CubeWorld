#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <random>
#include <utility>
#include <vector>
#include "position.h"
#include "single/PerlinNoise.hpp"
#include "utils.h"
#include <map>

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

class SplineCurves {
    // give x, return
   public:
    SplineCurves(const std::vector<std::pair<float, float>> &kvs) : kvs_(kvs) {
        if (kvs.empty()) {
            //   LE("The spline curuves is empty.");
        }
    }
    inline float get(float x) {
        if (kvs_.empty()) return x;
        auto min = kvs_.begin()->first;
        auto max = kvs_.rbegin()->first;
        if (x <= min) return kvs_.begin()->second;
        if (x >= max) return kvs_.rbegin()->second;
        for (int i = 0; i < kvs_.size() - 1; i++) {
            auto dx = kvs_[i + 1].first - kvs_[i].first;
            auto dy = kvs_[i + 1].second - kvs_[i].second;
            if (kvs_[i].first <= x && x < kvs_[i + 1].first) {
                return kvs_[i].second + dy / dx * (x - kvs_[i].first);
            }
        }
        return max;
    }

   private:
    std::vector<std::pair<float, float>> kvs_;
};

class SplineCurvesNorse {
   public:
    SplineCurvesNorse(siv::PerlinNoise::seed_type seed, const std::vector<std::pair<float, float>> &keys) : sc_(keys) {
        this->noise_ = new siv::PerlinNoise(seed);
    }
    SplineCurvesNorse(siv::PerlinNoise::seed_type seed) : sc_({}) { this->noise_ = new siv::PerlinNoise(seed); }
    float noise(siv::PerlinNoise::value_type x, siv::PerlinNoise::value_type y) {
        return sc_.get((float)noise_->noise2D(x, y));
    }

    ~SplineCurvesNorse() { delete noise_; }
    siv::PerlinNoise *noise_;
    SplineCurves sc_;
};

class PerlinTerrainGeneratror : public AbstractTerrainGenerator {
   public:
    PerlinTerrainGeneratror(siv::PerlinNoise::seed_type seed);

    void fill(LevelChunk *chunk) override;

    void placeTree(LevelChunk *chunk, const BlockPos &pos, int height);

    ~PerlinTerrainGeneratror() override {
        delete perlin_;
        delete continents_;
        delete mountains_;
    }

   private:
    siv::PerlinNoise *perlin_;
    std::mutex generartor_lock_;
    SplineCurvesNorse *continents_{nullptr};
    SplineCurvesNorse *mountains_{nullptr};
    SplineCurvesNorse *plains_{nullptr};
    // random seed
    std::default_random_engine random_engine_;
};
