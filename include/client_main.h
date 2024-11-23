#pragma once
#include "bridge.h"
#include "client_level.h"
#include "cube_map.h"
#include "imgui_debug_info.h"
#include "level_renderer.h"
#include "opengl_window.h"
#include "render_context.h"

class ClientMain {
   public:
    explicit ClientMain(DataBridge* bridge) : bridge_(bridge) {
        this->window_ = new OpenGLWindow(Config::window_width, Config::window_height, Config::SOFTWARE_NAME);
        this->window_->setMouseEnable(this->enable_mouse_);
        this->client_level_ = new ClientLevel();
        this->level_render_ = new LevelRenderer(this->client_level_);
    }

    void show();
    void init();
    ~ClientMain();

   private:
    void renderTick();
    void syncRead();
    void syncWrite();

    void processKeyBoardInput(GLFWwindow* window, double delta);
    void processMouseInput(GLFWwindow* window, double x, double y);

   private:
    // render
    OpenGLWindow* window_{nullptr};
    RenderContext render_ctx_;
    ImguiInfoDisplayer imgui_displayer_;
    CubeMap* skybox{nullptr};
    LevelRenderer* level_render_{nullptr};
    // data
    ClientLevel* client_level_{nullptr};
    // control
    bool enable_mouse_{false};
    // bridge
    DataBridge* bridge_{};
};
