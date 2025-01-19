#include "position.h"
#include <cmath>

ChunkPos BlockPos::toChunkPos() const {
    auto cx = x < 0 ? x - 15 : x;
    auto cz = z < 0 ? z - 15 : z;
    cx /= 16;
    cz /= 16;
    return {cx, cz};
}

BlockPos BlockPos::fromVec3(const glm::vec3& vec3) {
    return {
        static_cast<int>(vec3.x),
        static_cast<int>(vec3.y),
        static_cast<int>(vec3.z),
    };
}

ChunkPos ChunkPos::fromVec3(const glm::vec3& vec3) { return BlockPos::fromVec3(vec3).toChunkPos(); }

BlockPos ChunkPos::toBlockPos() const { return {x << 4, 0, z << 4}; }

int ChunkPos::dis2(const ChunkPos& pos) const {
    auto dx = abs(x - pos.x);
    auto dz = abs(z - pos.z);
    return dx * dx + dz * dz;
}

double ChunkPos::dis(const ChunkPos& pos) const { return ::sqrt(dis2(pos)); }
