#include "level_renderer.h"
#include <cassert>
#include "level_mesh.h"
LevelRenderer::LevelRenderer() { this->level_mesh_ = new LevelMesh(); }

void LevelRenderer::rednerOneFrame(RenderContext &ctx) {
    this->renderBlockWorld(ctx);
    // todo: render fog, env and UI
}

void LevelRenderer::renderBlockWorld(RenderContext &ctx) {}

LevelRenderer::~LevelRenderer() { delete this->level_mesh_; }