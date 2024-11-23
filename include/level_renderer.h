#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

#include <cstddef>
#include "chunk_mesh.h"

class RenderContext;
class ClientLevel;

class LevelRenderer {
    static constexpr size_t VIEW_DISTANCE = 5;

   public:
    explicit LevelRenderer(ClientLevel* clientLevel);
    ~LevelRenderer();

    void init();

    void updateMesh(RenderContext& ctx);

    void renderOneFrame(RenderContext& ctx);

    void renderBlockWorld(RenderContext& ctx);

    [[nodiscard]] size_t render_tick() const { return render_tick_; }

   private:
    size_t render_tick_;
    ClientLevel* client_level_{nullptr};
    ChunkMeshMgr chuink_mesh_manager_;
    // data
    // debug
};
#endif
