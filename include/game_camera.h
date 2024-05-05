#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/detail/func_trigonometric.hpp"
#include "glm/detail/type_vec.hpp"
#include "glm/gtc/quaternion.hpp"

class GameCamera {
   public:
    enum Dir { left, right, forward, back, up, down, invalid };

   public:
    glm::mat4 getViewMatrix();

    GameCamera(glm::vec3 position, glm::vec3 front);

    inline void updatePosition(const glm::vec3& newPosition) { position_ = newPosition; }

    inline void updateDir(float yawOffset, float pitchOffset) {
        yaw_ = yawOffset + yaw_;
        pitch_ = std::clamp(pitch_ + pitchOffset, -89.0f, 89.0f);
        // 计算方向
    }
    inline void refreshDirCache() {
        front_.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_.y = sin(glm::radians(pitch_));
        front_.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    }

    inline void Move(Dir dir, float delta) {
        switch (dir) {
            case left:
                position_ += glm::normalize(glm::vec3(front_.z, 0, -front_.x)) * delta;
                break;
            case right:
                position_ += glm::normalize(glm::vec3(front_.z, 0, -front_.x)) * -delta;
                break;
            case forward:
                position_ += glm::normalize(glm::vec3(front_.x, 0, front_.z)) * delta;
                break;
            case back:
                position_ += glm::normalize(glm::vec3(front_.x, 0, front_.z)) * -delta;
                break;
            case up:
                position_.y += delta;
                return;
            case down:
                position_.y -= delta;
                return;
            default:
                break;
        }
    }
    glm::vec3 position_;
    float yaw_{0.0f};    // 左右角度
    float pitch_{0.0f};  // 上下仰角
    glm::vec3 front_;
};