//
// Created by xhy on 2024/5/3.
//
#pragma once

#include <vector>
#include "glad/glad.h"

struct VertexAttribute {
    // 坐标
    GLfloat x;
    GLfloat y;
    GLfloat z;
    // uv
    GLfloat u;
    GLfloat v;
    // 法线
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;
};
static_assert(sizeof(VertexAttribute) == sizeof(GLfloat) * 8, "Error in vertexAttribute strcuture");

class DrawableObject {
   public:
    DrawableObject(const std::vector<VertexAttribute> &vertices, const std::vector<GLuint> &indices)
        : vertices_(vertices), indices_(indices) {}

    void init();

    void draw();

   private:
    void sendData() const;

   private:
    GLuint VAO{0};
    std::vector<VertexAttribute> vertices_;
    const std::vector<GLuint> indices_;
};
