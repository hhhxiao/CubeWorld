#include "buffer.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "config.h"
#include "utils.h"
#include "glad/glad.h"

void ChunkBuffer::init(size_t chunk_num) {
    int max_bucket_count = static_cast<uint32_t>(chunk_num * (Config::CHUNK_HEIGHT << 8) / BUCKET_SIZE) >> 2;
    buffer_data_.resize(max_bucket_count * BUCKET_SIZE);
    for (int i = 0; i < max_bucket_count; i++) {
        unused_.insert(i);
    }
    LD("Chunk num is %zu, Max bucket count is %d, total store %zu vertices", chunk_num, max_bucket_count,
       buffer_data_.size());
    LD("Memory usgae: %.3lf MiB", static_cast<double>(buffer_data_.size() * sizeof(Vert)) / (1024.0 * 1024.0));
    glGenBuffers(1, &vbo_);
    bind();
    glBufferData(GL_ARRAY_BUFFER, buffer_data_.size() * sizeof(Vert), buffer_data_.data(), GL_STATIC_DRAW);
    unbind();
}
ChunkBuffer::~ChunkBuffer() {}
// assign a buffer to chunk for vertices
ChunkBuffer::BucketInstance ChunkBuffer::assgin(const ChunkPos& pos) {
    if (unused_.empty()) {
        //  LE("Can not assign bucket to chunk %s\n", pos.toString().c_str());
        return {0, nullptr, 0};
    }
    auto index = *unused_.begin();
    unused_.erase(index);
    used_.insert(index);
    return {index, &buffer_data_[index * BUCKET_SIZE], 0};
}

void ChunkBuffer::enableVertexAttrArray() {
    // location n_ele _ _  offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(7 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(10 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);
}

// reclaim the buffer
void ChunkBuffer::reclaim(const ChunkPos& pos, size_t index) {
    unused_.insert(index);
    used_.erase(index);
}

void ChunkBuffer::rebufferBucket(const BucketInstance& inst) {
    glBufferSubData(GL_ARRAY_BUFFER, inst.index * sizeof(Vert) * BUCKET_SIZE, inst.size * sizeof(Vert), inst.bucket);
}

void FrameBuffer::init() {
    glGenFramebuffers(1, &fbo_);
    bind();
    // create a color attachment texture
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Config::window_width, Config::window_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Config::window_width, Config::window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    if (!doCheckSuccess()) {
        LW("Frame buffer is not complete.");
    } else {
        LD("Frame buffer created.");
    }
    unbind();
}
void DepthMap::init() {
    glGenFramebuffers(1, &fbo_);
    bind();
    // create depth texture
    glGenTextures(1, &depth_map_);
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_, 0);
    glDrawBuffer(GL_NONE);
    if (!doCheckSuccess()) {
        LW("Create depth map failed.");
    } else {
        LD("Depth map created.");
    }
    unbind();
}