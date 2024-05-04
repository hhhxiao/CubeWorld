//
// Created by xhy on 2024/5/3.
//

#ifndef LEARNOPENGL_TEXTURE_H
#define LEARNOPENGL_TEXTURE_H

#include <glad/glad.h>

#include <string>

class Texture2D {
   public:
    bool load(const std::string &path);

    inline GLuint id() const { return textureID; }

   private:
    GLuint textureID;
};

#endif  // LEARNOPENGL_TEXTURE_H
