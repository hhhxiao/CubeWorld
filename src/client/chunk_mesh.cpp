#include "chunk_mesh.h"
#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>
#include "block.h"
#include "buffer.h"
#include "chunk.h"
#include "config.h"
#include "glm/detail/type_vec.hpp"
#include "level_renderer.h"
#include "position.h"
#include "texture.h"
#include "render_context.h"
#include "level_renderer.h"
#include "client_level.h"
#include "Remotery.h"
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
    BlockType getBlock(const BlockPos& bp, LevelChunk* chunk, LevelRenderer* levelRender) {
        auto chunkPos = bp.toChunkPos();
        if (chunkPos == chunk->pos()) {
            auto cx = bp.x - chunkPos.x * 16;
            auto cz = bp.z - chunkPos.z * 16;
            return chunk ? chunk->getBlock(cx, bp.y, cz) : air;
        }
        return levelRender->getBlock(bp.x, bp.y, bp.z);
    }

}  // namespace
void ChunkMeshOld::genBuffer() {
    vertices_.clear();
    texture_mappings_.clear();
    glGenBuffers(1, &VBO);
    status_ = HashGenBuffer;
}

void ChunkMeshOld::buildMesh(LevelRenderer* level) {
    auto* chunk = level->getChunkData(this->pos());
    if (!chunk) {
        status_ = HasBufferData;
        return;
    }
    auto bp = this->pos_.toBlockPos();
    if (status_ == HashGenBuffer) {
        int by = current_build_height_;
        current_build_height_ = std::min(current_build_height_ + ChunkMeshOld::GRANULARYTY, (int)Config::CHUNK_HEIGHT);
        for (int x = 0; x < 16; x++) {
            for (int y = by; y < current_build_height_; y++) {
                for (int z = 0; z < 16; z++) {
                    auto p = BlockPos{x + bp.x, y + bp.y, z + bp.z};
                    auto b = level->getBlock(p.x, p.y, p.z);
                    if (b == air) continue;
                    if (getBlock({p.x + 1, p.y, p.z}, chunk, level) == air) fis_.emplace_back(px, b, p);
                    if (getBlock({p.x - 1, p.y, p.z}, chunk, level) == air) fis_.emplace_back(nx, b, p);
                    if (getBlock({p.x, p.y + 1, p.z}, chunk, level) == air) fis_.emplace_back(py, b, p);
                    if (getBlock({p.x, p.y - 1, p.z}, chunk, level) == air) fis_.emplace_back(ny, b, p);
                    if (getBlock({p.x, p.y, p.z + 1}, chunk, level) == air) fis_.emplace_back(pz, b, p);
                    if (getBlock({p.x, p.y, p.z - 1}, chunk, level) == air) fis_.emplace_back(nz, b, p);
                }
            }
        }
    }
    if (current_build_height_ >= Config::CHUNK_HEIGHT) status_ = HasBuildMesh;
}

void ChunkMeshOld::bufferData() {
    std::unordered_map<GLuint, std::vector<ChunkMeshOld::V>> tmp_mappings;
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
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(vertices_.size() * sizeof(ChunkMeshOld::V)), vertices_.data(),
                 GL_STATIC_DRAW);
    status_ = HasBufferData;
}

bool ChunkMeshOld::stepInit(LevelRenderer* level) {
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

void ChunkMeshOld::render(RenderContext& ctx) {
    if (status_ < HasBufferData) return;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMeshOld::V), static_cast<void*>(0 * sizeof(float)));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMeshOld::V),
                          reinterpret_cast<void*>(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    // 顶点颜色
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ChunkMeshOld::V),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 顶点纹理
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkMeshOld::V),
                          reinterpret_cast<void*>(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // 顶点法线
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkMeshOld::V),
                          reinterpret_cast<void*>(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    for (auto& [textureID, vbo] : this->texture_mappings_) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glDrawArrays(GL_TRIANGLES, static_cast<GLuint>(vbo.first), static_cast<GLsizei>(vbo.second));
    }
}
ChunkMeshOld::~ChunkMeshOld() {
    if (status_ != Init) glDeleteBuffers(1, &VBO);
}

ChunkMesh::ChunkMesh(const ChunkPos& pos) : pos_(pos) {}

