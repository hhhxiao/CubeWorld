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
    // 颜色
    GLfloat r;
    GLfloat g;
    GLfloat b;
    // uv
    GLfloat u;
    GLfloat v;
    // 法线
    GLfloat nx;
    GLfloat ny;
    GLfloat nz;
};

static_assert(sizeof(VertexAttribute) == sizeof(GLfloat) * 11, "Error in vertexAttribute strcuture");

class DrawableObject {
   public:
    DrawableObject() = default;
    DrawableObject(const std::vector<VertexAttribute> &vertices, const std::vector<GLuint> &indices)
        : vertices_(vertices), indices_(indices) {}

    void init();

    void draw();

    inline bool empty() { return indices_.empty(); }

   public:
    static DrawableObject createCube();

   private:
    void sendData() const;

   private:
    GLuint VAO{0};
    std::vector<VertexAttribute> vertices_;
    std::vector<GLuint> indices_;
};
