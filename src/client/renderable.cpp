//
// Created by xhy on 2024/5/3.
//

#include "renderable.h"
#include <vector>

void Renderable::init() {
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    sendData();
}

Renderable::~Renderable() { glDeleteBuffers(1, &VBO); }

void Renderable::sendData() const {
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
}