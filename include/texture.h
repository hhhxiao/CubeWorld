//
// Created by xhy on 2024/5/3.
//

#ifndef LEARNOPENGL_TEXTURE_H
#define LEARNOPENGL_TEXTURE_H

#include <glad/glad.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include "block.h"
#include "subchunk_mesh.h"

// 简单的实现，没有缓存
class TexturePool {
   public:
    static TexturePool& instance();

   public:
    GLuint getTextureID(BlockType type, Face face);
    void init(const std::filesystem::path& path);

   private:
    std::unordered_map<BlockType, std::unordered_map<Face, GLuint>> texture_ids_;
};

// class Texture2D {
//    public:
//     bool load(const std::string &path);

//     inline GLuint id() const { return textureID; }

//    private:
//     GLuint textureID;
// };

#endif  // LEARNOPENGL_TEXTURE_H
