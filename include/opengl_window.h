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
    void onFrame(const std::function<void(GLFWwindow *)> &func);

    void pool();

    ~OpenGLWindow();

   private:
    GLFWwindow *window_{nullptr};
    std::function<void(GLFWwindow *)> onRender;
};
