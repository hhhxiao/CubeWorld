#ifndef VBO_POOL_H
#define VBO_POOL_H

#include "glad/glad.h"
#include <set>

class VboPool {
    inline GLuint acquire() { return 0; }
    GLuint recliam(GLuint v) { return v; }

   private:
    std::set<GLuint> assigned, reserved_;
};

#endif
