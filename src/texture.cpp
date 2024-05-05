//
// Created by xhy on 2024/5/3.
//

#include "texture.h"
#include <filesystem>
#include <string>
#include <unordered_map>
#include "block.h"
#include "subchunk_mesh.h"
#include "utils.h"
#include "magic_enum/magic_enum.hpp"

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
}  // namespace

TexturePool &TexturePool::instance() {
    static TexturePool pool;
    return pool;
}
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

void TexturePool::init(const std::filesystem::path &path) {
    std::unordered_map<std::string, BlockType> block_type_map;
    for (const auto &entry : magic_enum::enum_entries<BlockType>()) {
        block_type_map[std::string(entry.second)] = entry.first;
    }
    LOGGER("Block type map:");
    for (auto &kv : block_type_map) {
        LOGGER(" - %s -> %d", kv.first.c_str(), kv.second);
    }

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
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
}
