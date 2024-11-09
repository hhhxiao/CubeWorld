//
// Created by xhy on 2024/5/3.
//

#include "texture.h"
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "block.h"
#include "subchunk_mesh.h"
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
            LOGGER("Can not load texture %s", path.c_str());
            stbi_image_free(data);
            return false;
        }
        glGenTextures(1, texture_id);
        glBindTexture(GL_TEXTURE_2D, *texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        return true;
    }

    bool load_cumbe_map(const fs::path &path, GLuint *textureId) {
        std::vector<fs::path> paths;
        for (const auto &entry : fs::directory_iterator(path)) {
            if (!entry.is_regular_file()) continue;
            auto ext = entry.path().extension();
            if (ext != ".jpg" && ext != ".png") continue;
            paths.emplace_back(entry.path());
        }
        if (paths.size() < 6) {
            ERROR("The cube map texture in dir %s is less than 6.", path.string().c_str());
            return false;
        }
        std::sort(paths.begin(), paths.end());
        paths.resize(6);

        // load
        glGenTextures(1, textureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *textureId);
        for (GLuint i = 0; i < paths.size(); i++) {
            int width, height, nrChannels;
            unsigned char *data = stbi_load(paths[i].string().c_str(), &width, &height, &nrChannels, 0);
            if (!data) {
                LOGGER("Can not load texture %s", path.string().c_str());
                stbi_image_free(data);
                return false;
            }
            LOGGER(" - Load image %s", paths[i].string().c_str());
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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
        ERROR("Can not found cubemap texture %s", name.c_str());
        throw std::runtime_error("Can not find cube map texture" + name);
    }
    return id->second;
}

void TexturePool::init(const std::filesystem::path &path) {
    LOGGER("Texture root path: %s", path.string().c_str());
    std::unordered_map<std::string, BlockType> block_type_map;
    for (const auto &entry : magic_enum::enum_entries<BlockType>()) {
        block_type_map[std::string(entry.second)] = entry.first;
    }
    LOGGER("Block type map:");
    for (auto &kv : block_type_map) {
        LOGGER(" - %s -> %d", kv.first.c_str(), kv.second);
    }

    for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file() && (entry.path().extension() == ".jpg" || entry.path().extension() == ".png")) {
            auto filename = entry.path().stem().string();
            if (filename.find("_") == std::string::npos) {
                if (block_type_map.count(filename)) {
                    auto block_id = block_type_map[filename];
                    LOGGER("%d -> %s", block_id, entry.path().filename().string().c_str());
                    GLuint textureId;
                    if (load_texture(entry.path(), &textureId)) {
                        for (const auto &kv : magic_enum::enum_entries<Face>()) {
                            this->texture_ids_[block_id][kv.first] = textureId;
                        }
                    }
                }
            }
        }
    }
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file() && (entry.path().extension() == ".jpg" || entry.path().extension() == ".png")) {
            // TODO:加载各个面的单独贴图
        }
    }

    loadCubeMaps(path / "cubemaps");
}

void TexturePool::loadCubeMaps(const fs::path &path) {
    LOGGER("Cube map path: %s", path.string().c_str());
    for (const auto &entry : fs::directory_iterator(path)) {
        GLuint cubeMapId;
        auto name = entry.path().filename().string();
        if (load_cumbe_map(entry.path(), &cubeMapId)) {
            LOGGER("Cube map %s loaded", name.c_str());
            this->cubemap_ids_[name] = cubeMapId;
        } else {
            ERROR("Can not load cube map %s", name.c_str());
        }
    }
}
