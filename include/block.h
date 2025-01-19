#pragma once
#include "position.h"
#include <cassert>
#include <glm/glm.hpp>
#include <vector>
enum BlockType { air, grass, dirt, water, bedrock, oakLog, oakLeaves, invalid };

enum Face { nx = 0, px = 1, ny = 2, py = 3, nz = 4, pz = 5 };

struct BlockFaceInfo {
    Face face;
    BlockType type;
    BlockPos pos;
    BlockFaceInfo() = default;
    BlockFaceInfo(Face f, BlockType t, BlockPos p) : face(f), type(t), pos(p) {}
};

struct Block {
    BlockType type{invalid};
    glm::vec4 color{1.0, 1.0, 0.0, 1.0};
    bool solid{true};
    bool liquid{false};
};

class BlockRegistry {
   public:
    BlockRegistry();

    static BlockRegistry& instance();
    void add(const Block& block);
    inline Block get(BlockType type) {
        auto id = static_cast<int>(type);
        assert(id < blocks.size());
        return blocks_[id];
    }

   private:
    std::vector<Block> blocks_;
};
Block getBlockByType(BlockType type);