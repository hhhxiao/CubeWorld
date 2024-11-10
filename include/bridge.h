#ifndef BRIDGE_H
#define BRIDGE_H
#include <cstddef>
#include <mutex>
#include "chunk.h"
#include "glm/detail/type_vec.hpp"
#include "position.h"
#include <glm/glm.hpp>
#include <map>
#include <unordered_map>

class Buffer {
   public:
    inline bool dirty() const { return dirty_; }
    inline size_t version() const { return version_; }
    inline void cleanDirty() {
        std::lock_guard<std::mutex> l(m_);
        dirty_ = false;
    }

    void beginWrite() {
        m_.lock();
        this->dirty_ = true;
        this->version_++;
    }
    void endWrite() { m_.unlock(); }

   protected:
    std::mutex m_;
    size_t version_{0};
    bool dirty_{false};
};

class ClientBuffer : public Buffer {
   public:
    glm::vec3 camera_position;
};
class ServerBuffer : public Buffer {
   public:
    std::unordered_map<ChunkPos, LevelChunk> chunks;
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
