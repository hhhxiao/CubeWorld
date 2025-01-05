//
// Created by xhy on 2024/5/3.
//

#include "texture.h"
#include <algorithm>
#include <array>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "block.h"
#include "utils.h"
#include "magic_enum/magic_enum.hpp"
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

namespace {
    bool load_texture(const std::filesystem::path &path, GLuint *texture_id) {
        int width, height, nrChannels;
        unsigned char *data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            LE("Can not load texture %s", path.string().c_str());
            stbi_image_free(data);
            return false;
        }
        glGenTextures(1, texture_id);
        glBindTexture(GL_TEXTURE_2D, *texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        LD("texture: %s => %d channels", path.string().c_str(), nrChannels);

        glTexImage2D(GL_TEXTURE_2D, 0, nrChannels == 3 ? GL_RGB : GL_RGBA, width, height, 0,
                     nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        stbi_image_free(data);
        return true;
    }

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

GLuint TexturePool::getTextureID(BlockType type, Face face) {
    auto it = this->texture_ids_.find(type);
    if (it == this->texture_ids_.end()) {
        return getTextureID(invalid, nx);
    }
    auto it2 = this->texture_ids_[type].find(face);
    if (it2 == this->texture_ids_[type].end()) {
        return getTextureID(invalid, nx);
    }
    return it2->second;
}

GLuint TexturePool::getCubeMapID(const std::string &name) {
    auto id = this->cubemap_ids_.find(name);
    if (id == this->cubemap_ids_.end()) {
        LE("Can not found cubemap texture %s", name.c_str());
        throw std::runtime_error("Can not find cube map texture" + name);
    }
    return id->second;
}

void TexturePool::init(const std::filesystem::path &path) {
    LD("Texture root path: %s", path.string().c_str());

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

    for (int i = 0; i < 3; i++) {
        auto pathList = paths[i];
        for (auto &path : pathList) {
            BlockType type;
            std::vector<Face> faces;
            if (!parse_block_texture(path.stem().string(), type, faces)) {
                LW("Invalid texture %s", path.string().c_str());
                continue;
            }
            GLuint textureId;
            if (load_texture(path, &textureId)) {
                for (const auto &face : faces) {
                    this->texture_ids_[type][face] = textureId;
                }
            } else {
                LE("Can not load texture %s", path.string().c_str());
            }
        }
    }
    loadCubeMaps(path / "cubemaps");
}

void TexturePool::loadCubeMaps(const fs::path &path) {
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
