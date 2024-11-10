#include "resource_manager.h"
#include <filesystem>

void ResourceManager::init(const fs::path& res_root) { this->res_root_ = res_root; }
