#include "game_main.h"
#include "GLFW/glfw3.h"
#include "config.h"
#include "game_camera.h"
#include "glm/detail/type_vec.hpp"
#include "imgui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
namespace {
    // DrawableObject textureText() {
    //     std::vector<VertexAttribute> vas{{0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
    //                                      {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
    //                                      {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
    //                                      {-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f}};
    //     std::vector<GLuint> indices{0, 1, 3, 1, 3, 2};
    //     return DrawableObject(vas, indices);
    // }
}  // namespace

void GameMain::init() {
    this->window_->setMouseCallBack(
        [&](GLFWwindow *window, double x, double y) { this->processMouseInput(window, x, y); });

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.FontGlobalScale = 2.0;
    ImGui_ImplGlfw_InitForOpenGL(this->window_->window(), true);
    ImGui_ImplOpenGL3_Init();

    this->window_->onLogic([this]() {
        processKeyBoardInput(this->window_->window());
        this->gameTick();
    });

    this->window_->onRender([this](GLFWwindow *) {
        if (Config::enableImgui) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Debug");
            auto stat = this->level_->getStats();
            for (auto kv : stat) {
                ImGui::Text("%s: %s", kv.first.c_str(), kv.second.c_str());
            }
            ImGui::End();
        }
        renderTick();
        if (Config::enableImgui) {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    });
}

void GameMain::renderTick() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 绘制一个简单的正方形来调试相机
    this->shader->use();
    // 传入相关矩阵
    shader->setMat4("projection", Config::getProjectionMatrix());
    shader->setMat4("view", this->camera_->getViewMatrix());
    auto model = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0));
    shader->setMat4("model", model);
    //  this->level_->draw(shader);  // temp
    this->level_render_->rednerOneFrame(render_ctx_);
}

void GameMain::gameTick() { this->level_->tick(); }

void GameMain::show() { this->window_->pool(); }

void GameMain::processKeyBoardInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        static auto temp_anti_shake_timer = 0;
        temp_anti_shake_timer++;
        if (temp_anti_shake_timer % 10 == 0) {  // 10帧检测一次按下
            this->window_->setMouseEnable(!this->enable_mouse_);
            this->enable_mouse_ = !this->enable_mouse_;
        }
    }
    GameCamera::Dir dir{GameCamera::invalid};
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dir = GameCamera::forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dir = GameCamera::back;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dir = GameCamera::left;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dir = GameCamera::right;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) dir = GameCamera::up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) dir = GameCamera::down;
    camera_->Move(dir, (float)delta_time_ * 15.0f);
}

void GameMain::processMouseInput(GLFWwindow *window, double x, double y) {
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
    this->camera_->updateDir(offset.x, -offset.y);
}

GameMain::~GameMain() {
    delete window_;
    delete level_render_;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    delete this->resource_manager_;
    delete this->shader;
    delete this->camera_;
    delete this->level_;
}
