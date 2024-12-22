#include "level_renderer.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <utility>
#include "block.h"
#include "chunk.h"
#include "chunk_mesh.h"
#include "config.h"
#include "position.h"
#include "render_context.h"
#include "client_level.h"

LevelRenderer::LevelRenderer(ClientLevel* clientLevel) : client_level_(clientLevel) {}

void LevelRenderer::renderOneFrame(RenderContext& ctx) {
    this->renderBlockWorld(ctx);
    // todo: render fog, env
}

void LevelRenderer::init() {}

void LevelRenderer::updateMesh(RenderContext& ctx) {
    if (!render_chunk_queue_.empty()) {
        if (!current_mesh_) {
            auto top = this->render_chunk_queue_.top();
            render_chunk_queue_.pop();
            // build mesh
            current_mesh_ = new ChunkMesh(top.pos());
            current_mesh_->stepInit(this);
        } else {
            if (!current_mesh_->hasInited()) {
                current_mesh_->stepInit(this);
            } else {
                auto pos = current_mesh_->pos();
                auto it = this->meshes_.find(pos);
                if (it != this->meshes_.end()) {
                    delete it->second;
                }
                this->meshes_[pos] = current_mesh_;
                current_mesh_ = nullptr;
            }
        }
    } else if (client_level_->newDataReceived()) {
        auto cp = ChunkPos::fromVec3(ctx.camera().position_);
        LevelChunkRenderOrder::camera = cp;
        // current basic data
        auto data = client_level_->fetchChunkData();
        std::unordered_map<ChunkPos, std::tuple<uint8_t, uint8_t>> newMasks;
        for (auto& [pos, chunk] : this->data_) {
            newMasks[pos] = LevelChunk::adjacentMask(pos, this->data_);
        }
        for (auto& [pos, chunk] : data) {
            auto dx = abs(pos.x - cp.x);
            auto dz = abs(pos.z - cp.z);
            if (dx * dx + dz * dz > Config::VIEW_DISTANCE * Config::VIEW_DISTANCE) continue;

            if (!this->data_.count(pos)) {
                render_chunk_queue_.push(chunk);
                continue;
            }

            auto curMask = newMasks[pos];
            auto lastMask = this->masks_[pos];
            // // edge chunk changed
            if (std::get<0>(curMask) != std::get<0>(lastMask) || std::get<1>(curMask) > 0) {
                render_chunk_queue_.push(chunk);
                continue;
            }
        }
        this->data_ = std::move(data);
        this->masks_ = std::move(newMasks);
        for (auto it = meshes_.begin(); it != meshes_.end();) {
            if (!this->data_.count(it->first)) {
                delete it->second;
                it = meshes_.erase(it);
            } else {
                it++;
            }
        }
    }
}

void LevelRenderer::renderBlockWorld(RenderContext& ctx) {
    auto& shader = ctx.shader();
    shader.use("chunk");
    shader.setMat4("projection", Config::getProjectionMatrix());
    shader.setMat4("view", ctx.camera().getViewMatrix());
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(0, 0, 0));
    shader.setMat4("model", model);
    for (auto& [pos, mesh] : this->meshes_) {
        mesh->render(ctx);
    }
}

BlockType LevelRenderer::getBlock(int x, int y, int z) {
    BlockPos p{x, y, z};
    const auto cp = p.toChunkPos();
    const auto [ox, _, oz] = cp.toBlockPos();
    const auto it = this->data_.find(cp);
    return it == this->data_.end() ? BlockType::bedrock : it->second.getBlock(x - ox, y, z - oz);
}

LevelChunk* LevelRenderer::getChunkData(const ChunkPos& pos) {
    auto it = this->data_.find(pos);
    return it == this->data_.end() ? nullptr : &it->second;
}

LevelRenderer::~LevelRenderer() = default;

bool LevelChunkRenderOrder::operator()(const LevelChunk& r1, const LevelChunk& r2) {
    return r1.pos().dis2(camera) < r2.pos().dis2(camera);
}

ChunkPos LevelChunkRenderOrder::camera = {0, 0};
