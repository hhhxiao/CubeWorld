#include "level_renderer.h"
#include <cassert>
#include "chunk_mesh.h"
#include "render_context.h"
#include "client_level.h"

LevelRenderer::LevelRenderer(ClientLevel* clientLevel) : render_tick_(0), client_level_(clientLevel) {}

void LevelRenderer::renderOneFrame(RenderContext& ctx) {
    this->renderBlockWorld(ctx);
    // todo: render fog, env
}

void LevelRenderer::init() {}

void LevelRenderer::updateMesh(RenderContext& ctx) {
    auto size = client_level_->getAllChunks().size();
    auto va = Config::view_distance * 2 + 1;
    if (size < va * va) {
        // require that load_radius > view radius?
        return;
    }
    chuink_mesh_manager_.update(ctx, this->client_level_);
}

void LevelRenderer::renderBlockWorld(RenderContext& ctx) {
    auto& shader = ctx.shader();
    shader.use("chunk");
    shader.setMat4("projection", Config::getProjectionMatrix());
    shader.setMat4("view", ctx.camera().getViewMatrix());
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(0, 0, 0));
    shader.setMat4("model", model);
    this->chuink_mesh_manager_.render(ctx);
}

LevelRenderer::~LevelRenderer() = default;