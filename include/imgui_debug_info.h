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
        if (Config::ENABLE_IMGUI) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            for (auto &info : infos_) {
                ImGui::Begin(info.first.c_str());
                info.second->showDebugInfo();
                ImGui::End();
            }
        }
        if (Config::ENABLE_IMGUI) {
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

class ConfigurationDisplay : public ImguiInfo {
   public:
    ConfigurationDisplay() : ImguiInfo("Configurations") {}
    void showDebugInfo() override {
        if (ImGui::CollapsingHeader("Fog")) {
            ImGui::Checkbox("Enable Fog", &Config::enableFog);
            ImGui::SliderFloat("For Near", &Config::fogNear, 0, Config::VIEW_DISTANCE * 16 + 20);
            ImGui::SliderFloat("For Far", &Config::fogFar, 0, Config::VIEW_DISTANCE * 16 + 20);
        }
        if (ImGui::CollapsingHeader("ShadowMap")) {
            ImGui::SliderFloat("Side", &Config::depth_ortho_side, 0, 1000);
            ImGui::SliderFloat("Near", &Config::depth_ortho_z_near, -1000, 0);
            ImGui::SliderFloat("Far", &Config::depth_ortho_z_far, 0, 6000);
            ImGui::SliderFloat3("Sun light", &Config::sun_light_dir.x, -10, 10);
        }
        if (ImGui::CollapsingHeader("SSAO")) {
            ImGui::Checkbox("Enabele SSAO", &Config::enableSSAO);
        }

        if (ImGui::CollapsingHeader("Debug")) {
            ImGui::Checkbox("Show shadowmap", &Config::show_debug_frame_);
            //  ImGui::ListBox("Select Frame", &Config::current_frame_, Config::debug_frame_items_, 2);
        }
    };
};

#endif
