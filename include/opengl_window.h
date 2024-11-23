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
    OpenGLWindow(int width, int height, const std::string &name);

    inline GLFWwindow *window() { return this->window_; }

    void setMouseCallBack(const std::function<void(GLFWwindow *w, double, double)> &func);
    void onRender(const std::function<void(GLFWwindow *)> &func);
    void onLogic(const std::function<void(double)> &func);

    void setMouseEnable(bool able);

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
};
