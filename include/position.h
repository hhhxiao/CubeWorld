#ifndef VEC3_H
#define VEC3_H

#include <cstdint>

#include <glm/vec3.hpp>

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
    int x{0};
    int z{0};
    inline uint64_t hash() const {
        return (static_cast<uint64_t>(x) & 0xFFFFFFFFULL) << 32 | (static_cast<uint64_t>(z) & 0xFFFFFFFFULL);
    }

    static ChunkPos fromVec3(const glm::vec3& vec3);
};

#endif
