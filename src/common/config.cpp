#include "config.h"

float Config::fov = 30.0f;
float Config::zNear = 0.1f;
float Config::zFar = 400.0f;

int Config::window_width = 1920;
int Config::window_height = 1080;

// fog
float Config::fogFar = VIEW_DISTANCE * 16;
float Config::fogNear = (VIEW_DISTANCE - 1.0) * 16;
bool Config::enableFog = true;

float Config::depth_ortho_side = 100;
float Config::depth_ortho_z_near = -100;
float Config::depth_ortho_z_far = 300;

void Config::updateWindowSize(int w, int h) {
    window_width = w;
    window_height = h;
}

glm::mat4 Config::getProjectionMatrix() {
    return glm::perspective(glm::radians(fov), (float)window_width / (float)window_height, zNear, zFar);
}
