//
// Created by xhy on 2024/5/3.
//
#pragma once

#include <string>
#include <vector>
#include "glad/glad.h"
#include "render_context.h"

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

class Renderable {
   public:
    // full
    Renderable(const std::vector<VertexAttribute> &vertices, const std::vector<GLuint> &indices,
               const std::string &name)
        : vertices_(vertices), indices_(indices), shader_name_(name) {}
    // with base shader
    Renderable(const std::vector<VertexAttribute> &vertices, const std::vector<GLuint> &indices)
        : Renderable(vertices, indices, "base") {}
    // empty with shader only
    Renderable(const std::string &name) : Renderable({}, {}, name) {}
    // empty with base shader
    Renderable() : Renderable("base") {}
    void init();
    virtual void render(RenderContext &ctx) = 0;
    inline bool empty() { return indices_.empty(); }

   protected:
    virtual void sendData() const;
    GLuint VAO{0};
    std::vector<VertexAttribute> vertices_;
    std::vector<GLuint> indices_;
    std::string shader_name_;
};
