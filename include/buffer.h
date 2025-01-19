#ifndef BUFFER_H
#define BUFFER_H
#include <cstddef>
#include <vector>
#include <set>
#include "config.h"
#include "glad/glad.h"
#include "position.h"
struct Vert {
    GLfloat x{0};
    GLfloat y{0};
    GLfloat z{0};
    static Vert fromPos(const glm::vec3& pos) {
        Vert v;
        v.x = pos.x;
        v.y = pos.y;
        v.z = pos.z;
        return v;
    }
};

class ChunkBuffer {
   public:
    static constexpr auto BUCKET_SIZE = (256) * 24;
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

    void bind();

    ~ChunkBuffer();
    // assign a bucket to chunk for vertices
    BucketInstance assgin(const ChunkPos& pps);
    // reclaim a buckect
    void reclaim(const ChunkPos& pos, size_t index);
    //
    void rebufferBucket(const BucketInstance& inst);

   private:
    size_t max_chunks_num_{0};
    std::vector<Vert> solid_buffer_;
    //    std::vector<VertBucket> transparent_buffer_;
    std::set<size_t> used_, unused_;
    GLuint vbo_solid_{0};
};

#endif
