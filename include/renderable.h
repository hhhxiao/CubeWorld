//
// Created by xhy on 2024/5/3.
//
#pragma once

#include <string>
#include <vector>
#include "glad/glad.h"
#include "glm/detail/type_vec.hpp"
#include "render_context.h"
#include "config.h"
#include "utils.h"

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
    ~Renderable();
    void init();
    virtual void render(RenderContext &ctx) = 0;
    inline bool empty() { return indices_.empty(); }

   protected:
    virtual void sendData() const;
    void enableAttr() const;
    GLuint VBO, EBO{0};
    std::vector<VertexAttribute> vertices_;
    std::vector<GLuint> indices_;
    std::string shader_name_;
};

class Light : public Renderable {
   public:
    Light() : Renderable() {
        float vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
            0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

            -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
            0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
            0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

            -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
        };

        auto sz = sizeof(vertices) / sizeof(float);
        for (auto i = 0; i < sz; i += 3) {
            VertexAttribute attribute{
                vertices[i], vertices[i + 1], vertices[i + 2], 1.0f, 1.0f, 1.0f, 0.0, 0.0, 0.0, 0.0, 0.0};
            vertices_.push_back(attribute);
        }
    }
    void render(RenderContext &ctx) override {
        auto &shader = ctx.shader();
        shader.use(shader_name_);
        ctx.shader().setMat4("projection", Config::getProjectionMatrix());
        ctx.shader().setMat4("view", ctx.camera().getViewMatrix());
        glm::mat4 model;
        model = glm::translate(model, pos_);
        shader.setMat4("model", model);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        enableAttr();
        if (indices_.size() > 0) {
            glDrawElements(GL_TRIANGLES, (GLint)this->indices_.size(), GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    auto position() const { return pos_; }

   private:
    glm::vec3 pos_{0, 120, 0};
};