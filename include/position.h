#ifndef VEC3_H
#define VEC3_H

#include <functional>
#include <glm/vec3.hpp>
#include <string>
#include "parallel_hashmap/phmap_utils.h"

class ChunkPos;
class BlockPos {
   public:
    int x{0};
    int y{0};
    int z{0};
    ChunkPos toChunkPos() const;
    static BlockPos fromVec3(const glm::vec3& vec3);
    friend size_t hash_value(const BlockPos& p) { return phmap::HashState().combine(0, p.x, p.y, p.z); }
    [[nodiscard]] std::string toString() const {
        return "[" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "]";
    }
};

class ChunkPos {
   public:
    ChunkPos() = default;
    ChunkPos(int vx, int vz) : x(vx), z(vz) {}
    int x{0};
    int z{0};

    friend size_t hash_value(const ChunkPos& p) { return phmap::HashState().combine(0, p.x, p.z); }

    [[nodiscard]] std::string toString() const { return "[" + std::to_string(x) + ", " + std::to_string(z) + "]"; }

    [[nodiscard]] BlockPos toBlockPos() const;

    int dis2(const ChunkPos& pos) const;
    double dis(const ChunkPos& pos) const;

    bool operator==(const ChunkPos& p) const { return x == p.x && z == p.z; }

    bool operator!=(const ChunkPos& p) const { return !(x == p.x && z == p.z); }

    ChunkPos operator+(const ChunkPos& p) const { return {x + p.x, z + p.z}; }

    ChunkPos operator-(const ChunkPos& p) const { return {x - p.x, z - p.z}; }

    static ChunkPos fromVec3(const glm::vec3& vec3);
};

namespace std {
    template <>
    struct hash<ChunkPos> {
        auto operator()(const ChunkPos& p) const -> size_t { return hash_value(p); }
    };
    template <>
    struct hash<BlockPos> {
        auto operator()(const BlockPos& p) const -> size_t { return hash_value(p); }
    };
}  // namespace std

#endif
