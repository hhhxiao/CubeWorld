#pragma once
#include "bridge.h"
#include "cube_map.h"
#include "imgui_debug_info.h"
#include "level_renderer.h"
#include "opengl_window.h"
#include "render_context.h"

class ClientMain {
   public:
    ClientMain(DataBridge *bridge) : bridge_(bridge) {
        this->window_ = new OpenGLWindow(Config::window_width, Config::window_height, Config::SOFTWARE_NAME);
        this->window_->setMouseEnable(this->enable_mouse_);
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

    OpenGLWindow *window_{nullptr};
    RenderContext render_ctx_;
    ImguiInfoDisplayer imgui_diplayer_;
    // for debug
    CubeMap *skybox{nullptr};
    LevelRenderer *level_render_{nullptr};
    // control
    bool enable_mouse_{false};

    // bridge
    DataBridge *bridge_{};
};