void ChunkMesh::freeBuckect(ChunkRenderer& chunk_renderer) {
    for (auto& bi : buckets_) {
        chunk_renderer.buffer().reclaim(pos_, bi.index);
    }
}
void ChunkMesh::build(ChunkRenderer& renderer) {
    // reclaim old
    freeBuckect(renderer);
    // fetch neghbor chunks;
    auto* cpx = renderer.getChunk(pos_ + ChunkPos{1, 0});
    auto* cnx = renderer.getChunk(pos_ - ChunkPos{1, 0});
    auto* cpz = renderer.getChunk(pos_ + ChunkPos{0, 1});
    auto* cnz = renderer.getChunk(pos_ + ChunkPos{0, -1});
    auto* self = renderer.getChunk(pos_);
    if (!self) {
        LE("Error in building chunk mesh: self nullptr");
        return;
    }
    static int dx[] = {1, -1, 0, 0, 0, 0};
    static int dy[] = {0, 0, 1, -1, 0, 0};
    static int dz[] = {0, 0, 0, 0, 1, -1};
    static Face faces[] = {px, nx, py, ny, pz, nz};
    LevelChunk* target{nullptr};
    auto obp = pos_.toBlockPos();
    for (int y = 0; y < Config::CHUNK_HEIGHT; y++) {
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                auto cur = self->getBlock(x, y, z);
                if (cur == air) continue;
                for (int i = 0; i < 6; i++) {
                    target = self;
                    auto px = dx[i] + x;
                    auto py = dy[i] + y;
                    auto pz = dz[i] + z;
                    if (py >= Config::CHUNK_HEIGHT || py < 0) continue;
                    if (px < 0) {
                        target = cnx;
                        px = 15;
                    } else if (px >= 16) {
                        target = cpx;
                        px = 0;
                    }
                    if (pz < 0) {
                        target = cnz;
                        pz = 15;
                    } else if (pz >= 16) {
                        target = cpz;
                        pz = 0;
                    }
                    if (target && target->getBlock(px, py, pz) == air) {
                        addFace({faces[i], cur, {x + obp.x, y + obp.y, z + obp.z}}, renderer);
                    }

                    // auto nb = self->getBlock(px, py, pz);
                    // if (nb == air) {
                    // }

                    // if (py < 0 || py >= Config::CHUNK_HEIGHT) continue;
                    // if (px == 16) {
                    //     target = cpx;
                    //     px = 0;
                    // }
                    // if (px < 0) {
                    //     target = cnx;
                    //     px = 15;
                    // }

                    // if (pz >= 16) {
                    //     target = cpz;
                    //     pz = 0;
                    // }
                    // if (pz < 0) {
                    //     target = cnz;
                    //     pz = 15;
                    // }
                    // if (!target || target->getBlock(px, py, pz) != air) continue;
                    // addFace({faces[i], cur, {x + obp.x, y + obp.y, z + obp.z}}, renderer);
                }
            }
        }
    }
    renderer.buffer().bind();
    for (auto& bucket : this->buckets_) {
        renderer.buffer().rebufferBucket(bucket);
    }
}

void ChunkMesh::addFace(const BlockFaceInfo& f, ChunkRenderer& render) {
    // preassign buckect
    if (buckectFull()) {
        auto bu = render.buffer().assgin(pos_);
        if (!bu.valid()) {
            this->buckets_.push_back(bu);
        }
        this->buckets_.push_back(bu);
    }
    // full-> no more space
    if (buckectFull()) {
        return;
    }

    auto vs = createFaceVertices(f.face);
    std::vector<Vert> verts;
    // auto normal = getFaceNormal(f.face);
    for (int i = 0; i < vs.size(); i++) {
        auto v = vs[i];
        // auto U = 0.0f, V = 0.0f;
        // if (i == 2) U = 1.0f;
        // if (i == 0) V = 1.0f;
        // if (i == 1) U = 1.0f, V = 1.0f;
        auto dx = static_cast<GLfloat>(v % 2);
        auto dy = static_cast<GLfloat>(v >= 4 ? 1 : 0);
        auto dz = static_cast<GLfloat>((v % 4) >= 2 ? 1 : 0);
        verts.push_back(Vert{dx + f.pos.x, dy + f.pos.y, dz + f.pos.z});
    }
    static int idxs[] = {0, 2, 1, 2, 0, 3};
    auto& buckect = this->buckets_.back();
    for (int i = 0; i < 6; i++) {
        buckect.bucket[buckect.size] = verts[idxs[i]];
        buckect.size++;
    }
}

