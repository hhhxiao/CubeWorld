#include "position.h"

ChunkPos BlockPos::toChunkPos() {
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
