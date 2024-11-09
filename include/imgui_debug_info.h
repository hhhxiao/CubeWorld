#ifndef IMGUI_DEBUG_INFO_H
#define IMGUI_DEBUG_INFO_H
#include <sys/types.h>
#include "config.h"
#include "imgui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>
#include <unordered_map>
class ImguiInfo {
   public:
    ImguiInfo(const std::string &name) : imgui_win_name_(name) {}
    virtual void showDebugInfo() {};
    std::string imgui_win_name_;
};

class ImguiInfoDisplayer {
   public:
    void init(GLFWwindow *window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.FontGlobalScale = 2.0;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
    }
    void render() {
        if (Config::enableImgui) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            for (auto &info : infos_) {
                ImGui::Begin(info.first.c_str());
                info.second->showDebugInfo();
                ImGui::End();
            }
        }
        if (Config::enableImgui) {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    }

    void addInfo(ImguiInfo *info) {
        if (!info) return;
        infos_[info->imgui_win_name_] = info;
    }

   private:
    std::unordered_map<std::string, ImguiInfo *> infos_;
};
#endif
