#include "chunk.h"
#include <array>
#include "block.h"
#include "drawable_object.h"
#include "subchunk_mesh.h"

namespace {}  // namespace

// subchunk
SubChunkMesh* SubChunk::createMesh() {
    SubChunkMesh* mesh = new SubChunkMesh();
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {
                auto b = getBlock(x, y, z);
                auto p = BlockPos{x, y, z};
                if (b == air) continue;
                if (!hasBlock(x + 1, y, z)) mesh->AddFace({px, b, p});
                if (!hasBlock(x - 1, y, z)) mesh->AddFace({nx, b, p});
                if (!hasBlock(x, y + 1, z)) mesh->AddFace({py, b, p});
                if (!hasBlock(x, y - 1, z)) mesh->AddFace({ny, b, p});
                if (!hasBlock(x, y, z + 1)) mesh->AddFace({pz, b, p});
                if (!hasBlock(x, y, z - 1)) mesh->AddFace({nz, b, p});
            }
        }
    }
    return mesh;
}

BlockType SubChunk::getBlock(int sx, int sy, int sz) {
    if (sx < 0 || sy < 0 || sz < 0 || sx >= 16 || sy >= 16 || sz >= 16) return air;
    return block_volume_[sx][sy][sz];
}

void SubChunk::setBlock(int sx, int sy, int sz, BlockType type) {
    if (sx < 0 || sy < 0 || sz < 0 || sx >= 16 || sy >= 16 || sz >= 16) return;
    block_volume_[sx][sy][sz] = type;
}

bool SubChunk::hasBlock(int sx, int sy, int sz) {
    if (sx < 0 || sy < 0 || sz < 0 || sx >= 16 || sy >= 16 || sz >= 16) return false;
    return block_volume_[sx][sy][sz] != air;
}

std::array<DrawableObject, 16> Chunk::getMesh() { return std::array<DrawableObject, 16>{}; }

void setBlock(int cx, int y, int cz, BlockType type) {}