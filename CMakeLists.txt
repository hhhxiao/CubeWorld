cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)
project(LearnOpenGL VERSION 0.1.0 LANGUAGES CXX)
include_directories(${PROJECT_SOURCE_DIR}/include  ${PROJECT_SOURCE_DIR}/include/imgui)

add_library(base 3rd/glad.cpp
        3rd/imgui/imgui_demo.cpp
        3rd/imgui/imgui_draw.cpp
        3rd/imgui/imgui_impl_glfw.cpp
        3rd/imgui/imgui_impl_opengl3.cpp
        3rd/imgui/imgui_tables.cpp
        3rd/imgui/imgui_widgets.cpp
        3rd/imgui/imgui.cpp
)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819")
set_target_properties(base PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(base ${PROJECT_SOURCE_DIR}/3rd/glfw3.lib)

add_executable(
main 
main.cpp
# render
src/opengl_window.cpp
src/game_main.cpp
src/config.cpp
src/texture.cpp
src/simple_shader.cpp
src/utils.cpp
src/drawable_object.cpp
src/resource_manager.cpp
src/game_camera.cpp
src/input_handler.cpp
# data
src/level.cpp
src/chunk.cpp 
src/terrain_generator.cpp
src/subchunk_mesh.cpp 
src/block.cpp
src/async_chunk_cache.cpp
)
target_link_libraries(main base)
