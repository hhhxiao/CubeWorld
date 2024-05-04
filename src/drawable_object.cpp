//
// Created by xhy on 2024/5/3.
//

#include "drawable_object.h"

void DrawableObject::draw() {
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, (GLint)this->indices_.size(), GL_UNSIGNED_INT, 0);
}

void DrawableObject::init() {
    glGenVertexArrays(1, &this->VAO);
    this->sendData();
}

void DrawableObject::sendData() const {
    if (this->VAO == 0) return;
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
    // 顶点坐标
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)3);
    glEnableVertexAttribArray(0);
    // 法线坐标
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 取消绑定缓冲区和当前对象
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
