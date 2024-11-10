#pragma once
#include "cube_map.h"
#include "imgui_debug_info.h"
#include "level_renderer.h"
#include "opengl_window.h"
#include "render_context.h"
#include "level_server.h"

class ClientMain {
   public:
    ClientMain() {
        this->window_ = new OpenGLWindow(Config::window_width, Config::window_height, Config::SOFTWARE_NAME);
        this->window_->setMouseEnable(this->enable_mouse_);
        this->level_ = new LevelServer();
        this->level_render_ = new LevelRenderer();
    }

    void show();

    void init();

    ~ClientMain();

   private:
    void renderTick();
    void gameTick();
    void processKeyBoardInput(GLFWwindow *window, double delta);
    void processMouseInput(GLFWwindow *window, double x, double y);

    // render
    OpenGLWindow *window_{nullptr};
    RenderContext render_ctx_;
    ImguiInfoDisplayer imgui_diplayer_;

    // for debug

    CubeMap *skybox{nullptr};
    // not uese currently
    LevelRenderer *level_render_{nullptr};
    // data
    LevelServer *level_{nullptr};
    // control
    bool enable_mouse_{false};
};
