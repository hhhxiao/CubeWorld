cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 20)
project(LearnOpenGL VERSION 0.1.0 LANGUAGES CXX)
include_directories(
PUBLIC
${PROJECT_SOURCE_DIR}/include  
${PROJECT_SOURCE_DIR}/include/imgui
${PROJECT_SOURCE_DIR}/include/single
${PROJECT_SOURCE_DIR}/3rd
)
set_source_files_properties(3rd/Remotery.c PROPERTIES LANGUAGE CXX )
add_library(base
        STATIC
        3rd/glad.cpp
        3rd/loguru.cpp
        3rd/imgui/imgui_demo.cpp
        3rd/imgui/imgui_draw.cpp
        3rd/imgui/imgui_impl_glfw.cpp
        3rd/imgui/imgui_impl_opengl3.cpp
        3rd/imgui/imgui_tables.cpp
        3rd/imgui/imgui_widgets.cpp
        3rd/imgui/imgui.cpp
        3rd/Remotery.c
)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819")
set_target_properties(base PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(base ${PROJECT_SOURCE_DIR}/3rd/glfw3.lib)

add_executable(
main 
main.cpp
# client
src/client/opengl_window.cpp
src/client/client_main.cpp
src/client/texture.cpp
src/client/simple_shader.cpp
src/client/renderable.cpp
src/client/resource_manager.cpp
src/client/game_camera.cpp
src/client/chunk_mesh.cpp
src/client/level_renderer.cpp
src/client/render_context.cpp
src/client/cube_map.cpp
src/client/client_level.cpp
src/client/buffer.cpp
#server
src/server/level_server.cpp
src/server/chunk.cpp 
src/server/terrain_generator.cpp
src/server/block.cpp
src/server/chunk_builder.cpp
src/server/player.cpp
src/server/data/position.cpp 

#common
src/common/config.cpp
src/common/utils.cpp
src/common/bridge.cpp
)
target_link_libraries(main base)
