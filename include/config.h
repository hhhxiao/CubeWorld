#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string_view>

class Config {
   public:
    // server
    static constexpr auto MAX_MSPT = 1000;
    static constexpr auto CHUNK_HEIGHT = 256;
    static constexpr auto CHUNK_GEN_THREAD_NUM = 4;
    static constexpr auto CHUNK_LIFE_TIME = 10;  // chunk存活动时间，如果超过30s没有被加载就析构
    static constexpr auto CHUNK_LIFE_TICK = CHUNK_LIFE_TIME * 1000 / MAX_MSPT;  // chuk存活时间(游戏刻单位)
    static constexpr auto LOAD_RADIUS = 12;
    static constexpr auto CHUNK_CACHE_SIZE = 400;

    // client
    // fps
    static constexpr auto MAX_FPS = 1000.f;
    static constexpr auto FRAME_TIME = 1.f / MAX_FPS;
    // tps
    static constexpr auto MAX_UPS = 240.f;
    static constexpr auto UPDATE_TIME = 1.f / MAX_UPS;

    static constexpr std::string_view SOFTWARE_NAME = "CubeWorld";
    static constexpr std::string_view SOFTWARE_VERSION = "0.0.1";
    static constexpr std::string_view RES_ROOT = R"(..\res)";

    static constexpr auto VIEW_DISTANCE = 10;
    static constexpr auto ENABLE_IMGUI = true;
    static constexpr auto ENABLE_OPENGL_DEBUG = true;
    // runtime
    static float fov;
    static float zFar;
    static float zNear;
    static int window_width;
    static int window_height;

    // fog
    static float fogNear;
    static float fogFar;
    static bool enableFog;

    // depth math ortho
    static float depth_ortho_side;
    static float depth_ortho_z_near;
    static float depth_ortho_z_far;
    static void updateWindowSize(int w, int h);
    static glm::mat4 getProjectionMatrix();
    // window size
    // others
};
