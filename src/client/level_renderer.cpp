#include "level_renderer.h"
#include <cassert>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <utility>
#include "block.h"
#include "chunk.h"
#include "chunk_mesh.h"
#include "config.h"
#include "cube_map.h"
#include "position.h"
#include "render_context.h"
#include "client_level.h"

LevelRenderer::LevelRenderer(ClientLevel* clientLevel) : client_level_(clientLevel) {}

void LevelRenderer::renderOneFrame(RenderContext& ctx) {
    skybox->render(ctx);
    this->renderBlockWorld(ctx);
    //   light.render(ctx);
    // todo: render fog, env
}

void LevelRenderer::init() {
    light.init();
    skybox = new CubeMap();
    skybox->init();
    chunk_render_.init();
}

void LevelRenderer::updateMesh(RenderContext& ctx) { chunk_render_.updateMesh(*this, ctx); }

void LevelRenderer::renderBlockWorld(RenderContext& ctx) {
    chunk_render_.render(ctx);
    // auto& shader = ctx.shader();
    // shader.use("chunk");
    // shader.setMat4("projection", Config::getProjectionMatrix());
    // shader.setMat4("view", ctx.camera().getViewMatrix());

    // shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    // shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    // shader.setVec3("lightPos", light.position());
    // shader.setVec3("viewPos", ctx.camera().position_);

    // glm::mat4 model;
    // model = glm::translate(model, glm::vec3(0, 0, 0));
    // shader.setMat4("model", model);
    // for (auto& [pos, mesh] : this->meshes_) {
    //     mesh->render(ctx);
    // }
}

BlockType LevelRenderer::getBlock(int x, int y, int z) {
    BlockPos p{x, y, z};
    const auto cp = p.toChunkPos();
    const auto [ox, _, oz] = cp.toBlockPos();
    // const auto it = this->data_.find(cp);
    // return it == this->data_.end() ? BlockType::bedrock : it->second.getBlock(x - ox, y, z - oz);

    return air;
}

LevelChunk* LevelRenderer::getChunkData(const ChunkPos& pos) {
    // auto it = this->data_.find(pos);
    // return it == this->data_.end() ? nullptr : &it->second;
    return nullptr;
}

LevelRenderer::~LevelRenderer() { delete skybox; }

bool LevelChunkRendererOrder::operator()(const LevelChunk& r1, const LevelChunk& r2) {
    return r1.pos().dis2(camera) < r2.pos().dis2(camera);
}

ChunkPos LevelChunkRendererOrder::camera = {0, 0};
