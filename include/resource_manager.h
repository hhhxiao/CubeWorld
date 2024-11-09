#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class ResourceManager {
   public:
    ResourceManager() = default;
    inline std::filesystem::path shader_path() { return res_root_ / "shaders"; }
    inline std::filesystem::path texture_path() { return res_root_ / "textures"; }

   public:
    void init(const fs::path& path);

   private:
    fs::path res_root_;
};
