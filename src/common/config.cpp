#include "config.h"

float Config::fov = 30.0f;
float Config::zNear = 0.1f;
float Config::zFar = 400.0f;

int Config::window_width = 1920;
int Config::window_height = 1080;

void Config::updateWindowSize(int w, int h) {
    window_width = w;
    window_height = h;
}

glm::mat4 Config::getProjectionMatrix() {
    return glm::perspective(glm::radians(fov), (float)window_width / (float)window_height, zNear, zFar);
}
