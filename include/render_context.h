#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include "game_camera.h"
#include "resource_manager.h"
#include "simple_shader.h"

class RenderContext {
   public:
    RenderContext();

    ~RenderContext();

    inline GameCamera &camera() { return camera_; }
    inline ResourceManager &resourceMgr() { return resource_manager_; }
    inline Shader &shader() { return shader_; }

    void init();

   private:
    GameCamera camera_;
    ResourceManager resource_manager_;
    Shader shader_;
};
#endif
