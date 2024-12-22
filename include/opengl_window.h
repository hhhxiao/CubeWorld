//
// Created by xhy on 2024/5/3.
//
#pragma once
#include <functional>
#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

class OpenGLWindow {
   public:
    using KeyboardFuncType = void(GLFWwindow *, int32_t, int32_t, int32_t, int32_t);
    using MouseFuncType = void(GLFWwindow *w, double, double);

   public:
    OpenGLWindow(int width, int height, const std::string &name);

    inline GLFWwindow *window() { return this->window_; }

    void setMouseCallback(const std::function<MouseFuncType> &func);
    void setKeyboardCallback(const std::function<KeyboardFuncType> &func);
    void setMouseEnable(bool able);

    void onRender(const std::function<void(GLFWwindow *)> &func);
    void onLogic(const std::function<void(double)> &func);

    void pool();

    ~OpenGLWindow();

   private:
    GLFWwindow *window_{nullptr};
    std::function<void(GLFWwindow *)> on_render_;
    std::function<void(double)> on_logic_;
    // timer
    double last_frame_time_{0.0};   // redner timer
    double last_update_time_{0.0};  // logic timer
    GLuint VAO{0};
    // size
};
