#ifndef LEVEL_MESH_H
#define LEVEL_MESH_H
#include "glad/glad.h"
#include "glm/detail/type_vec.hpp"
#include "position.h"
#include <cstddef>
#include <vector>
#include "block.h"

class LevelRenderer;
class RenderContext;
class ChunkMesh {
    enum MeshStatus { Init = 0, HashGenBuffer, HasBuildMesh, HasBufferData };
    static constexpr auto GRANULARYTY = 32;
    friend class ChunkMeshMgr;
    struct V {
        // 坐标
        GLfloat x;
        GLfloat y;
        GLfloat z;
        // 颜色
        GLfloat r;
        GLfloat g;
        GLfloat b;
        // uv
        GLfloat u;
        GLfloat v;
        // 法线
        GLfloat nx{0.0};
        GLfloat ny{0.0};
        GLfloat nz{0.0};
        void setUV(GLfloat u, GLfloat v) {
            this->u = u;
            this->v = v;
        }
        void setNormal(GLfloat nx, GLfloat ny, GLfloat nz) {
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
        }
        static V fromPos(const glm::vec3& pos) { return {pos.x, pos.y, pos.z}; }
    };

   public:
    explicit ChunkMesh(const ChunkPos& pos) : VBO(0), pos_(pos) {}

    ~ChunkMesh();

   private:
    void genBuffer();
    void buildMesh(LevelRenderer* level);
    void bufferData();

   public:
    bool stepInit(LevelRenderer* level);
    void render(RenderContext& ctx);
    bool hasInited() const { return status_ == HasBufferData; };

    [[nodiscard]] ChunkPos pos() const { return pos_; }

   private:
    void addFace(const BlockFaceInfo& info);

   private:
    GLuint VBO;
    std::vector<ChunkMesh::V> vertices_;
    // textureID -> vert begin vert count
    std::unordered_map<GLuint, std::pair<size_t, size_t>> texture_mappings_;
    ChunkPos pos_;
    std::vector<BlockFaceInfo> fis_;
    //
    int current_build_height_{0};
    MeshStatus status_{ChunkMesh::MeshStatus::Init};
};

#endif
