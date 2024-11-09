//
// Created by xhy on 2024/5/3.
//

#include "renderable.h"
#include <vector>
#include "utils.h"

void Renderable::init() {
    glGenVertexArrays(1, &this->VAO);
    sendData();
}

void Renderable::sendData() const {
    if (this->VAO == 0) return;
    LOGGER("vecs: %zu, indices: %zu", vertices_.size(), indices_.size());
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
}
