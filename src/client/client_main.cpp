#include "client_main.h"
#include "GLFW/glfw3.h"
#include "buffer.h"
#include "imgui.h"
#include "imgui_debug_info.h"
#include "position.h"
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
    TextureManager::instance().init(render_ctx_.resourceMgr().texture_path());
    imgui_displayer_.init(window_->window());
    imgui_displayer_.addInfo(&bridge_->serverBuffer());
    imgui_displayer_.addInfo(this);
    imgui_displayer_.addInfo(new ConfigurationDisplay());

    level_render_->init();
    this->window_->onLogic([this](double delta) {
        processKeyBoardInput(this->window_->window(), delta);
        using namespace std::chrono;
        auto begin = steady_clock::now();
        syncRead();
        syncWrite();
        this->level_render_->updateMesh(render_ctx_);
        auto end = steady_clock::now();
        this->mspt_ = this->mspt_ * 0.95 + 0.05 * std::chrono::duration<double, std::milli>(end - begin).count();
    });
    this->window_->onRender([this](GLFWwindow*) {
        renderTick();
        imgui_displayer_.render();
    });
}

void ClientMain::renderTick() { level_render_->renderOneFrame(render_ctx_); }

void ClientMain::syncRead() {
    auto& buffer = bridge_->serverBuffer();
    if (!buffer.beginRead()) return;
    if (buffer.dirty()) {
        this->client_level_->syncChunks(buffer.chunks, ChunkPos::fromVec3(render_ctx_.camera().position_));
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
    if (this->mouse_enabled_) return;  // noting todo, has curser

    static bool firstMouse = true;
    static auto lastPos = glm::vec2(0.0, 0.0);
    auto currentPos = glm::vec2(x, y);

    // reset status
    if (mouse_status_changed_) {
        mouse_status_changed_ = false;
        lastPos = currentPos;
    }

    if (firstMouse) {
        lastPos = currentPos;
        firstMouse = false;
    }
    // position recording
    auto offset = currentPos - lastPos;
    lastPos = currentPos;
    offset *= 0.1;  // 灵敏度
    render_ctx_.camera().updateDir(offset.x, -offset.y);
}

void ClientMain::processKeyBoardCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action,
                                         int32_t mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        this->window_->setMouseEnable(!mouse_enabled_);
        mouse_enabled_ = !mouse_enabled_;
        mouse_status_changed_ = true;
        LD("Mouse changed");
    }
}

void ClientMain::showDebugInfo() {
    ImGui::Text("FPS: %d / %d", static_cast<int>(window_->fps()), static_cast<int>(window_->lowFps(1)));
    ImGui::Text("Logic frame time: %.3lf", mspt_);

    auto cp = render_ctx_.camera().position_;
    auto ccp = ChunkPos::fromVec3(cp);
    ImGui::Text("Pos: %.3f / %.3f / %.3f", cp.x, cp.y, cp.z);
    ImGui::Text("Chunk pos: %d / %d", ccp.x, ccp.z);
    ImGui::Text("Yaw / Pitch: %.3f / %.3f", render_ctx_.camera().yaw_, render_ctx_.camera().pitch_);

    auto& cr = level_render_->chunkRender();
    auto& sb = cr.solid_buffer();
    auto& tb = cr.translucent_buffer();
    ImGui::Text("Chunk Mesh building:  Total: %zu, In queue: %zu", cr.mesh_size(), cr.mesh_queue_size());
    ImGui::Text("Vertices: %zu", cr.last_vertices_count);
    ImGui::Text("Chunks: %zu", cr.last_chunk_count);
    ImGui::Text("Solid chunk Buffer: %zu / %zu", sb.used_size(), sb.unused_size() + sb.used_size());
    ImGui::Text("Translucent chunk Buffer: %zu / %zu", tb.used_size(), tb.unused_size() + tb.used_size());
    auto usage = cr.last_vertices_count * 100.0 / (sb.size() * 2);
    ImGui::Text("Bucket utilization: %.3lf %%", usage);
}

ClientMain::~ClientMain() {
    delete window_;
    delete level_render_;
    delete client_level_;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}