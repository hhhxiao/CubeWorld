#pragma once
#include "position.h"

enum BlockType { air, grass, dirt, water, bedrock, invalid };

enum Face { nx = 0, px = 1, ny = 2, py = 3, nz = 4, pz = 5 };

struct BlockFaceInfo {
    Face face;
    BlockType type;
    BlockPos pos;
    BlockFaceInfo() = default;
    BlockFaceInfo(Face f, BlockType t, BlockPos p) : face(f), type(t), pos(p) {}
};

class Block {
   public:
   private:
    BlockPos pos{};
};
