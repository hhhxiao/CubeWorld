#pragma once
#include "cube_map.h"
#include "drawable_object.h"
#include "imgui_debug_info.h"
#include "level_renderer.h"
#include "opengl_window.h"
#include "level.h"
#include "render_context.h"

class GameMain {
   public:
    GameMain() {
        this->window_ = new OpenGLWindow(Config::window_width, Config::window_height, Config::SOFTWARE_NAME);
        this->window_->setMouseEnable(this->enable_mouse_);
        this->level_ = new Level();
        this->level_render_ = new LevelRenderer(level_);
    }

    void show();

    void init();

    ~GameMain();

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
    Level *level_{nullptr};
    // control
    bool enable_mouse_{false};
};
