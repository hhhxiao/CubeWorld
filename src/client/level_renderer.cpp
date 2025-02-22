#include "level_renderer.h"
#include <cassert>
#include "buffer.h"
#include "chunk.h"
#include "chunk_renderer.h"
#include "config.h"
#include "cube_map.h"
#include "position.h"
#include "render_context.h"
#include "client_level.h"
#include "texture.h"

LevelRenderer::LevelRenderer(ClientLevel* clientLevel) : client_level_(clientLevel) {}

void LevelRenderer::renderOneFrame(RenderContext& ctx) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->renderShadowMap(ctx);
    skybox->render(ctx);
    this->renderBlockWorld(ctx);
}

void LevelRenderer::init() {
    skybox = new CubeMap();
    skybox->init();
    chunk_render_.init();
    depth_map_debug_buffer_.init();
    level_depth_map_.init();
}

void LevelRenderer::updateMesh(RenderContext& ctx) { chunk_render_.updateMesh(*this, ctx); }

void LevelRenderer::renderBlockWorld(RenderContext& ctx) {
    auto& shader = ctx.shader();
    shader.use("chunk");
    shader.setMat4("projection", Config::getProjectionMatrix());
    shader.setMat4("view", ctx.camera().getViewMatrix());
    shader.setMat4("model", glm::mat4(1.0));
    shader.setVec3("worldCamera", ctx.camera().position_);
    shader.setFloat("fogNear", Config::fogNear);
    shader.setFloat("fogFar", Config::fogFar);
    shader.setBool("enableFog", Config::enableFog);
    shader.setMat4("lightSpaceMatrix", depth_map_mvp_);
    shader.setVec3("sunLightDir", Config::sun_light_dir);
    shader.setInt("blockTexture", 0);
    shader.setInt("shadowMap", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureManager::instance().blockAtlas().id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, level_depth_map_.id());
    chunk_render_.render(ctx);

    if (Config::show_debug_shadow_map) {
        glViewport(0, 0, 512, 512);
        ctx.shader().use("shadow_map_debug");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, level_depth_map_.id());
        depth_map_debug_buffer_.render();
        glViewport(0, 0, Config::window_width, Config::window_height);
    }
}

void LevelRenderer::renderShadowMap(RenderContext& ctx) {
    level_depth_map_.bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    auto& shader = ctx.shader();
    shader.use("depth_map");
    auto os = Config::depth_ortho_side;
    auto ozn = Config::depth_ortho_z_near;
    auto ozf = Config::depth_ortho_z_far;
    auto depth_projection_mat = glm::ortho<float>(-os, os, -os, os, ozn, ozf);
    auto light_pos = ctx.camera().position_ - Config::sun_light_dir * 10.0f;
    auto depth_view_mat = glm::lookAt(light_pos, ctx.camera().position_, glm::vec3(0, 1, 0));
    auto depth_model_mat = glm::mat4(1.0);
    this->depth_map_mvp_ = depth_projection_mat * depth_view_mat * depth_model_mat;
    shader.setMat4("depthMVP", this->depth_map_mvp_);
    glViewport(0, 0, 4096, 4096);
    glClear(GL_DEPTH_BUFFER_BIT);
    chunk_render_.render(ctx);
    level_depth_map_.unbind();
    glViewport(0, 0, Config::window_width, Config::window_height);
}

LevelRenderer::~LevelRenderer() { delete skybox; }

bool LevelChunkRendererOrder::operator()(const LevelChunk& r1, const LevelChunk& r2) {
    return r1.pos().dis2(camera) < r2.pos().dis2(camera);
}

ChunkPos LevelChunkRendererOrder::camera = {0, 0};
