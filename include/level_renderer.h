#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

#include <cstddef>
#include "drawable_object.h"
#include "level_mesh.h"

class RenderContext;
class Level;
class LevelRenderer {
    static constexpr size_t VIEW_DISTANCE = 5;

   public:
    LevelRenderer(Level*);
    ~LevelRenderer();

    void rednerOneFrame(RenderContext& ctx);

    void renderBlockWorld(RenderContext& ctx);

    size_t render_tick() const { return render_tick_; }

   private:
   private:
    LevelMesh* level_mesh_{nullptr};
    size_t render_tick_;
    Level* level_{nullptr};
    // debug
    DrawableObject obj_;
};
#endif
