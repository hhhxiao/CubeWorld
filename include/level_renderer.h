#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

#include "buffer.h"
#include "chunk.h"
#include "chunk_renderer.h"
#include "position.h"
#include "cube_map.h"

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
    ChunkRenderer& chunkRender() { return chunk_render_; }

   private:
    ClientLevel* client_level_{nullptr};
    CubeMap* skybox{nullptr};
    ChunkRenderer chunk_render_;
    // depth map
    QuadScreenBuffer depth_map_debug_buffer_;
    DepthMap level_depth_map_;
};
#endif
