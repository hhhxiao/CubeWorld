#include "level_renderer.h"
#include <cassert>
#include "buffer.h"
#include "chunk.h"
#include "chunk_renderer.h"
#include "config.h"
#include "position.h"
#include "render_context.h"
#include "client_level.h"
#include "texture.h"

LevelRenderer::LevelRenderer(ClientLevel* clientLevel) : client_level_(clientLevel) {}

void LevelRenderer::renderOneFrame(RenderContext& ctx) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->renderShadowMap(ctx);
    this->renderGBuffer(ctx);
    this->renderSSAO(ctx);
    this->renderSkyBox(ctx);
    this->renderBlockWorld(ctx);
    this->renderBaseUI(ctx);
}

void LevelRenderer::init() {
    skybox.init();
    chunk_render_.init();
    quad_screen_buffer_.init();
    level_depth_map_.init();
    g_buffer_.init();
    ssao_buffer_.init();
    ssao_blur_buffer_.init();
}

void LevelRenderer::updateMesh(RenderContext& ctx) { chunk_render_.updateMesh(*this, ctx); }

void LevelRenderer::renderBlockWorld(RenderContext& ctx) {
    auto& shader = ctx.shader();
    shader.use("chunk");
    // Fog
    shader.setBool("enableFog", Config::enableFog);
    shader.setVec3("worldCamera", ctx.camera().position_);
    shader.setFloat("fogNear", Config::fogNear);
    shader.setFloat("fogFar", Config::fogFar);

    // Lighting
    shader.setVec3("sunLightDir", Config::sun_light_dir);
    shader.setFloat("ambient_strength", Config::ambient_strength);
    shader.setFloat("diffuse_strength", Config::diffuse_strength);

    // SSAO
    shader.setBool("enableSSAO", Config::enableSSAO);

    // Textures
    shader.setInt("gPosition", 0);
    shader.setInt("gNormal", 1);
    shader.setInt("gAlbedo", 2);
    shader.setInt("shadowMap", 3);
    shader.setInt("ssao", 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_buffer_.position_id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_buffer_.normal_id());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_buffer_.albedo_id());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, g_buffer_.shadow_id());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ssao_blur_buffer_.id());
    quad_screen_buffer_.render();
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

void LevelRenderer::renderGBuffer(RenderContext& ctx) {
    g_buffer_.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto& shader = ctx.shader();
    // // render chunks
    shader.use("gbuffer_chunk");
    shader.setMat4("projection", Config::getProjectionMatrix());
    shader.setMat4("view", ctx.camera().getViewMatrix());
    shader.setMat4("model", glm::mat4(1.0));
    shader.setFloat("near", Config::zNear);
    shader.setFloat("far", Config::zFar);
    shader.setInt("blockTexture", 0);
    shader.setInt("depthMap", 1);
    shader.setVec3("sunLightDir", Config::sun_light_dir);
    shader.setMat4("lightSpaceMatrix", depth_map_mvp_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureManager::instance().blockAtlas().id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, level_depth_map_.id());
    chunk_render_.render(ctx);
    g_buffer_.unbind();
}

void LevelRenderer::renderDebug(RenderContext& ctx) {
    if (Config::show_debug_frame_) {
        ctx.shader().use("screen_debug");
        ctx.shader().setFloat("aspectRatio",
                              static_cast<float>(Config::window_width) / static_cast<float>(Config::window_height));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_buffer_.albedo_id());
        quad_screen_buffer_.render();
    }
}

void LevelRenderer::renderSSAO(RenderContext& ctx) {
    ssao_buffer_.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    auto& shader = ctx.shader();
    shader.use("ssao");
    for (GLuint i = 0; i < 64; ++i) {
        shader.setVec3("samples[" + std::to_string(i) + "]", TextureManager::instance().ssaoNoiseTexture().kernel()[i]);
    }
    shader.setMat4("projection", Config::getProjectionMatrix());
    shader.setInt("gPositionDepth", 0);
    shader.setInt("gNormal", 1);
    shader.setInt("texNoise", 2);
    shader.setVec2("noiseScale", {Config::window_width / 4.0, Config::window_height / 4.0});
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_buffer_.position_id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_buffer_.normal_id());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, TextureManager::instance().ssaoNoiseTexture().id());
    quad_screen_buffer_.render();
    ssao_buffer_.unbind();
    // blur
    ssao_blur_buffer_.bind();
    shader.use("ssao_blur");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssao_buffer_.id());
    quad_screen_buffer_.render();
    ssao_blur_buffer_.unbind();
}

void LevelRenderer::renderSkyBox(RenderContext& ctx) {
    auto& shader = ctx.shader();
    glDepthMask(GL_FALSE);
    shader.use("overworld_sky");
    shader.setMat4("projection", Config::getProjectionMatrix());
    shader.setMat4("view", glm::mat4(glm::mat3(ctx.camera().getViewMatrix())));
    shader.setInt("gameTime", client_level_->time());
    shader.setInt("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureManager::instance().getCubeMapID("overworld_sky"));
    skybox.render();
    glDepthMask(GL_TRUE);
}
void LevelRenderer::renderBaseUI(RenderContext& ctx) {
    auto& shader = ctx.shader();
    shader.use("cursor");
    shader.setInt("srcWidth", Config::window_width);
    shader.setInt("srcHeight", Config::window_height);
    shader.setInt("gAlbedo", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_buffer_.albedo_id());
    glDisable(GL_DEPTH_TEST);
    quad_screen_buffer_.render();
    glEnable(GL_DEPTH_TEST);
}

LevelRenderer::~LevelRenderer() {}

bool LevelChunkRendererOrder::operator()(const LevelChunk& r1, const LevelChunk& r2) {
    return r1.pos().dis2(camera) < r2.pos().dis2(camera);
}

ChunkPos LevelChunkRendererOrder::camera = {0, 0};
