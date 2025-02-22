//
// Created by xhy on 2024/5/3.
//
#include "opengl_window.h"

#include "GLFW/glfw3.h"
#include "config.h"
#include "glad/glad.h"
#include "utils.h"
#include <iostream>

namespace {

    // mouse
    std::function<OpenGLWindow::MouseFuncType> &mouse_callback_function() {
        static std::function<OpenGLWindow::MouseFuncType> callback{};
        return callback;
    }
    void mouse_callback(GLFWwindow *window, double xpos, double ypos) { mouse_callback_function()(window, xpos, ypos); }

    // keyboard
    std::function<OpenGLWindow::KeyboardFuncType> &keyboard_callback_function() {
        static std::function<OpenGLWindow::KeyboardFuncType> callback{};
        return callback;
    }

    void keyboard_callback(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mode) {
        keyboard_callback_function()(window, key, scancode, action, mode);
    }
    void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
        Config::updateWindowSize(width, height);
    }
    // Callback function to handle OpenGL debug messages
    void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
        // Ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::cout << "---------------" << std::endl;
        std::cout << "Debug message (" << id << "): " << message << std::endl;

        switch (source) {
            case GL_DEBUG_SOURCE_API:
                std::cout << "Source: API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                std::cout << "Source: Window System";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                std::cout << "Source: Shader Compiler";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                std::cout << "Source: Third Party";
                break;
            case GL_DEBUG_SOURCE_APPLICATION:
                std::cout << "Source: Application";
                break;
            case GL_DEBUG_SOURCE_OTHER:
                std::cout << "Source: Other";
                break;
        }
        std::cout << std::endl;

        switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                std::cout << "Type: Error";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                std::cout << "Type: Deprecated Behaviour";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                std::cout << "Type: Undefined Behaviour";
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                std::cout << "Type: Portability";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                std::cout << "Type: Performance";
                break;
            case GL_DEBUG_TYPE_MARKER:
                std::cout << "Type: Marker";
                break;
            case GL_DEBUG_TYPE_PUSH_GROUP:
                std::cout << "Type: Push Group";
                break;
            case GL_DEBUG_TYPE_POP_GROUP:
                std::cout << "Type: Pop Group";
                break;
            case GL_DEBUG_TYPE_OTHER:
                std::cout << "Type: Other";
                break;
        }
        std::cout << std::endl;

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                std::cout << "Severity: high";
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                std::cout << "Severity: medium";
                break;
            case GL_DEBUG_SEVERITY_LOW:
                std::cout << "Severity: low";
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                std::cout << "Severity: notification";
                break;
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

}  // namespace

OpenGLWindow::OpenGLWindow(int width, int height, const std::string &name) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    window_ = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        LE("Can not create window");
        glfwTerminate();
    }

    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwSetKeyCallback(window_, keyboard_callback);
    glfwSetCursorPosCallback(window_, mouse_callback);
    glfwSetInputMode(this->window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window_);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LE("Failed to initialize GLA");
    }
    // Enable OpenGL debug output
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        LD("Eanbel OpenGL Debug...");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glfwSwapInterval(0);  // 关闭垂直同步
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(VAO);
}

void OpenGLWindow::setMouseCallback(const std::function<MouseFuncType> &func) { mouse_callback_function() = func; }
void OpenGLWindow::setKeyboardCallback(const std::function<KeyboardFuncType> &func) {
    keyboard_callback_function() = func;
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
            frame_++;
            if (frame_ % 100 == 0) {
                auto cur_fps = 1.0 / (curTime - last_frame_time_);
                this->fps_counter_.push(cur_fps);
                this->fps_ = this->fps_ * 0.9 + cur_fps * 0.1;
            }
            // render
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
