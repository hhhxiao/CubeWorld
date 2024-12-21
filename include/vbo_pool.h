#ifndef VBO_POOL_H
#define VBO_POOL_H

#include "glad/glad.h"
class VboPool {
    GLuint acquire();
    GLuint recliam();
};

#endif
