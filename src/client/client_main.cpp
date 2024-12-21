#include "client_main.h"
#include "GLFW/glfw3.h"
#include "cube_map.h"
#include "renderable.h"
#include "game_camera.h"
#include "glm/detail/type_vec.hpp"
#include "texture.h"

void ClientMain::init() {
    window_->setMouseCallback(
        [&](GLFWwindow* window, double x, double y) { this->processMouseCallback(window, x, y); });

    window_->setKeyboardCallback([&](GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mode) {
        this->processKeyBoardCallback(window, key, scancode, action, mode);
    });
    render_ctx_.init();
    TexturePool::instance().init(render_ctx_.resourceMgr().texture_path());
    this->level_render_->init();
    imgui_displayer_.init(window_->window());
    imgui_displayer_.addInfo(&render_ctx_.camera());
    imgui_displayer_.addInfo(&bridge_->serverBuffer());

    skybox = new CubeMap();
    //   skybox->init();
    // setup update function
    this->window_->onLogic([this](double delta) {
        processKeyBoardInput(this->window_->window(), delta);
        syncRead();
        syncWrite();
        this->level_render_->updateMesh(render_ctx_);
    });
    this->window_->onRender([this](GLFWwindow*) {
        renderTick();
        imgui_displayer_.render();
    });
}

void ClientMain::renderTick() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // skybox->render(render_ctx_);
    level_render_->renderOneFrame(render_ctx_);
}

void ClientMain::syncRead() {
    auto& buffer = bridge_->serverBuffer();
    if (!buffer.beginRead()) return;
    if (buffer.dirty()) {
        this->client_level_->syncChunks(buffer.chunks);
        buffer.cleanDirty();
    }
    buffer.endRead();
}

void ClientMain::syncWrite() {
    auto& buffer = bridge_->clientBuffer();
    buffer.beginWrite();
    buffer.camera_position = render_ctx_.camera().position_;
    buffer.endWrite();
}

void ClientMain::show() { this->window_->pool(); }

void ClientMain::processKeyBoardInput(GLFWwindow* window, double delta) {
    GameCamera::Dir dir{GameCamera::invalid};
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dir = GameCamera::forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dir = GameCamera::back;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dir = GameCamera::left;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dir = GameCamera::right;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) dir = GameCamera::up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) dir = GameCamera::down;
    render_ctx_.camera().move(dir, (float)delta * 15.0f);
}

void ClientMain::processMouseCallback(GLFWwindow* window, double x, double y) {
    if (this->mouse_enabled_) return;
    static bool firstMouse = false;
    auto current = glm::vec2(x, y);
    static auto lastPos = glm::vec2(0.0, 0.0);
    if (firstMouse) {
        lastPos = current;
        firstMouse = true;
    }
    auto offset = current - lastPos;
    lastPos = current;
    offset *= 0.1;  // 灵敏度
    render_ctx_.camera().updateDir(offset.x, -offset.y);
}

void ClientMain::processKeyBoardCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action,
                                         int32_t mode) {
    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
        this->window_->setMouseEnable(!mouse_enabled_);
        mouse_enabled_ = !mouse_enabled_;
    }
}

ClientMain::~ClientMain() {
    delete window_;
    delete skybox;
    delete level_render_;
    delete client_level_;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}