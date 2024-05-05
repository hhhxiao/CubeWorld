//
// Created by xhy on 2024/5/3.
//
#include "opengl_window.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "utils.h"

namespace {

    std::function<void(GLFWwindow *, double, double)> &mouse_callback_function() {
        static std::function<void(GLFWwindow *, double, double)> callback = [](GLFWwindow *window, double xpos,
                                                                               double ypos) {};
        return callback;
    }

    void framebuffer_size_callback(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

    void mouse_callback(GLFWwindow *window, double xpos, double ypos) { mouse_callback_function()(window, xpos, ypos); }
}  // namespace

OpenGLWindow::OpenGLWindow(int width, int height, const std::string &name) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window_ = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        ERROR("Can not create window");
        glfwTerminate();
    }

    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    // 捕捉鼠标
    glfwSetCursorPosCallback(window_, mouse_callback);
    glfwSetInputMode(this->window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window_);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ERROR("Failed to initialize GLA");
    }
    glEnable(GL_DEPTH_TEST);
}

void OpenGLWindow::setMouseCallBack(const std::function<void(GLFWwindow *, double, double)> &func) {
    mouse_callback_function() = func;
}

void OpenGLWindow::setMouseEnable(bool able) {
    glfwSetInputMode(this->window(), GLFW_CURSOR, able ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void OpenGLWindow::onFrame(const std::function<void(GLFWwindow *)> &render) { this->onRender = render; }
void OpenGLWindow::pool() {
    while (!glfwWindowShouldClose(this->window_)) {
        this->onRender(this->window_);
        glfwPollEvents();
        glfwSwapBuffers(window_);
    }
}

OpenGLWindow::~OpenGLWindow() { glfwTerminate(); }
