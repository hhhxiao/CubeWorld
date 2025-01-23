#include "block.h"

BlockRegistry::BlockRegistry() {
    add({BlockType::air, {0.0, 0.0, 0.0, 0.0}, false});
    add({BlockType::dirt});
    add({BlockType::grass});
    add({BlockType::water, {0.3, 0.57, 0.67, 0.7}, false, true});
    add({BlockType::oakLog});
    add({BlockType::oakLeaves, {0.16, 0.44, 0.27, 1.0}, false});
    add({BlockType::stone});
    add({BlockType::bedrock});
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
