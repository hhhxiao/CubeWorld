#pragma once
#include <cstdint>
#include <string>
#include "bridge.h"
#include "client_level.h"
#include "config.h"
#include "glm/detail/type_vec.hpp"
#include "imgui_debug_info.h"
#include "level_renderer.h"
#include "opengl_window.h"
#include "render_context.h"

class ClientMain : public ImguiInfo {
   public:
    explicit ClientMain(DataBridge* bridge) : ImguiInfo("Client"), bridge_(bridge) {
        window_ = new OpenGLWindow(Config::window_width, Config::window_height, std::string(Config::SOFTWARE_NAME));
        window_->setMouseEnable(mouse_enabled_);
        client_level_ = new ClientLevel(this);
        level_render_ = new LevelRenderer(this->client_level_);
    }

    void show();
    void init();
    virtual ~ClientMain();

    DataBridge* bridge() { return bridge_; }
    int time() const { return time_; }

   private:
    void renderTick();
    void syncRead();
    void syncWrite();

    void processKeyBoardInput(GLFWwindow* window, double delta);  // old
    void processMouseCallback(GLFWwindow* window, double x, double y);
    void processKeyBoardCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mode);

    void showDebugInfo();

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
    int time_{0};
    // bridge
    DataBridge* bridge_{};

    // stats
    double mspt_;
};
