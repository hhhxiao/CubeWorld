#ifndef BUFFER_H
#define BUFFER_H
#include <cstddef>
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
    Vert() = default;
    Vert(const glm::vec3& pos, const glm::vec4& color, const glm::vec3& normal) {
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
    };
};

class ChunkBuffer {
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

    void bind();

    void unbind();

    void enableVertexAttribArray();

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
    GLuint vbo_{0};
};

#endif
