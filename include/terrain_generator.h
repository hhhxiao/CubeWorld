#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <random>
#include <utility>
#include <vector>
#include "position.h"
#include "single/PerlinNoise.hpp"
#include "utils.h"
#include <map>

using value_type = siv::PerlinNoise::value_type;
using seed_type = siv::PerlinNoise::seed_type;
using kv_list = std::vector<std::pair<value_type, value_type>>;

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
    SplineCurves(const kv_list &kvs) : kvs_(kvs) {
        if (kvs.empty()) {
            //   LE("The spline curuves is empty.");
        }
    }
    inline value_type get(value_type x) {
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
    kv_list kvs_;
};

class SplineCurvesNorse {
   public:
    SplineCurvesNorse(seed_type seed, const kv_list &keys) : sc_(keys) { this->noise_ = new siv::PerlinNoise(seed); }
    SplineCurvesNorse(seed_type seed) : sc_({}) { this->noise_ = new siv::PerlinNoise(seed); }
    value_type noise2D(value_type x, value_type y) { return sc_.get(noise_->noise2D(x, y)); }

    value_type octave2D(value_type x, value_type y, const std::int32_t octave, value_type persistence = 0.5) {
        return sc_.get(noise_->octave2D(x, y, octave, persistence));
    }
    value_type octave2D_01(value_type x, value_type y, const std::int32_t octave, value_type persistence = 0.5) {
        return sc_.get(noise_->octave2D_01(x, y, octave, persistence));
    }

    ~SplineCurvesNorse() { delete noise_; }
    siv::PerlinNoise *noise_;
    SplineCurves sc_;
};

class PerlinTerrainGeneratror : public AbstractTerrainGenerator {
    static constexpr auto SCALE = 1.0 / 1024;

   public:
    PerlinTerrainGeneratror(seed_type seed);

    void fill(LevelChunk *chunk) override;

    // in chunk pos
    void placeSurface(LevelChunk *chunk, int x, int z, int height);

    // help functions
    void placeTree(LevelChunk *chunk, int x, int z, int height);

    ~PerlinTerrainGeneratror() override {
        delete mountain_;
        delete plain_;
        delete sea_;
        delete river_;
        delete forest_;
    }

   private:
    siv::PerlinNoise *perlin_;
    SplineCurvesNorse *sea_;
    SplineCurvesNorse *mountain_;
    SplineCurvesNorse *plain_;
    SplineCurvesNorse *river_;
    SplineCurvesNorse *forest_;
    std::default_random_engine random_engine_;
};
