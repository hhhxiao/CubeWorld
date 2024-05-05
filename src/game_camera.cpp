#include "game_camera.h"
#include "glm/detail/type_vec.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

glm::mat4 GameCamera::getViewMatrix() {
    // 由于这里规定世界方向永远竖直向上，（人物不需要歪头，因此这里两个向量就能定位相机坐标，up可以直接计算）
    refreshDirCache();
    auto right = glm::cross(front_, glm::vec3(0.0, 1.0, 0.0));
    auto up = glm::cross(right, front_);
    return glm::lookAt(position_, position_ + front_, up);
}

GameCamera::GameCamera(glm::vec3 position, glm::vec3 front) : position_(position), front_(front) {}