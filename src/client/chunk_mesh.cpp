#include "chunk_mesh.h"
#include <wtypes.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include "block.h"
#include "client_level.h"
#include "config.h"
#include "glm/detail/type_vec.hpp"
#include "position.h"
#include "texture.h"
#include "render_context.h"
#include <unordered_set>

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
void ChunkMesh::init() {
    tick_ = 0;
    glGenBuffers(1, &VBO);
}

void ChunkMesh::buildMesh(ClientLevel* level) {
    vertices_.clear();
    texture_mappings_.clear();
    neighbor_mask_ = level->neighborMask(this->pos_);
    std::vector<BlockFaceInfo> fis;
    auto bp = this->pos_.toBlockPos();
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < Config::CHUNK_HEIGHT; y++) {
            for (int z = 0; z < 16; z++) {
                auto p = BlockPos{x + bp.x, y + bp.y, z + bp.z};
                auto b = level->getBlock(p.x, p.y, p.z, pos_);
                if (b == air) continue;
                if (level->getBlock(p.x + 1, p.y, p.z, pos_) == air) fis.emplace_back(px, b, p);
                if (level->getBlock(p.x - 1, p.y, p.z, pos_) == air) fis.emplace_back(nx, b, p);
                if (level->getBlock(p.x, p.y + 1, p.z, pos_) == air) fis.emplace_back(py, b, p);
                if (level->getBlock(p.x, p.y - 1, p.z, pos_) == air) fis.emplace_back(ny, b, p);
                if (level->getBlock(p.x, p.y, p.z + 1, pos_) == air) fis.emplace_back(pz, b, p);
                if (level->getBlock(p.x, p.y, p.z - 1, pos_) == air) fis.emplace_back(nz, b, p);
            }
        }
    }

    std::unordered_map<GLuint, std::vector<ChunkMesh::V>> tmp_mappings;
    for (auto f : fis) {
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
    has_build_ = true;
    has_buffer_data_ = false;
}

void ChunkMesh::render(RenderContext& ctx) {
    tick_ = 0;
    if (!has_build_) return;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (!has_buffer_data_) {
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(vertices_.size() * sizeof(ChunkMesh::V)), vertices_.data(),
                     GL_STATIC_DRAW);
        // 顶点位置
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), static_cast<void*>(0 * sizeof(float)));
        glEnableVertexAttribArray(0);
        // 顶点颜色
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V),
                              reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // 顶点纹理
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V),
                              reinterpret_cast<void*>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        // 顶点法线
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V),
                              reinterpret_cast<void*>(8 * sizeof(float)));
        glEnableVertexAttribArray(3);
        has_build_ = true;
    }
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMesh::V), static_cast<void*>(0 * sizeof(float)));
    for (auto& [textureID, vbo] : this->texture_mappings_) {
        // glBindTexture(GL_TEXTURE_2D, textureID);
        glDrawArrays(GL_TRIANGLES, static_cast<GLuint>(vbo.first), static_cast<GLsizei>(vbo.second));
    }
}

ChunkMesh::~ChunkMesh() { glDeleteBuffers(1, &VBO); }

void ChunkMeshMgr::update(RenderContext& ctx, ClientLevel* level) {
    if (!task_queue_.empty()) return;

    const auto R = Config::view_distance;
    const auto cp = BlockPos::fromVec3(ctx.camera().position_).toChunkPos();

    std::unordered_map<ChunkPos, ChunkMesh*> meshes_to_refresh;
    // remove old meshes
    for (auto it = meshes_.cbegin(); it != meshes_.cend();) {
        if (it->second->isDead() && it->second->has_buffer_data()) {
            LI("Remove mesh: %s", it->first.toString().c_str());
            delete it->second;
            it = meshes_.erase(it);
        } else {
            ++it;
        }
    }
    for (auto& [p, mesh] : meshes_) {
        if (mesh->has_build() && mesh->neighbor_mask() != level->neighborMask(p)) {
            meshes_to_refresh[p] = mesh;
        }
    }
    // update chunks
    std::unordered_set<ChunkPos> actives;
    for (auto i = -R; i <= R; i++) {
        for (auto j = -R; j <= R; j++) {
            auto pos = ChunkPos(cp.x + i, cp.z + j);
            actives.insert(pos);  //
            if (!this->meshes_.count(pos) && level->hasChunk(pos)) {
                auto* mesh = new ChunkMesh(pos);
                mesh->init();
                this->meshes_[pos] = mesh;
                meshes_to_refresh[pos] = mesh;
            }
        }
    }

    for (auto& kv : meshes_to_refresh) {
        auto p = kv.first;
        auto* mesh = kv.second;
        if (!task_queue_.contains(p)) {
            task_queue_.insert(p);
            this->pool_->enqueue([this, mesh, level, p]() {
                mesh->buildMesh(level);
                task_queue_.erase(p);
            });
        }
    }
}

void ChunkMeshMgr::foreachRenderChunk(RenderContext& ctx, const std::function<void(const ChunkPos&)>& f) {
    const auto R = Config::view_distance;
    const auto cp = BlockPos::fromVec3(ctx.camera().position_).toChunkPos();
    for (auto i = -R; i <= R; i++) {
        for (auto j = -R; j <= R; j++) {
            auto pos = ChunkPos(cp.x + i, cp.z + j);
            f(pos);
        }
    }
}

void ChunkMeshMgr::render(RenderContext& ctx) {
    for (auto& [pos, mesh] : meshes_) {
        mesh->tick_++;
    }
    this->foreachRenderChunk(ctx, [this, &ctx](const ChunkPos& pos) {
        auto it = meshes_.find(pos);
        if (it != meshes_.end() && !task_queue_.contains(pos)) {
            it->second->render(ctx);
            it->second->tick_ = 0;
        }
    });
}