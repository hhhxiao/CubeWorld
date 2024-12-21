#include "chunk_mesh.h"
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <vector>
#include "block.h"
#include "config.h"
#include "glm/detail/type_vec.hpp"
#include "level_renderer.h"
#include "position.h"
#include "texture.h"
#include "render_context.h"

namespace {
    std::vector<int> createFaceVertices(Face face) {
        if (face == px) return {1, 3, 5, 7};
        if (face == nx) return {0, 2, 4, 6};
        if (face == py) return {4, 5, 6, 7};
        if (face == ny) return {0, 1, 2, 3};
        if (face == pz) return {2, 3, 6, 7};
        if (face == nz) return {0, 1, 4, 5};
        throw std::exception("error in create Face indices");
    }

    BlockPos getFaceNormal(const Face face) {
        if (face == px) return {1, 0, 0};
        if (face == nx) return {-1, 0, 0};
        if (face == py) return {0, 1, 0};
        if (face == ny) return {0, -1, 0};
        if (face == pz) return {0, 0, 1};
        if (face == nz) return {0, 0, -1};
        throw std::exception("error in create Face indices");
    }
}  // namespace
void ChunkMesh::genBuffer() {
    vertices_.clear();
    texture_mappings_.clear();
    glGenBuffers(1, &VBO);
    status_ = HashGenBuffer;
}

void ChunkMesh::buildMesh(LevelRenderer* level) {
    auto bp = this->pos_.toBlockPos();
    if (status_ == HashGenBuffer) {
        int by = current_build_height_;
        current_build_height_ = std::min(current_build_height_ + ChunkMesh::GRANULARYTY, (int)Config::CHUNK_HEIGHT);
        for (int x = 0; x < 16; x++) {
            for (int y = by; y < current_build_height_; y++) {
                for (int z = 0; z < 16; z++) {
                    auto p = BlockPos{x + bp.x, y + bp.y, z + bp.z};
                    auto b = level->getBlock(p.x, p.y, p.z);
                    if (b == air) continue;
                    if (level->getBlock(p.x + 1, p.y, p.z) == air) fis_.emplace_back(px, b, p);
                    if (level->getBlock(p.x - 1, p.y, p.z) == air) fis_.emplace_back(nx, b, p);
                    if (level->getBlock(p.x, p.y + 1, p.z) == air) fis_.emplace_back(py, b, p);
                    if (level->getBlock(p.x, p.y - 1, p.z) == air) fis_.emplace_back(ny, b, p);
                    if (level->getBlock(p.x, p.y, p.z + 1) == air) fis_.emplace_back(pz, b, p);
                    if (level->getBlock(p.x, p.y, p.z - 1) == air) fis_.emplace_back(nz, b, p);
                }
            }
        }
    }
    if (current_build_height_ >= Config::CHUNK_HEIGHT) status_ = HasBuildMesh;
}

void ChunkMesh::bufferData() {
    std::unordered_map<GLuint, std::vector<ChunkMesh::V>> tmp_mappings;
    for (auto f : fis_) {
        auto vs = createFaceVertices(f.face);
        auto pos = f.pos;
        std::vector<glm::vec3> poss;
        for (auto v : vs) {
            auto dx = static_cast<GLfloat>(v % 2);
            auto dy = static_cast<GLfloat>(v >= 4 ? 1 : 0);
            auto dz = static_cast<GLfloat>((v % 4) >= 2 ? 1 : 0);
            poss.emplace_back(dx + pos.x, dy + pos.y, dz + pos.z);
        }
        auto& array = tmp_mappings[TexturePool::instance().getTextureID(f.type, f.face)];
        array.push_back(V::fromPos(poss[0]));
        array.push_back(V::fromPos(poss[1]));
        array.push_back(V::fromPos(poss[2]));
        array.push_back(V::fromPos(poss[1]));
        array.push_back(V::fromPos(poss[2]));
        array.push_back(V::fromPos(poss[3]));
    }

    // build verticles
    for (auto& kv : tmp_mappings) {
        auto begin = vertices_.size();
        auto cnt = kv.second.size();
        vertices_.insert(vertices_.end(), kv.second.begin(), kv.second.end());
        this->texture_mappings_[kv.first] = std::make_pair(begin, cnt);
    }
    // buffer data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(vertices_.size() * sizeof(ChunkMesh::V)), vertices_.data(),
                 GL_STATIC_DRAW);
    // 顶点位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), static_cast<void*>(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    // 顶点颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 顶点纹理
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // 顶点法线
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), reinterpret_cast<void*>(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    status_ = HasBufferData;
}

bool ChunkMesh::stepInit(LevelRenderer* level) {
    if (status_ == Init) {
        genBuffer();
        return false;
    }
    if (status_ == HashGenBuffer) {
        buildMesh(level);
        return false;
    }
    if (status_ == HasBuildMesh) {
        bufferData();
    }
    return true;
}

void ChunkMesh::render(RenderContext& ctx) {
    if (status_ < HasBufferData) return;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), static_cast<void*>(0 * sizeof(float)));
    for (auto& [textureID, vbo] : this->texture_mappings_) {
        // glBindTexture(GL_TEXTURE_2D, textureID);
        glDrawArrays(GL_TRIANGLES, static_cast<GLuint>(vbo.first), static_cast<GLsizei>(vbo.second));
    }
}
ChunkMesh::~ChunkMesh() {
    if (status_ != Init) glDeleteBuffers(1, &VBO);
}
