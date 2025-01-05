#include "chunk_mesh.h"
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <vector>
#include "block.h"
#include "chunk.h"
#include "config.h"
#include "glm/detail/type_vec.hpp"
#include "level_renderer.h"
#include "position.h"
#include "texture.h"
#include "render_context.h"

namespace {
    std::vector<int> createFaceVertices(Face face) {
        if (face == px) return {1, 3, 7, 5};
        if (face == nx) return {2, 0, 4, 6};
        if (face == py) return {4, 5, 7, 6};
        if (face == ny) return {2, 3, 1, 0};
        if (face == pz) return {3, 2, 6, 7};
        if (face == nz) return {0, 1, 5, 4};
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

    // optimize getblock (currently unused)
    BlockType getBlock(const BlockPos& bp, int cx, int y, int cz, LevelChunk* chunk, LevelRenderer* levelRender) {
        if (!chunk) return air;
        if (chunk->posValid(cx, y, cz)) return chunk->getBlock(cx, y, cz);
        return levelRender->getBlock(bp.x + cz, bp.y + y, bp.z + cz);
    }
}  // namespace
void ChunkMesh::genBuffer() {
    vertices_.clear();
    texture_mappings_.clear();
    glGenBuffers(1, &VBO);
    status_ = HashGenBuffer;
}

void ChunkMesh::buildMesh(LevelRenderer* level) {
    auto* chunk = level->getChunkData(this->pos());
    if (!chunk) {
        status_ = HasBufferData;
        return;
    }
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
        auto normal = getFaceNormal(f.face);
        auto pos = f.pos;
        std::vector<V> vertices;
        vertices.reserve(4);
        for (int i = 0; i < vs.size(); i++) {
            auto v = vs[i];
            auto U = 0.0f, V = 0.0f;
            if (i == 2) U = 1.0f;
            if (i == 0) V = 1.0f;
            if (i == 1) U = 1.0f, V = 1.0f;
            auto dx = static_cast<GLfloat>(v % 2);
            auto dy = static_cast<GLfloat>(v >= 4 ? 1 : 0);
            auto dz = static_cast<GLfloat>((v % 4) >= 2 ? 1 : 0);
            auto vert = V::fromPos(glm::vec3(dx + pos.x, dy + pos.y, dz + pos.z));
            vert.setNormal((GLfloat)normal.x, (GLfloat)normal.y, (GLfloat)normal.z);
            vert.setUV(U, V);
            if (f.type == oakLeaves) {
                vert.r = 0.13f;
                vert.g = 0.74f;
                vert.b = 0.0f;
                // #
            } else if (f.type == water) {
                vert.r = 0.6f;
                vert.g = 0.6f;
                vert.b = 1.0f;
                vert.a = 0.6f;
            }
            vertices.push_back(vert);
        }
        auto& array = tmp_mappings[TexturePool::instance().getTextureID(f.type, f.face)];
        array.push_back(vertices[0]);
        array.push_back(vertices[2]);
        array.push_back(vertices[1]);
        array.push_back(vertices[2]);
        array.push_back(vertices[0]);
        array.push_back(vertices[3]);
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
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), static_cast<void*>(0 * sizeof(float)));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), static_cast<void*>(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    // 顶点颜色
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 顶点纹理
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), reinterpret_cast<void*>(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // 顶点法线
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), reinterpret_cast<void*>(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    for (auto& [textureID, vbo] : this->texture_mappings_) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glDrawArrays(GL_TRIANGLES, static_cast<GLuint>(vbo.first), static_cast<GLsizei>(vbo.second));
    }
}
ChunkMesh::~ChunkMesh() {
    if (status_ != Init) glDeleteBuffers(1, &VBO);
}
