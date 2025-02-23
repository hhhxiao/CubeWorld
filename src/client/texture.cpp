//
// Created by xhy on 2024/5/3.
//

#include "texture.h"
#include <float.h>
#include <algorithm>
#include <array>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include "block.h"
#include "image.h"
#include "utils.h"
#include "magic_enum/magic_enum.hpp"
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

namespace {
    bool load_cumbe_map(const fs::path &path, GLuint *textureId) {
        std::vector<fs::path> paths;
        paths.resize(6);
        for (const auto &entry : fs::directory_iterator(path)) {
            if (!entry.is_regular_file()) continue;
            auto ext = entry.path().extension();
            if (ext != ".jpg" && ext != ".png") continue;
            auto name = entry.path().stem().string();
            if (name.find("right") != std::string::npos) paths[0] = entry.path();
            if (name.find("left") != std::string::npos) paths[1] = entry.path();
            if (name.find("top") != std::string::npos) paths[2] = entry.path();
            if (name.find("bottom") != std::string::npos) paths[3] = entry.path();
            if (name.find("front") != std::string::npos) paths[4] = entry.path();
            if (name.find("back") != std::string::npos) paths[5] = entry.path();
        }
        if (std::count_if(paths.begin(), paths.end(), [](const fs::path &p) { return p.string().empty(); }) > 0) {
            LE("The cube map texture in dir %s is less than 6.", path.string().c_str());
            return false;
        }
        // load
        glGenTextures(1, textureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *textureId);
        for (GLuint i = 0; i < paths.size(); i++) {
            int width, height, nrChannels;
            unsigned char *data = stbi_load(paths[i].string().c_str(), &width, &height, &nrChannels, 0);
            if (!data) {
                LE("Can not load texture %s", path.string().c_str());
                stbi_image_free(data);
                return false;
            }
            LD(" - Load image %s", paths[i].string().c_str());
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0,
                         nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        return true;
    }

    bool parse_block_texture(const std::string &stem, BlockType &type, std::vector<Face> &faces) {
        std::unordered_map<std::string, BlockType> block_type_map;
        for (const auto &entry : magic_enum::enum_entries<BlockType>()) {
            block_type_map[std::string(entry.second)] = entry.first;
        }

        auto p = stem.find("_");
        auto name = p == std::string::npos ? stem : stem.substr(0, p);
        if (block_type_map.count(name) == 0) {
            return false;
        }
        type = block_type_map[name];
        if (p == std::string::npos) {
            faces = {nx, px, ny, py, nz, pz};
            return true;
        } else {
            auto suffix = stem.substr(p + 1);
            if (suffix == "side") {
                faces = {nx, nz, px, pz};
            } else if (suffix == "nx") {
                faces = {nx};
            } else if (suffix == "px") {
                faces = {px};
            } else if (suffix == "ny" || suffix == "buttom") {
                faces = {ny};
            } else if (suffix == "py" || suffix == "top") {
                faces = {py};
            } else if (suffix == "nz") {
                faces = {nz};
            } else if (suffix == "pz") {
                faces = {pz};
            } else {
                return false;
            }
        }
        return true;
    }

}  // namespace

void TextureManager::loadCubeMaps(const fs::path &path) {
    LD("Cube map path: %s", path.string().c_str());
    for (const auto &entry : fs::directory_iterator(path)) {
        GLuint cubeMapId;
        auto name = entry.path().filename().string();
        if (load_cumbe_map(entry.path(), &cubeMapId)) {
            this->cubemap_ids_[name] = cubeMapId;
        } else {
            LE("Can not load cube map %s", name.c_str());
        }
    }
}

void BlockTextureAtlas::init(const fs::path &path) {
    LD("Block Atlas root path: %s", path.string().c_str());

    auto *texture = new Image(P_TEX_SIZE * ATLAS_WIDTH, P_TEX_SIZE * ATLAS_HEIGHT, 4);

    // find all paths (for pri)
    std::array<std::vector<fs::path>, 3> paths{};
    for (const auto &entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;
        auto ext = entry.path().extension();
        if (ext != ".jpg" && ext != ".png") continue;
        auto stem = entry.path().stem().string();
        auto p = stem.find("_");
        if (p == std::string::npos) {
            paths[0].push_back(entry.path());
        } else {
            auto suffix = stem.substr(p + 1);
            if (suffix == "side") {
                paths[1].push_back(entry.path());
            } else {
                auto faceNames = {"nx", "px", "ny", "py", "nz", "pz", "top", "buttom"};
                if (std::find(faceNames.begin(), faceNames.end(), suffix) == faceNames.end()) {
                    LW("Invalid texture %s", path.string().c_str());
                    continue;
                } else {
                    paths[2].push_back(entry.path());
                }
            }
        }
    }

    // // load valid textures
    int width, height, nrChannels;
    int x{0};
    int y{0};
    for (int i = 0; i < 3; i++) {
        auto pathList = paths[i];
        for (auto &path : pathList) {
            // parse block info
            BlockType type;
            std::vector<Face> faces;
            if (!parse_block_texture(path.stem().string(), type, faces)) {
                LW("Invalid texture %s", path.string().c_str());
                continue;
            }
            // load img
            LD("Process %s", path.string().c_str());
            auto *data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, 0);
            if (!data) {
                LE("Can not load texture %s", path.string().c_str());
                stbi_image_free(data);
                continue;
            }
            if (width != TEX_SIZE || height != TEX_SIZE) {
                LD("Invalid texture size: %s", path.string().c_str());
                stbi_image_free(data);
                continue;
            }
            Image img(width, height, nrChannels, data, false);
            if (x < ATLAS_WIDTH && y < ATLAS_HEIGHT) {
                texture->buildAtlas(x * P_TEX_SIZE, y * P_TEX_SIZE, img, PADDING);
                auto u = static_cast<float>(x * P_TEX_SIZE + PADDING) / (P_TEX_SIZE * ATLAS_WIDTH);
                auto v = static_cast<float>(y * P_TEX_SIZE + PADDING) / (P_TEX_SIZE * ATLAS_HEIGHT);
                for (auto &face : faces) {
                    auto &info = this->atlas_table_[static_cast<BlockType>(type)][face];
                    info.u = u;
                    info.v = v;
                    info.type = type;
                }

                x++;
                if (x >= ATLAS_WIDTH) {
                    x = 0;
                    y++;
                }
            } else {
                LE("The atlas is too small, some block texture can not be loaded.");
            }
            stbi_image_free(data);
        }
    }
    texture->save("atlas.bmp");  // for debug

    for (auto &bs : atlas_table_) {
        auto b = bs[0];
        LD("Block %d: uv = (%.2f,%.2f)", b.type, b.u, b.v);
    }

    // // gen id
    glGenTextures(1, &this->id_);
    glBindTexture(GL_TEXTURE_2D, this->id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width(), texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 texture->data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, 0);
    delete texture;
}

TextureManager &TextureManager::instance() {
    static TextureManager mgr;
    return mgr;
}

void SSAONoiseTexture::init() {
    LD("Init ssao texture");
    auto ourLerp = [](float a, float b, float f) { return a + f * (b - a); };

    // from https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/5.advanced_lighting/9.ssao/ssao.cpp
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);  // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i) {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,
                         randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = ourLerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssao_kernel_.push_back(sample);
    }
    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,
                        0.0f);  // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
}
