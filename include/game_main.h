#pragma once

#include <cstddef>
#include "block.h"
#include "chunk.h"
#include "drawable_object.h"
#include "game_camera.h"
#include "simple_shader.h"
#include "opengl_window.h"
#include "config.h"
#include "level.h"
#include "resource_manager.h"
#include "subchunk_mesh.h"
#include "texture.h"

class GameMain {
   public:
    GameMain() {
        this->window = new OpenGLWindow(Config::window_width, Config::window_height, Config::SOFTWARE_NAME);
        this->resource_manager_ = new ResourceManager(R"(C:\Users\xhy\dev\CubeWorld\res)");
        shader =
            new Shader(resource_manager_->shader_path() / "main.vert", resource_manager_->shader_path() / "main.frag");
        this->camera_ = new GameCamera(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        this->texture_pool_ = new TexturePool();
        this->texture_pool_->init(resource_manager_->texture_path());
        this->level = new Level();

        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                subchunk.setBlock(x + z / 8, 0 + x / 4, z, dirt);
                subchunk.setBlock(x + z / 8, 1 + x / 4, z, dirt);
                subchunk.setBlock(x + z / 8, 2 + x / 4, z, dirt);
                subchunk.setBlock(x + z / 8, 3 + x / 4, z, dirt);
                subchunk.setBlock(x + z / 8, 4 + x / 4, z, grass);
            }
        }
        this->mesh = subchunk.createMesh();
    }

    void show();

    void init();

    ~GameMain();

   private:
    void renderTick();
    void gameTick();
    void processKeyBoardInput(GLFWwindow *window);
    void processMouseInput(GLFWwindow *window, double x, double y);

    // render
    size_t tick_{0};
    double last_frame_time_{0.0};
    double delta_time_{0.0};
    GameCamera *camera_;
    ResourceManager *resource_manager_;
    Shader *shader;
    OpenGLWindow *window{nullptr};
    TexturePool *texture_pool_{nullptr};
    // data
    SubChunkMesh *mesh;
    SubChunk subchunk;
    DrawableObject debugObj;
    Level *level;
};
