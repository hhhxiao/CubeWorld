#include "buffer.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>
#include "config.h"
#include "utils.h"
#include "glad/glad.h"

void ChunkBuffer::init(size_t chunk_num) {
    int max_bucket_count = static_cast<uint32_t>(chunk_num * (Config::CHUNK_HEIGHT << 8) / BUCKET_SIZE);
    solid_buffer_.resize(max_bucket_count * BUCKET_SIZE);
    for (int i = 0; i < max_bucket_count; i++) {
        unused_.insert(i);
    }
    LD("Chunk num is %zu, Max bucket count is %d, total store %zu vertices", chunk_num, max_bucket_count,
       solid_buffer_.size());
    LD("Memory usgae: %.3lf MiB", static_cast<double>(solid_buffer_.size() * sizeof(Vert)) / (1024.0 * 1024.0));
    glGenBuffers(1, &vbo_solid_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_solid_);
    glBufferData(GL_ARRAY_BUFFER, solid_buffer_.size() * sizeof(Vert), solid_buffer_.data(), GL_STATIC_DRAW);
}
ChunkBuffer::~ChunkBuffer() { glDeleteBuffers(1, &vbo_solid_); }
// assign a buffer to chunk for vertices
ChunkBuffer::BucketInstance ChunkBuffer::assgin(const ChunkPos& pos) {
    if (unused_.empty()) {
        LE("Can not assign bucket to chunk %s\n", pos.toString().c_str());
        return {0, nullptr, 0};
    }
    auto index = *unused_.begin();
    unused_.erase(index);
    used_.insert(index);
    return {index, &solid_buffer_[index * BUCKET_SIZE], 0};
}

void ChunkBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_solid_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), reinterpret_cast<void*>(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
}

// reclaim the buffer
void ChunkBuffer::reclaim(const ChunkPos& pos, size_t index) {
    unused_.insert(index);
    used_.erase(index);
}

void ChunkBuffer::rebufferBucket(const BucketInstance& inst) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_solid_);
    glBufferSubData(GL_ARRAY_BUFFER, inst.index * sizeof(Vert) * BUCKET_SIZE, inst.size * sizeof(Vert), inst.bucket);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}