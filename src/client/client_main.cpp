#include "client_main.h"
#include "GLFW/glfw3.h"
#include "cube_map.h"
#include "renderable.h"
#include "game_camera.h"
#include "glm/detail/type_vec.hpp"
#include "texture.h"
#include "utils.h"

void ClientMain::init() {
    window_->setMouseCallBack([&](GLFWwindow *window, double x, double y) { this->processMouseInput(window, x, y); });
    render_ctx_.init();
    TexturePool::instance().init(render_ctx_.resourceMgr().texture_path());
    imgui_diplayer_.init(window_->window());
    imgui_diplayer_.addInfo(&render_ctx_.camera());
    skybox = new CubeMap();
    skybox->init();
    // setup update function
    this->window_->onLogic([this](double delta) {
        processKeyBoardInput(this->window_->window(), delta);
        syncRead();
        syncWrite();
    });

    this->window_->onRender([this](GLFWwindow *) {
        renderTick();
        imgui_diplayer_.render();
    });
}

void ClientMain::renderTick() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    skybox->render(render_ctx_);
}

void ClientMain::syncRead() {
    auto &buffer = bridge_->serverBuffer();
    if (!buffer.dirty()) return;
    LD("Read buffer(version: %zu)", buffer.version());
    buffer.cleanDirty();
}

void ClientMain::syncWrite() {
    auto &buffer = bridge_->clientBuffer();
    buffer.beginWrite();
    buffer.camera_position = render_ctx_.camera().position_;
    buffer.endWrite();
}

void ClientMain::show() { this->window_->pool(); }

void ClientMain::processKeyBoardInput(GLFWwindow *window, double delta) {
    // if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        static auto temp_anti_shake_timer = 0;
        temp_anti_shake_timer++;
        if (temp_anti_shake_timer % 10 == 0) {  // 这里修改了逻辑，要重新做检查，暂时懒得改了
            this->window_->setMouseEnable(!this->enable_mouse_);
            this->enable_mouse_ = !this->enable_mouse_;
            temp_anti_shake_timer = 0;
        }
    }
    GameCamera::Dir dir{GameCamera::invalid};
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dir = GameCamera::forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dir = GameCamera::back;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dir = GameCamera::left;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dir = GameCamera::right;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) dir = GameCamera::up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) dir = GameCamera::down;
    render_ctx_.camera().move(dir, (float)delta * 15.0f);
}
void ClientMain::processMouseInput(GLFWwindow *window, double x, double y) {
    if (enable_mouse_) return;
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

ClientMain::~ClientMain() {
    delete window_;
    delete skybox;
    delete level_render_;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
