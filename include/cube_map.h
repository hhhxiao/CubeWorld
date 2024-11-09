#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include "drawable_object.h"
#include "render_context.h"
class CubeMap : public Renderable {
   public:
    CubeMap();
    void render(RenderContext& ctx) override;

    virtual ~CubeMap() {}

   private:
    GLuint texture_{0};
};
#endif
