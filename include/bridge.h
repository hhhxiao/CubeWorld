#ifndef BRIDGE_H
#define BRIDGE_H
#include <cstddef>
#include <mutex>
#include "chunk.h"
#include "glm/detail/type_vec.hpp"
#include "imgui.h"
#include "imgui_debug_info.h"
#include "position.h"
#include <glm/glm.hpp>
#include <vector>

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

class ClientBuffer : public Buffer {
   public:
    glm::vec3 camera_position;
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
    }
    std::vector<LevelChunk> chunks;
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
