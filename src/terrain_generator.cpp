#include "terrain_generator.h"
#include "block.h"
#include "chunk.h"
void FlatTerrainGenerator::fill(Chunk *chunk) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            chunk->setBlock(x, 0, z, bedrock);
            chunk->setBlock(x, 1, z, dirt);
            chunk->setBlock(x, 2, z, dirt);
            chunk->setBlock(x, 3, z, dirt);
            chunk->setBlock(x, 4, z, dirt);
            chunk->setBlock(x, 5, z, dirt);
            chunk->setBlock(x, 6, z, grass);
        }
    }
}
