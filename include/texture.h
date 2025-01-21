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
#include <tuple>
#include <unordered_map>
#include "block.h"

namespace fs = std::filesystem;

class BlockTextureAtlas {
   public:
    static constexpr auto BLOCK_TEXTURE_SIZE = 16;
    static constexpr auto ATLAS_WIDTH = 4;
    static constexpr auto ATLAS_HEIGHT = 4;
    static constexpr float U_SCALUE = 1.f / ATLAS_WIDTH;
    static constexpr float V_SCALUE = 1.f / ATLAS_HEIGHT;

    BlockTextureAtlas() {}
    struct AtlasInfo {
        BlockType type{invalid};
        float u{0};
        float v{0};
    };

   public:
    void init(const fs::path& path);

    inline std::tuple<float, float> uv(BlockType type, Face face) {
        auto i = atlas_table_[type][face];
        return {i.u, i.v};
    }
    inline void bind() { glBindTexture(GL_TEXTURE_2D, this->id); }

   private:
    GLuint id{0};
    std::array<std::array<AtlasInfo, 6>, static_cast<size_t>(BlockType::invalid + 1)> atlas_table_;
};

class TextureManager {
   public:
    static TextureManager& instance();
    inline BlockTextureAtlas& blockAtlas() { return block_alas_; }
    inline void init(const fs::path& path) { block_alas_.init(path / "blocks"); }

   private:
    BlockTextureAtlas block_alas_;
};

// 简单的实现，没有缓存
// class TexturePool {
//    public:
//     inline static TexturePool& instance() {
//         static TexturePool pool;
//         return pool;
//     }

//     GLuint getTextureID(BlockType type, Face face);

//     GLuint getCubeMapID(const std::string& name);

//     void init(const fs::path& path);

//    private:
//     void loadBlockTexture(const fs::path& path);
//     void loadCubeMaps(const fs::path& path);

//    private:
//     std::unordered_map<std::string, GLuint> cubemap_ids_;
//     // block faceid textureID
//     std::unordered_map<BlockType, std::unordered_map<Face, GLuint>> texture_ids_;
// };

#endif  // LEARNOPENGL_TEXTURE_H
