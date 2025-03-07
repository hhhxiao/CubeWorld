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
#include "chunk_renderer.h"

namespace fs = std::filesystem;

class BlockTextureAtlas {
   public:
    static constexpr auto TEX_SIZE = 16;
    static constexpr auto PADDING = 2;
    static constexpr auto P_TEX_SIZE = TEX_SIZE + PADDING * 2;
    static constexpr auto ATLAS_WIDTH = 4;
    static constexpr auto ATLAS_HEIGHT = 4;
    static constexpr float U_SCALUE = TEX_SIZE * 1.f / (ATLAS_WIDTH * P_TEX_SIZE);
    static constexpr float V_SCALUE = TEX_SIZE * 1.f / (ATLAS_HEIGHT * P_TEX_SIZE);
    BlockTextureAtlas() {}
    struct AtlasInfo {
        BlockType type{invalid};
        float u{0};
        float v{0};
    };

   public:
    void init(const fs::path& path);
    inline GLuint id() const { return id_; }
    inline std::tuple<float, float> uv(BlockType type, Face face) {
        auto i = atlas_table_[type][face];
        return {i.u, i.v};
    }
    inline void bind() { glBindTexture(GL_TEXTURE_2D, this->id_); }

   private:
    GLuint id_{0};
    std::array<std::array<AtlasInfo, 6>, static_cast<size_t>(BlockType::invalid + 1)> atlas_table_;
};

class SSAONoiseTexture {
   public:
    void init();
    GLuint id() { return id_; }
    const std::vector<glm::vec3>& kernel() { return ssao_kernel_; }

   private:
    GLuint id_{0};
    std::vector<glm::vec3> ssao_kernel_;
};

class TextureManager {
   public:
    static TextureManager& instance();
    inline BlockTextureAtlas& blockAtlas() { return block_alas_; }
    inline SSAONoiseTexture& ssaoNoiseTexture() { return ssao_noise_; }
    inline void init(const fs::path& path) {
        block_alas_.init(path / "blocks");
        loadCubeMaps(path / "cubemaps");
        ssao_noise_.init();
    }

    inline GLuint getCubeMapID(const std::string& name) { return cubemap_ids_[name]; }

   private:
    void loadCubeMaps(const fs::path& path);
    std::unordered_map<std::string, GLuint> cubemap_ids_;
    BlockTextureAtlas block_alas_;
    SSAONoiseTexture ssao_noise_;
};

#endif  // LEARNOPENGL_TEXTURE_H
