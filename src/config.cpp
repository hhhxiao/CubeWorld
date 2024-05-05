#include "config.h"
#include <string>

std::string Config::SOFTWARE_NAME = "CubeWorld";
std::string Config::SOFTWARE_VERSION = "0.0.1";

int Config::window_width = 1920;
int Config::window_height = 1080;

float Config::fov = 45.0f;
float Config::zNear = 0.1f;
float Config::zFar = 100.0f;

bool Config::enableImgui = true;
int Config::load_radius = 6;
int Config::chunk_cache_size = 200;

glm::mat4 Config::getProjectionMatrix() {
    return glm::perspective(glm::radians(fov), (float)window_width / (float)window_height, zNear, zFar);
}
