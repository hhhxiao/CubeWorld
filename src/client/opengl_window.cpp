//
// Created by xhy on 2024/5/3.
//
#include "opengl_window.h"

#include "GLFW/glfw3.h"
#include "config.h"
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
        LE("Can not create window");
        glfwTerminate();
    }

    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    // 捕捉鼠标
    glfwSetCursorPosCallback(window_, mouse_callback);
    glfwSetInputMode(this->window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window_);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LE("Failed to initialize GLA");
    }
    glfwSwapInterval(0);  // 关闭垂直同步
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(VAO);
}

void OpenGLWindow::setMouseCallBack(const std::function<void(GLFWwindow *, double, double)> &func) {
    mouse_callback_function() = func;
}

void OpenGLWindow::setMouseEnable(bool able) {
    glfwSetInputMode(this->window(), GLFW_CURSOR, able ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void OpenGLWindow::onRender(const std::function<void(GLFWwindow *)> &render) { this->on_render_ = render; }
void OpenGLWindow::onLogic(const std::function<void(double)> &logic) { this->on_logic_ = logic; }

void OpenGLWindow::pool() {
    while (!glfwWindowShouldClose(this->window_)) {
        auto curTime = glfwGetTime();

        if (curTime - last_update_time_ >= Config::UPDATE_TIME) {
            glfwPollEvents();
            this->on_logic_(curTime - last_update_time_);
            last_update_time_ = curTime;
        }

        if (curTime - last_frame_time_ >= Config::FRAME_TIME) {
            this->on_render_(this->window_);
            glfwSwapBuffers(window_);
            last_frame_time_ = curTime;
        }
    }
}
OpenGLWindow::~OpenGLWindow() {
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
}
