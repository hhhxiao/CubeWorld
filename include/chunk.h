#pragma once

#include <array>
#include "drawable_object.h"
#include "block.h"
#include "subchunk_mesh.h"

struct ChunkPos {
    int x{0};
    int z{0};
};

// 16 * 16 * 256
class SubChunk {
   public:
    SubChunk() {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                for (int k = 0; k < 16; k++) {
                    block_volume_[i][j][k] = air;
                }
            }
        }
    }
    SubChunkMesh* createMesh();

    BlockType getBlock(int sx, int sy, int sz);
    void setBlock(int sx, int sy, int sz, BlockType type);

   private:
    bool hasBlock(int sx, int sy, int sz);

    std::array<std::array<std::array<BlockType, 16>, 16>, 16> block_volume_;
};

class Chunk {
    std::array<DrawableObject, 16> getMesh();
    void setBlock(int cx, int y, int cz, BlockType type);

   private:
    ChunkPos pos{};
    std::array<SubChunk, 16> sub_chunks;
};