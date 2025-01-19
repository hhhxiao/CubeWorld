#include "block.h"

BlockRegistry::BlockRegistry() {
    add({BlockType::air, {0.0, 0.0, 0.0, 0.0}, false});
    add({BlockType::dirt, {0.74, 0.5, 0.36, 1.0}});
    add({BlockType::grass, {0.65, 0.72, 0.34, 1.0}});
    add({BlockType::water, {0.3, 0.57, 0.67, 0.7}, false, true});
    add({BlockType::oakLog, {0.6, 0.375, 0.14, 1.0}});
    add({BlockType::oakLeaves, {0.16, 0.44, 0.27, 1.0}, false});
    add({BlockType::bedrock, {0.16, 0.15, 0.17, 1.0}});
}

BlockRegistry& BlockRegistry::instance() {
    static BlockRegistry br;
    return br;
}
void BlockRegistry::add(const Block& block) {
    auto id = static_cast<int>(block.type);
    if (id >= blocks_.size()) blocks_.resize(id + 1);
    blocks_[id] = block;
}
