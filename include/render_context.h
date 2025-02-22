#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include "chunk_renderer.h"
#include "game_camera.h"
#include "resource_manager.h"
#include "simple_shader.h"
#include "glad/glad.h"

class RenderContext {
   public:
    RenderContext();

    ~RenderContext();

    inline GameCamera &camera() { return camera_; }
    inline ResourceManager &resourceMgr() { return resource_manager_; }
    inline Shader &shader() { return shader_; }

    inline void setLevelDepthMap(GLuint id) { level_depth_map_ = id; }
    inline void tryBindLevelDepthMap(GLenum texture) {
        if (level_depth_map_ != 0) {
            glActiveTexture(texture);
            glBindTexture(GL_TEXTURE_2D, level_depth_map_);
        }
    }

    void init();

   private:
    GameCamera camera_;
    ResourceManager resource_manager_;
    Shader shader_;

    // level depth map
    GLuint level_depth_map_{0};
};
#endif
