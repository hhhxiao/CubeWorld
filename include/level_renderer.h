#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

#include <unordered_map>
#include "buffer.h"
#include "chunk.h"
#include "chunk_renderer.h"
#include "position.h"

class ChunkMeshOld;
class RenderContext;
class ClientLevel;

struct LevelChunkRendererOrder {
    bool operator()(const LevelChunk& c1, const LevelChunk& c2);
    static ChunkPos camera;
};

class LevelRenderer {
    friend class ChunkRenderer;

   public:
    explicit LevelRenderer(ClientLevel* clientLevel);
    ~LevelRenderer();
    void init();

    void updateMesh(RenderContext& ctx);
    void renderOneFrame(RenderContext& ctx);
    void renderBlockWorld(RenderContext& ctx);
    void renderGBuffer(RenderContext& ctx);
    void renderShadowMap(RenderContext& ctx);
    void renderSSAO(RenderContext& ctx);
    void renderDebug(RenderContext& ctx);

    ChunkRenderer& chunkRender() { return chunk_render_; }

   private:
    ClientLevel* client_level_{nullptr};
    SkyBoxBuffer skybox;
    // chunk
    ChunkRenderer chunk_render_;
    // screen space buffer
    QuadScreenBuffer quad_screen_buffer_;
    // g-buffer
    GBuffer g_buffer_;
    // shadow map
    DepthMapBuffer level_depth_map_;
    glm::mat4 depth_map_mvp_;
    // ssao
    SSAOBuffer ssao_buffer_, ssao_blur_buffer_;
};
#endif
