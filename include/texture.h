//
// Created by xhy on 2024/5/3.
//

#ifndef LEARNOPENGL_TEXTURE_H
#define LEARNOPENGL_TEXTURE_H

#include <glad/glad.h>

#include <array>
#include <cstddef>
#include <filesystem>
#include <string>
#include <unordered_map>
#include "block.h"
#include "image.h"

namespace fs = std::filesystem;

class BlockTextureAtlas {
    static constexpr auto BLOCK_TEXTURE_SIZE = 16;
    static constexpr auto ATLAS_WIDTH = 4;
    static constexpr auto ATLAS_HEIGHT = 4;

    struct AtlasInfo {
        BlockType type;
        float u;
        float v;
    };

   public:
    void init(const fs::path& path);
    void save();

   private:
    GLuint id{0};
    std::array<std::array<AtlasInfo, 6>, static_cast<size_t>(BlockType::invalid)> atlas_table_;
};

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
    void loadBlockTexture(const fs::path& path);
    void loadCubeMaps(const fs::path& path);

   private:
    std::unordered_map<std::string, GLuint> cubemap_ids_;
    // block faceid textureID
    std::unordered_map<BlockType, std::unordered_map<Face, GLuint>> texture_ids_;
};

#endif  // LEARNOPENGL_TEXTURE_H
