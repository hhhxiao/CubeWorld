#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Config {
   public:
    // client

    static constexpr auto MAX_FPS = 120.f;
    static constexpr auto FRAME_TIME = 1.f / MAX_FPS;

    static constexpr auto MAX_UPS = 240.f;
    static constexpr auto UPDATE_TIME = 1.f / MAX_UPS;

    static std::string SOFTWARE_NAME;
    static std::string SOFTWARE_VERSION;
    static std::string RES_ROOT;

    // server
    static constexpr auto MAX_MSPT = 1000;
    static constexpr auto CHUNK_HEIGHT = 256;
    static constexpr auto CHUNK_GEN_THREAD_NUM = 4;
    static constexpr auto CHUNK_LIFE_TIME = 10;  // chunk存活动时间，如果超过30s没有被加载就析构
    static constexpr auto CHUNK_LIFE_TICK = CHUNK_LIFE_TIME * 1000 / MAX_MSPT;  // chuk存活时间(游戏刻单位)

    static int window_width;
    static int window_height;
    static float fov;
    static float zFar;
    static float zNear;
    //
    static bool enableImgui;
    static int load_radius;
    static int chunk_cache_size;
    static int view_distance;

    static glm::mat4 getProjectionMatrix();

    // window size
    // others
};
