#include "cube_map.h"
#include "texture.h"
#include "renderable.h"

CubeMap::CubeMap() : Renderable("cubemap") {
    std::vector<float> v{// positions
                         -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
                         1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                         -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
                         -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                         1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                         1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                         -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                         1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                         -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
                         1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                         -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
                         1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    for (auto i = 0; i < v.size() / 3; i++) {
        VertexAttribute attribute{v[i * 3 + 0], v[i * 3 + 1], v[i * 3 + 2], .0f, .0f, .0f, 0.0, 1.0};
        vertices_.push_back(attribute);
        indices_.push_back(i * 3);
        indices_.push_back(i * 3 + 1);
        indices_.push_back(i * 3 + 2);
    }
    texture_ = TextureManager::instance().getCubeMapID("overworld_sky");
}

void CubeMap::render(RenderContext& ctx) {
    glDepthMask(GL_FALSE);
    auto& shader = ctx.shader();
    shader.use(shader_name_);
    ctx.shader().setMat4("projection", Config::getProjectionMatrix());
    ctx.shader().setMat4("view", glm::mat4(glm::mat3(ctx.camera().getViewMatrix())));
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    enableAttr();
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
    glDrawElements(GL_TRIANGLES, (GLint)this->indices_.size(), GL_UNSIGNED_INT, 0);
    glDepthMask(GL_TRUE);
}
