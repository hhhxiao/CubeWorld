#include "subchunk_mesh.h"
#include <vector>
#include "block.h"
#include "drawable_object.h"
#include "texture.h"

std::vector<int> createFaceVertices(Face face) {
    if (face == px) return {1, 3, 5, 7};
    if (face == nx) return {0, 2, 4, 6};
    if (face == py) return {4, 5, 6, 7};
    if (face == ny) return {0, 1, 2, 3};
    if (face == pz) return {2, 3, 6, 7};
    if (face == nz) return {0, 1, 4, 5};
    throw std::exception("error in create Face indices");
}

void SubChunkMesh::AddFace(const BlockFaceInfo &face) {
    auto pos = face.pos;
    auto idx = pos.y * 256 + pos.z * 16 + pos.x;
    this->blocks_[idx][face.face] = face;
}

void SubChunkMesh::buildData() {
    for (auto &block : this->blocks_) {  // 遍历每一个面
        for (auto block_face : block.second) {
            auto face = static_cast<Face>(block_face.first);
            auto face_info = block_face.second;
            auto vs = createFaceVertices(face);
            // 4个顶点
            for (int i = 0; i < vs.size(); i++) {
                auto U = 0.0f, V = 0.0f;
                if (i == 1) V = 1.0f;
                if (i == 2) U = 1.0f;
                if (i == 3) {
                    U = 1.0f, V = 1.0f;
                }
                auto vi = vs[i];
                auto dx = static_cast<GLfloat>(vi % 2);
                auto dy = static_cast<GLfloat>(vi >= 4 ? 1 : 0);
                auto dz = static_cast<GLfloat>((vi % 4) >= 2 ? 1 : 0);
                VertexAttribute va{static_cast<GLfloat>(face_info.pos.x + dx),
                                   static_cast<GLfloat>(face_info.pos.y + dy),
                                   static_cast<GLfloat>(face_info.pos.z + dz),
                                   dx,
                                   dy,
                                   dz,
                                   U,
                                   V,
                                   0.0f,
                                   0.0f,
                                   0.0f};
                this->vertices_.push_back(va);
            }
            auto size = (GLuint)vertices_.size();
            this->indices_.push_back(size - 4);
            this->indices_.push_back(size - 3);
            this->indices_.push_back(size - 2);
            this->indices_.push_back(size - 3);
            this->indices_.push_back(size - 2);
            this->indices_.push_back(size - 1);
            this->texuture_ids_.push_back({block_face.second.type, block_face.second.face});
        }
        // 每个面构建4个VertexAttribute(wwl1)
    }
};

void SubChunkMesh::sendData() {
    glGenVertexArrays(1, &this->VAO);
    // 绑定当前对象
    glBindVertexArray(this->VAO);
    // 创建缓冲区
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // 发送顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLuint)(vertices_.size() * sizeof(VertexAttribute)), vertices_.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLuint)(sizeof(GLuint) * indices_.size()), indices_.data(), GL_STATIC_DRAW);

    // 设置数据格式
    // 顶点位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    // 顶点颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 顶点纹理
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // 顶点法线
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // 取消绑定缓冲区和当前对象
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // LOGGER("VAO = %u", VAO);
}

void SubChunkMesh::draw(TexturePool *pool) {
    glBindVertexArray(this->VAO);
    auto faces_number = this->indices_.size() / 6;
    for (int i = 0; i < faces_number; i++) {
        glBindTexture(GL_TEXTURE_2D, pool->getTextureID(this->texuture_ids_[i].first, this->texuture_ids_[i].second));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)((i * 6) * sizeof(GLuint)));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)((i * 6 + 3) * sizeof(GLuint)));
    }
}
