//
// Created by xhy on 2024/5/3.
//

#include "drawable_object.h"
#include <vector>

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
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void *)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // 取消绑定缓冲区和当前对象
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

DrawableObject DrawableObject::createCube() {
    std::vector<VertexAttribute> vas;
    for (int x = 0; x <= 1; x++) {
        for (int y = 0; y <= 1; y++) {
            for (int z = 0; z <= 1; z++) {
                VertexAttribute attribute{x * 1.0f, y * 1.0f, z * 1.0f, x * 1.0f, y * 1.0f, z * 1.0f};
                vas.push_back(attribute);
            }
        }
    }

    std::vector<GLuint> indices{
        0, 1, 2, 1, 2, 3,  // x0
        4, 5, 6, 5, 6, 7,  // x1
        2, 3, 6, 3, 6, 7,  // y0
        0, 1, 4, 1, 4, 5,  // y1
        0, 2, 4, 2, 4, 6,  // z0
        1, 3, 5, 3, 5, 7   // z1
    };
    return DrawableObject(vas, indices);
}
