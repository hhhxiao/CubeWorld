#pragma once
#include <string>
#include "bridge.h"
#include "client_level.h"
#include "config.h"
#include "cube_map.h"
#include "glm/detail/type_vec.hpp"
#include "imgui_debug_info.h"
#include "level_renderer.h"
#include "opengl_window.h"
#include "render_context.h"

class ClientMain {
   public:
    explicit ClientMain(DataBridge* bridge) : bridge_(bridge) {
        window_ = new OpenGLWindow(Config::window_width, Config::window_height, std::string(Config::SOFTWARE_NAME));
        window_->setMouseEnable(mouse_enabled_);
        client_level_ = new ClientLevel();
        level_render_ = new LevelRenderer(this->client_level_);
    }

    void show();
    void init();
    ~ClientMain();

   private:
    void renderTick();
    void syncRead();
    void syncWrite();

    void processKeyBoardInput(GLFWwindow* window, double delta);  // old
    void processMouseCallback(GLFWwindow* window, double x, double y);
    void processKeyBoardCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mode);

   private:
    // render
    OpenGLWindow* window_{nullptr};
    RenderContext render_ctx_;
    ImguiInfoDisplayer imgui_displayer_;
    LevelRenderer* level_render_{nullptr};
    // control
    bool mouse_enabled_{false};
    bool mouse_status_changed_{false};
    glm::vec2 last_mouse_position_{0.0f, 0.0f};

    // data
    ClientLevel* client_level_{nullptr};
    // bridge
    DataBridge* bridge_{};
};
