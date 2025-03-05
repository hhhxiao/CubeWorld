#ifndef BRIDGE_H
#define BRIDGE_H
#include <cstddef>
#include <memory>
#include <mutex>
#include "chunk.h"
#include "glm/detail/type_vec.hpp"
#include "imgui.h"
#include "imgui_debug_info.h"
#include "position.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

class LevelServer;
class CommandInterface {
   public:
    virtual void consume(LevelServer*) { consumed = true; }
    virtual void produce() { consumed = false; }
    bool consumed{true};
};

class TimeCommand : public CommandInterface {
   public:
    virtual void consume(LevelServer*);
    int time_{0};
};

class Buffer {
   public:
    [[nodiscard]] bool dirty() const { return dirty_; }
    [[nodiscard]] size_t version() const { return version_; }
    void cleanDirty() { dirty_ = false; }

    void beginWrite() {
        m_.lock();
        this->dirty_ = true;
        this->version_++;
    }
    void endWrite() { m_.unlock(); }

    bool beginRead() { return m_.try_lock(); }
    void endRead() { m_.unlock(); }

   protected:
    std::mutex m_;
    size_t version_{0};
    bool dirty_{false};
};

class ClientBuffer : public Buffer, public ImguiInfo {
   public:
    ClientBuffer() : ImguiInfo("Gaming Control") {}
    void showDebugInfo() {
        ImGui::SliderInt("Time", &time_command_.time_, 0, Config::TICK_PER_DAY);
        ImGui::SameLine();
        if (ImGui::Button("SetTime")) {
            beginWrite();
            time_command_.produce();
            endWrite();
        }
    }

   public:
    size_t client_time_;
    glm::vec3 camera_position;
    std::vector<std::unique_ptr<CommandInterface>> commands_;
    // commands
    TimeCommand time_command_;
};

class ServerBuffer : public Buffer, public ImguiInfo {
   public:
    ServerBuffer() : ImguiInfo("Sever") {}

    void showDebugInfo() override {
        auto cp = ChunkPos::fromVec3(player_position);
        ImGui::Text("Position: %.3f / %.3f / %.3f", player_position.x, player_position.y, player_position.z);
        ImGui::Text("Chunk: %d / %d", cp.x, cp.z);
        ImGui::Text("Cache size: %zu / %zu", chunks.size(), chunk_cache_size);
        ImGui::Text("Mspt: %.3lf", mspt);
        ImGui::Text("Server Time: %d", server_time_);
    }
    std::unordered_map<ChunkPos, LevelChunk> chunks;
    int server_time_{0};
    // statics
    glm::vec3 player_position;
    size_t chunk_cache_size;
    double mspt;
};

class DataBridge {
   public:
    ClientBuffer& clientBuffer() { return client_buffer_; }
    ServerBuffer& serverBuffer() { return server_buffer_; }

   private:
    ClientBuffer client_buffer_;
    ServerBuffer server_buffer_;
};

#endif
