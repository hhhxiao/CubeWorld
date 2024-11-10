#ifndef VEC3_H
#define VEC3_H

#include <cstdint>

#include <functional>
#include <glm/vec3.hpp>
#include "parallel_hashmap/phmap_utils.h"

class ChunkPos;
class BlockPos {
   public:
    int x{0};
    int y{0};
    int z{0};
    ChunkPos toChunkPos();
    static BlockPos fromVec3(const glm::vec3& vec3);
};

class ChunkPos {
   public:
    ChunkPos() = default;
    ChunkPos(int vx, int vz) : x(vx), z(vz) {}
    int x{0};
    int z{0};

    friend size_t hash_value(const ChunkPos& p) { return phmap::HashState().combine(0, p.x, p.z); }

    bool operator==(const ChunkPos& p) const { return x == p.x && z == p.z; }

    static ChunkPos fromVec3(const glm::vec3& vec3);
};

namespace std {
    template <>
    struct hash<ChunkPos> {
        auto operator()(const ChunkPos& p) const -> size_t { return hash_value(p); }
    };
}  // namespace std

#endif
