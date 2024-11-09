#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include "glm/detail/type_vec.hpp"
class Player {
   public:
    inline void setPos(const glm::vec3& position) { this->position_ = position; }

    inline glm::vec3 getPos() const { return position_; }

   private:
    glm::vec3 position_;
};
#endif
