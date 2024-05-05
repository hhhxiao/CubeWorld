#pragma once

#include <cstddef>
#include <ratio>
#include "block.h"
#include "chunk.h"
#include "drawable_object.h"
#include "game_camera.h"
#include "simple_shader.h"
#include "opengl_window.h"
#include "config.h"
#include "level.h"
#include "resource_manager.h"
#include "subchunk_mesh.h"
#include "terrain_generator.h"
#include "texture.h"

class GameMain {
   public:
    GameMain() {
        this->window = new OpenGLWindow(Config::window_width, Config::window_height, Config::SOFTWARE_NAME);
        this->window->setMouseEnable(this->enable_mouse_);
        this->resource_manager_ = new ResourceManager(R"(.\res)");
        shader =
            new Shader(resource_manager_->shader_path() / "main.vert", resource_manager_->shader_path() / "main.frag");
        this->camera_ = new GameCamera(glm::vec3(0.0f, 70.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        TexturePool::instance().init(resource_manager_->texture_path());
        this->level = new Level();
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
    size_t tick_{0};
    double last_frame_time_{0.0};
    double delta_time_{0.0};
    GameCamera *camera_;
    ResourceManager *resource_manager_;
    Shader *shader;
    OpenGLWindow *window{nullptr};
    // data
    DrawableObject debugObj;
    Level *level;
    // control
    bool enable_mouse_{false};
};
