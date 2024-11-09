//
// Created by xhy on 2024/5/3.
//

#include "simple_shader.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "utils.h"
namespace {
    void checkCompileErrors(unsigned int shader, const std::string &type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    GLuint load_shader_file(const fs::path &vertexPath, const fs::path &fragmentPath) {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure &e) {
            ERROR("can not read shader %s", e.what());
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment SimpleShader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        auto id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        checkCompileErrors(id, "PROGRAM");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return id;
    }
}  // namespace

void Shader::init(const fs::path &path) {
    LOGGER("Begin read shaders in path: %s", path.string().c_str());
    std::unordered_map<std::string, fs::path> verts, frags;
    for (const auto &entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;
        auto ext = entry.path().extension();
        auto name = entry.path().stem().string();
        if (ext == ".vert") verts[name] = entry.path();
        if (ext == ".frag") frags[name] = entry.path();
    }

    for (auto &kv : verts) {
        auto it = frags.find(kv.first);
        if (it != frags.end()) {
            LOGGER("Found shader: %s", kv.first.c_str());
            auto id = load_shader_file(kv.second, it->second);
            this->shaders_[kv.first] = id;
        }
    }
}
