#include "chunk.h"
#include <cstddef>
#include "block.h"
#include "config.h"
#include "glm/detail/type_vec.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "simple_shader.h"
#include "subchunk_mesh.h"
#include "terrain_generator.h"

// // subchunk
// SubChunk::SubChunk(const ChunkPos& pos, int sub_chunk_index) : chunk_pos_(pos), sub_chunk_index_(sub_chunk_index) {
//     for (int i = 0; i < 16; i++) {
//         for (int j = 0; j < 16; j++) {
//             for (int k = 0; k < 16; k++) {
//                 block_volume_[i][j][k] = air;
//             }
//         }
//     }
// }

// void SubChunk::createMesh() {
//     this->mesh_ = new SubChunkMesh();
//     for (int x = 0; x < 16; x++) {
//         for (int y = 0; y < 16; y++) {
//             for (int z = 0; z < 16; z++) {
//                 auto b = getBlock(x, y, z);
//                 auto p = BlockPos{x, y, z};
//                 if (b == air) continue;
//                 if (!hasBlock(x + 1, y, z)) mesh_->AddFace({px, b, p});
//                 if (!hasBlock(x - 1, y, z)) mesh_->AddFace({nx, b, p});
//                 if (!hasBlock(x, y + 1, z)) mesh_->AddFace({py, b, p});
//                 if (!hasBlock(x, y - 1, z)) mesh_->AddFace({ny, b, p});
//                 if (!hasBlock(x, y, z + 1)) mesh_->AddFace({pz, b, p});
//                 if (!hasBlock(x, y, z - 1)) mesh_->AddFace({nz, b, p});
//             }
//         }
//     }
//     mesh_->buildData();
// }

// BlockType SubChunk::getBlock(int sx, int sy, int sz) {
//     if (sx < 0 || sy < 0 || sz < 0 || sx >= 16 || sy >= 16 || sz >= 16) return air;
//     return block_volume_[sx][sy][sz];
// }

// void SubChunk::setBlock(int sx, int sy, int sz, BlockType type) {
//     if (sx < 0 || sy < 0 || sz < 0 || sx >= 16 || sy >= 16 || sz >= 16) return;
//     block_volume_[sx][sy][sz] = type;
// }

// void SubChunk::draw(Shader* shader) {
//     if (this->mesh_) {
//         auto model = glm::translate(
//             glm::mat4(1.0), glm::vec3(this->chunk_pos_.x * 16, this->sub_chunk_index_ * 16, this->chunk_pos_.z *
//             16));
//         shader->setMat4("model", model);
//         this->mesh_->draw();
//     }
// }

// bool SubChunk::hasBlock(int sx, int sy, int sz) {
//     if (sx < 0 || sy < 0 || sz < 0 || sx >= 16 || sy >= 16 || sz >= 16) return false;
//     return block_volume_[sx][sy][sz] != air;
// }

// void SubChunk::trySendData() {
//     if (has_send_data_) return;
//     has_send_data_ = true;
//     if (this->mesh_) this->mesh_->sendData();
// }

// Chunk::Chunk(const ChunkPos& pos, AbstractTerrainGenerator* generator) {
//     this->pos_ = pos;
//     for (int i = 0; i < 16; i++) {
//         this->sub_chunks_[i] = new SubChunk(this->pos_, i);
//     }
//     this->init(generator);
// }

// Chunk::~Chunk() {
//     for (auto sub : this->sub_chunks_) {
//         delete sub;
//     }
// }

// void Chunk::setBlock(int cx, int y, int cz, BlockType type) {
//     this->sub_chunks_[y / 16]->setBlock(cx, y % 16, cz, type);
// }

// void Chunk::init(AbstractTerrainGenerator* generator) {
//     if (generator) {
//         generator->fill(this);
//     }
//     for (auto& sub : this->sub_chunks_) {
//         sub->createMesh();
//     }
// }

// void Chunk::draw(Shader* shader) {
//     for (int i = 0; i < 16; i++) {
//         if (this->sub_chunks_[i]) this->sub_chunks_[i]->draw(shader);
//     }
// }

// void Chunk::trySendData() {
//     for (int i = 0; i < 16; i++) {
//         if (this->sub_chunks_[i]) this->sub_chunks_[i]->trySendData();
//     }
// }

// new

LevelChunk::LevelChunk(const ChunkPos& pos) {}
void LevelChunk::setBlock(int cx, int y, int cz, BlockType type) {
    if (!posValid(cx, y, cz)) return;
    this->data_[y][cx][cz] = type;
}

void LevelChunk::tick(tick_t ts) {
    this->tick_ = ts;
    // Noting todo
}
BlockType LevelChunk::getBlock(int cx, int y, int cz) {
    if (!posValid(cx, y, cz)) return BlockType::air;
    return this->data_[y][cx][cz];
}
bool LevelChunk::posValid(int cx, int y, int cz) {
    return cx >= 0 && cx < 16 && cz >= 0 && cz < 16 && y >= 0 && y < Config::CHUNK_HEIGHT;
}