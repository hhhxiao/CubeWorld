#ifndef BUFFER_H
#define BUFFER_H
#include <cstddef>
#include <string>
#include <vector>
#include <set>
#include "config.h"
#include "glad/glad.h"
#include "glm/detail/type_vec.hpp"
#include "position.h"

struct Vert {
    GLfloat x{0};
    GLfloat y{0};
    GLfloat z{0};
    GLfloat r{0.0};
    GLfloat g{0.0};
    GLfloat b{0.0};
    GLfloat a{1.0};
    GLfloat nx{0.0};
    GLfloat ny{0.0};
    GLfloat nz{0.0};
    GLfloat u{0.0};
    GLfloat v{0.0};
    Vert() = default;
    Vert(const glm::vec3& pos, const glm::vec4& color, const glm::vec3& normal, const glm::vec2& uv) {
        x = pos.x;
        y = pos.y;
        z = pos.z;
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
        nx = normal.x;
        ny = normal.y;
        nz = normal.z;
        u = uv.x;
        v = uv.y;
    };
};

class VertexBufferInterface {
   public:
    inline void bind() { glBindBuffer(GL_ARRAY_BUFFER, vbo_); }
    inline void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    virtual void enableVertexAttrArray() = 0;
    virtual ~VertexBufferInterface() { glDeleteBuffers(1, &vbo_); }

   protected:
    GLuint vbo_{0};
};

class ChunkBuffer : public VertexBufferInterface {
   public:
    static constexpr auto BUCKET_SIZE = (64) * 24;
    struct BucketInstance {
        size_t index{0};
        Vert* bucket{nullptr};
        size_t size{0};  // size in vert
        bool valid() { return bucket != nullptr; }
    };

   public:
    void init(size_t chunk_num);

    inline size_t unused_size() const { return unused_.size(); }

    inline size_t used_size() const { return used_.size(); }
    inline size_t size() const { return buffer_data_.size(); }
    void enableVertexAttrArray() override;
    ~ChunkBuffer();
    // assign a bucket to chunk for vertices
    BucketInstance assgin(const ChunkPos& pps);
    // reclaim a buckect
    void reclaim(const ChunkPos& pos, size_t index);
    //
    void rebufferBucket(const BucketInstance& inst);

   private:
    size_t max_chunks_num_{0};
    std::vector<Vert> buffer_data_;
    std::set<size_t> used_, unused_;
};

class QuadScreenBuffer : public VertexBufferInterface {
   public:
    inline void init() {
        glGenBuffers(1, &vbo_);
        bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(data_), &data_, GL_STATIC_DRAW);
        unbind();
    }

    void enableVertexAttrArray() override {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }
    void render() {
        bind();
        enableVertexAttrArray();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        unbind();
    }

   private:
    float data_[24] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                       -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};
};

class SkyBoxBuffer : public VertexBufferInterface {
   public:
    inline void init() {
        glGenBuffers(1, &vbo_);
        bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(data_), &data_, GL_STATIC_DRAW);
        unbind();
    }

    void enableVertexAttrArray() override {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
    void render() {
        bind();
        enableVertexAttrArray();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        unbind();
    }

   private:
    float data_[108] = {-1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
                        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
                        -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                        -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                        -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
                        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
                        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
};

class FrameBufferInterface {
   public:
    virtual void init() = 0;

    inline void bind() { glBindFramebuffer(GL_FRAMEBUFFER, this->fbo_); }
    inline void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    inline bool doCheckSuccess() { return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE; }

   protected:
    GLuint fbo_{0};
};

// sample frame buffer
class FrameBuffer : public FrameBufferInterface {
   public:
    void init() override;
    auto texture_id() { return texture_; }
    auto frame_buffer_id() { return fbo_; }

   private:
    GLuint texture_{0};
    GLuint rbo_{0};
};

class DepthMapBuffer : public FrameBufferInterface {
   public:
    void init() override;
    auto id() { return depth_map_; }

   private:
    GLuint depth_map_{0};
};

class GBuffer : public FrameBufferInterface {
   public:
    void init() override;

    GLuint position_id() { return position_; }
    GLuint normal_id() { return normal_; }
    GLuint albedo_id() { return albedo_; }
    GLuint shadow_id() { return shadow_id_; }

   private:
    GLuint position_{0};
    GLuint normal_{0};
    GLuint albedo_{0};
    GLuint shadow_id_{0};
};

class SSAOBuffer : public FrameBufferInterface {
   public:
    void init() override;
    GLuint id() { return ssao_id_; }

   private:
    GLuint ssao_id_;
};

#endif
