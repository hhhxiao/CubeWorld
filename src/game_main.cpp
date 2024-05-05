#include "game_main.h"
#include <vector>
#include "GLFW/glfw3.h"
#include "block.h"
#include "config.h"
#include "drawable_object.h"
#include "game_camera.h"
#include "glm/detail/type_vec.hpp"
#include "imgui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "subchunk_mesh.h"
namespace {
    DrawableObject textureText() {
        std::vector<VertexAttribute> vas{{0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
                                         {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
                                         {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
                                         {-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f}};
        std::vector<GLuint> indices{0, 1, 3, 1, 3, 2};
        return DrawableObject(vas, indices);
    }
}  // namespace

void GameMain::init() {
    debugObj = textureText();
    debugObj.init();
    this->window->setMouseCallBack(
        [&](GLFWwindow *window, double x, double y) { this->processMouseInput(window, x, y); });

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.FontGlobalScale = 2.0;
    ImGui_ImplGlfw_InitForOpenGL(this->window->window(), true);
    ImGui_ImplOpenGL3_Init();
    this->window->onFrame([this](GLFWwindow *) {
        // 计算帧时间
        auto current_time = glfwGetTime();
        delta_time_ = current_time - last_frame_time_;
        last_frame_time_ = current_time;
        // 输入
        processKeyBoardInput(this->window->window());
        // 下面是渲染
        gameTick();
        if (Config::enableImgui) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Debug");
            ImGui::Text("Base");
            ImGui::Text("Tick: %zu", tick_);
            static auto display_frame_time = delta_time_;
            // 20帧更新一次显示，防止闪烁
            if (tick_ % 20 == 0) display_frame_time = delta_time_;
            auto current_chunk_pos = level->getChunkPos();
            ImGui::Text("Frame time: %.4lf (%.2f)", display_frame_time, 1 / display_frame_time);
            ImGui::Text("Camera");
            ImGui::SliderFloat("Fov", &Config::fov, 5.0, 100.0);
            ImGui::Text("XYZ: %.3lf %.3lf %.3lf", camera_->position_.x, camera_->position_.y, camera_->position_.z);

            ImGui::Text("Yaw: %.3lf", camera_->yaw_);
            ImGui::Text("Pitch %.3lf", camera_->pitch_);
            ImGui::Text("Chunks");
            ImGui::Text("Chunk XZ: %d %d", current_chunk_pos.x, current_chunk_pos.z);
            auto stat = this->level->getChunkStats();
            for (auto kv : stat) {
                ImGui::Text("%s: %zu", kv.first.c_str(), kv.second);
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
    this->level->draw(shader);
}

void GameMain::gameTick() { tick_++; }

void GameMain::show() { this->window->pool(); }

void GameMain::processKeyBoardInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        static auto temp_anti_shake_timer = 0;
        temp_anti_shake_timer++;
        if (temp_anti_shake_timer % 10 == 0) {  // 10帧检测一次按下
            this->window->setMouseEnable(!this->enable_mouse_);
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
    auto cp = camera_->position_;
    this->level->updatePlayerPos({(int)cp.x, (int)cp.y, (int)cp.z});
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
    delete window;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    delete this->resource_manager_;
    delete this->shader;
    delete this->camera_;
    delete this->level;
}
