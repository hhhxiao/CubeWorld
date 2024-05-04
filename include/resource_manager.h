#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class ResourceManager {
   public:
    inline std::filesystem::path shader_path() { return res_root_ / "shaders"; }
    ResourceManager(const std::string& res_root);

   private:
    fs::path res_root_;
};
