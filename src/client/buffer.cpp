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
void DepthMapBuffer::init() {
    glGenFramebuffers(1, &fbo_);
    bind();
    // create depth texture
    glGenTextures(1, &depth_map_);
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
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

void GBuffer::init() {
    glGenFramebuffers(1, &fbo_);
    bind();
    // position
    glGenTextures(1, &position_);
    glBindTexture(GL_TEXTURE_2D, position_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Config::window_width, Config::window_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position_, 0);

    // normal
    glGenTextures(1, &normal_);
    glBindTexture(GL_TEXTURE_2D, normal_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Config::window_width, Config::window_height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_, 0);

    // - color + spec
    glGenTextures(1, &albedo_spec_);
    glBindTexture(GL_TEXTURE_2D, albedo_spec_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Config::window_width, Config::window_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedo_spec_, 0);

    GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);
    // // depth
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Config::window_width, Config::window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (!doCheckSuccess()) {
        LW("Create G-buffer failed.");
    } else {
        LD("G-buffer created.");
    }
    unbind();
}