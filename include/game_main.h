#pragma once

#include <cstddef>
#include "game_camera.h"
#include "simple_shader.h"
#include "opengl_window.h"
#include "config.h"
#include "resource_manager.h"
class GameMain {
   public:
    GameMain() {
        this->window = new OpenGLWindow(Config::window_width, Config::window_height, Config::SOFTWARE_NAME);
        this->resource_manager_ = new ResourceManager(R"(C:\Users\xhy\dev\CubeWorld\res)");
        shader =
            new Shader(resource_manager_->shader_path() / "main.vert", resource_manager_->shader_path() / "main.frag");
        this->camera_ = new GameCamera(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f));
    }

    void show();

    void init();

    ~GameMain();

   private:
    void renderTick();
    void gameTick();
    void processKeyBoardInput(GLFWwindow *window);
    void processMouseInput(GLFWwindow *window, double x, double y);

    size_t tick_{0};
    // frame info
    double last_frame_time_{0.0};
    double delta_time_{0.0};
    //

    GameCamera *camera_;
    ResourceManager *resource_manager_;
    Shader *shader;
    OpenGLWindow *window{nullptr};
};
