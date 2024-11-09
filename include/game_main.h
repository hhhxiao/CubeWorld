#pragma once

#include "chunk.h"
#include "game_camera.h"
#include "level_renderer.h"
#include "simple_shader.h"
#include "opengl_window.h"
#include "config.h"
#include "level.h"
#include "resource_manager.h"
#include "subchunk_mesh.h"
#include "texture.h"

class RenderContext {
   public:
    GameCamera *camera{nullptr};
    Shader *shader{nullptr};
};

class GameMain {
   public:
    GameMain() {
        this->window_ = new OpenGLWindow(Config::window_width, Config::window_height, Config::SOFTWARE_NAME);
        this->window_->setMouseEnable(this->enable_mouse_);
        this->resource_manager_ = new ResourceManager(R"(..\res)");

        shader =
            new Shader(resource_manager_->shader_path() / "main.vert", resource_manager_->shader_path() / "main.frag");
        this->camera_ = new GameCamera(glm::vec3(0.0f, 70.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));

        render_ctx_.camera = this->camera_;
        render_ctx_.shader = this->shader;

        TexturePool::instance().init(resource_manager_->texture_path());

        this->level_ = new Level();
        this->level_render_ = new LevelRenderer(level_);
    }

    void show();

    void init();

    ~GameMain();

   private:
    void renderTick();
    void gameTick();
    void processKeyBoardInput(GLFWwindow *window);
    void processMouseInput(GLFWwindow *window, double x, double y);

    // render
    double last_frame_time_{0.0};
    double delta_time_{0.0};
    RenderContext render_ctx_;
    GameCamera *camera_;
    ResourceManager *resource_manager_;
    Shader *shader;
    OpenGLWindow *window_{nullptr};
    LevelRenderer *level_render_{nullptr};

    // data
    Level *level_;
    // control
    bool enable_mouse_{false};
};
