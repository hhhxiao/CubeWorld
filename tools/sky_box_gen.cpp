#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "image.h"

#define _CRT_SECURE_NO_WARNINGS

int main(int argc, char **argv) {
    namespace fs = std::filesystem;
    auto path = fs::path("../res/textures/cubemaps/overworld_sky");
    Image image(128, 128, 3);
    glm::ivec3 c1(183, 210, 255), c2(128, 172, 255);
    float min = 50.f, max = 60.f;
    for (int x = 0; x < 128; x++) {
        for (int y = 0; y < 128; y++) {
            auto v = std::clamp((static_cast<float>(y) - min) / (max - min), 0.f, 1.f);
            for (int n = 0; n < 3; n++) {
                image.get(x, y)[n] = static_cast<uint8_t>((float)c1[n] * v + (float)c2[n] * (1.0 - v));
            }
        }
    }
    image.save(path / "front.png");
    image.save(path / "left.png");
    image.save(path / "right.png");
    image.save(path / "back.png");
    for (int x = 0; x < 128; x++) {
        for (int y = 0; y < 128; y++) {
            for (int n = 0; n < 3; n++) {
                image.get(x, y)[n] = static_cast<uint8_t>(c2[n]);
            }
        }
    }
    image.save(path / "top.png");
    for (int x = 0; x < 128; x++) {
        for (int y = 0; y < 128; y++) {
            for (int n = 0; n < 3; n++) {
                image.get(x, y)[n] = static_cast<uint8_t>(c1[n]);
            }
        }
    }
    image.save(path / "bottom.png");
    return 0;
}