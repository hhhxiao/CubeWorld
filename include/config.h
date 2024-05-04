#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Config {
   public:
    static std::string SOFTWARE_NAME;
    static std::string SOFTWARE_VERSION;

    static int window_width;
    static int window_height;
    static float fov;
    static float zFar;
    static float zNear;
    //
    static bool enableImgui;

    static glm::mat4 getProjectionMatrix();

    // window size
    // others
};