void ChunkRenderer::init() {
    auto vd = Config::VIEW_DISTANCE;
    buffer_.init((vd * 2 + 1) * (vd * 2 + 1));
}

void ChunkRenderer::render(RenderContext& ctx) {
    last_vertices_count = 0;
    last_chunk_count = 0;
    auto& shader = ctx.shader();
    shader.use("chunk2");
    shader.setMat4("projection", Config::getProjectionMatrix());
    shader.setMat4("view", ctx.camera().getViewMatrix());
    glm::mat4 model;
    model = glm::translate(model, glm::vec3{0, 0, 0});
    shader.setMat4("model", model);
    this->buffer_.bind();
    std::vector<GLint> offsets_;
    std::vector<GLsizei> sizes_;
    auto camera_cp = ChunkPos::fromVec3(ctx.camera().position_);
    for (auto& kv : chunk_meshes_) {
        if (camera_cp.dis2(kv.first) > Config::VIEW_DISTANCE * Config::VIEW_DISTANCE) continue;
        auto* mesh = kv.second;
        last_chunk_count++;
        for (auto& bi : mesh->buckets_) {
            offsets_.push_back(static_cast<GLint>(bi.index) * ChunkBuffer::BUCKET_SIZE);
            sizes_.push_back(static_cast<GLsizei>(bi.size));
            last_vertices_count += bi.size;
        }
    }
    glMultiDrawArrays(GL_TRIANGLES, offsets_.data(), sizes_.data(), static_cast<GLsizei>(offsets_.size()));
}

void ChunkRenderer::updateMesh(LevelRenderer& level_render, RenderContext& ctx) {
    rmt_ScopedCPUSample(updateMesh, 0);
    auto camera_pos = ChunkPos::fromVec3(ctx.camera().position_);
    auto vd2 = (Config::VIEW_DISTANCE + 1) * (Config::VIEW_DISTANCE + 1);
    auto& client_level = level_render.client_level_;  //+1 to cache some chunks(not shown)
    if (!mesh_building_queue_.empty()) {
        rmt_ScopedCPUSample(buildMesh, 0);
        // building mesh
        auto pos = *this->mesh_building_queue_.begin();
        this->mesh_building_queue_.erase(pos);
        if (camera_pos.dis2(pos) > vd2) return;
        if (this->chunk_meshes_.find(pos) != this->chunk_meshes_.end()) return;
        auto* mesh = new ChunkMesh(pos);
        mesh->build(*this);
        this->chunk_meshes_[pos] = mesh;
    } else if (client_level->newDataReceived()) {
        rmt_ScopedCPUSample(processNewData, 0);
        // current basic data

        auto& data = client_level->newestChunks();
        std::unordered_map<ChunkPos, std::tuple<uint8_t, uint8_t>> newMasks;
        {
            rmt_ScopedCPUSample(buildMask, 0);
            for (auto& [pos, chunk] : this->data_) {
                newMasks[pos] = LevelChunk::adjacentMask(pos, this->data_);
            }
        }

        {
            rmt_ScopedCPUSample(buildQueue, 0);
            for (auto& [pos, chunk] : data) {
                if (this->chunk_meshes_.find(pos) == this->chunk_meshes_.end()) {
                    mesh_building_queue_.insert(pos);
                    continue;
                }

                auto curMask = newMasks[pos];
                auto lastMask = this->masks_[pos];
                if (std::get<0>(curMask) != std::get<0>(lastMask) || std::get<1>(curMask) > 0) {
                    mesh_building_queue_.insert(pos);
                    continue;
                }
            }
        }
        {
            rmt_ScopedCPUSample(CopyData, 0);
            this->data_ = std::move(data);
            this->masks_ = std::move(newMasks);
            data.clear();
        }
    }

    // free data in free time
    {
        rmt_ScopedCPUSample(FreeMesh, 0);
        for (auto it = chunk_meshes_.begin(); it != chunk_meshes_.end(); it++) {
            if (camera_pos.dis2(it->first) > vd2) {
                it->second->freeBuckect(*this);
                delete it->second;
                it = chunk_meshes_.erase(it);
            }
        }
    }
}

ChunkRenderer::~ChunkRenderer() {
    for (auto& kv : chunk_meshes_) {
        delete kv.second;
    }
    delete demo_mesh_;
}
