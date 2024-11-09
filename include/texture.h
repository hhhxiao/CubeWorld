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

namespace fs = std::filesystem;

// 简单的实现，没有缓存
class TexturePool {
   public:
    inline static TexturePool& instance() {
        static TexturePool pool;
        return pool;
    }

    GLuint getTextureID(BlockType type, Face face);

    GLuint getCubeMapID(const std::string& name);

    void init(const fs::path& path);

   private:
    void loadCubeMaps(const fs::path& path);

   private:
    std::unordered_map<std::string, GLuint> cubemap_ids_;
    // block faceid textureID
    std::unordered_map<BlockType, std::unordered_map<Face, GLuint>> texture_ids_;
};

#endif  // LEARNOPENGL_TEXTURE_H
