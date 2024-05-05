#include "block.h"
#include "chunk.h"

ChunkPos BlockPos::toChunkPos() {
    auto cx = x < 0 ? x - 15 : x;
    auto cz = z < 0 ? z - 15 : z;
    cx /= 16;
    cz /= 16;
    return {cx, cz};
}
