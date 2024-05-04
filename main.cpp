#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "game_main.h"

// void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// void processInput(GLFWwindow *window);

// // settings
// const unsigned int SCR_WIDTH = 800;
// const unsigned int SCR_HEIGHT = 600;

// // camera
// Camera camera(glm::vec3(0.0f, 5.0f, 3.0f));
// float lastX = SCR_WIDTH / 2.0f;
// float lastY = SCR_HEIGHT / 2.0f;
// bool firstMouse = true;

// // timing
// float deltaTime = 0.0f;  // time between current frame and last frame
// float lastFrame = 0.0f;

// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() {
    GameMain main;
    main.init();
    main.show();
    // OpenGLWindow window(width, height, "CubeWorld");
    // // Setup Dear ImGui context
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO &io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    // io.FontGlobalScale = 2.0;

    // // Setup Platform/Renderer backends
    // ImGui_ImplGlfw_InitForOpenGL(
    //     window.window(),
    //     true);  // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    // ImGui_ImplOpenGL3_Init();

    // glfwSetCursorPosCallback(window.window(), mouse_callback);
    // glfwSetScrollCallback(window.window(), scroll_callback);
    // glfwSetInputMode(window.window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glEnable(GL_DEPTH_TEST);
    // const std::string shader_path = R"(C:\Users\xhy\dev\OpenGL\shaders\)";
    // SimpleShader shader(shader_path + "1.vert", shader_path + "1.frag");

    // SimpleShader boxShader(shader_path + "box.vert", shader_path + "box.frag");
    // SimpleShader lightShader(shader_path + "light.vert", shader_path + "light.frag");

    // std::vector<DrawableObject::Attribute> va1{
    //     {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f}, {0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    //     {0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f},   {0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    //     {-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f},  {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    //     {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f},   {0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
    //     {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f},     {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
    //     {-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f},    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
    //     {-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f},   {-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f},
    //     {-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f}, {-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f},
    //     {-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f},  {-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f},
    //     {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f},     {0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f},
    //     {0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f},   {0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f},
    //     {0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f},    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f},
    //     {-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f}, {0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f},
    //     {0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f},   {0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f},
    //     {-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f},  {-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f},
    //     {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f},   {0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
    //     {0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f},     {0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f},
    //     {-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f},    {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f}};

    // std::vector<GLuint> id1;
    // id1.reserve(va1.size());
    // for (int i = 0; i < va1.size(); i++) id1.push_back(i);

    // DrawableObject box(va1, id1);
    // box.init();

    // DrawableObject light(va1, id1);
    // light.init();
    // window.pool([&](GLFWwindow *win) {
    //     ImGui_ImplOpenGL3_NewFrame();
    //     ImGui_ImplGlfw_NewFrame();
    //     ImGui::NewFrame();
    //     ImGui::Begin("const char *name");
    //     ImGui::Text("Hello world");
    //     int intValue = 50;
    //     ImGui::SliderInt("Slider", &intValue, 0, 100);
    //     ImGui::End();
    //     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //     ImGui::Render();
    //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //     // (Your code calls glfwSwapBuffers() etc.)
    // });
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();
    return 0;
}

// void processInput(GLFWwindow *window) {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

//     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
//     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
//     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
//     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
// }

// // glfw: whenever the mouse moves, this callback is called
// // -------------------------------------------------------
// void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
//     auto xpos = static_cast<float>(xposIn);
//     auto ypos = static_cast<float>(yposIn);
//     if (firstMouse) {
//         lastX = xpos;
//         lastY = ypos;
//         firstMouse = false;
//     }

//     float xoffset = xpos - lastX;
//     float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top
//     lastX = xpos;
//     lastY = ypos;
//     camera.ProcessMouseMovement(xoffset, yoffset);
// }
// // glfw: whenever the mouse scroll wheel scrolls, this callback is called
// // ----------------------------------------------------------------------
// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
//     camera.ProcessMouseScroll(static_cast<float>(yoffset));
// }
