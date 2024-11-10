#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "client_main.h"

int main() {
    ClientMain main;
    main.init();
    main.show();

    return 0;
}