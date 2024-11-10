#include "render_context.h"
#include "config.h"

RenderContext::RenderContext() {}

void RenderContext::init() {
    this->resource_manager_.init(Config::RES_ROOT);
    this->shader_.init(resource_manager_.shader_path());
}
RenderContext::~RenderContext() {}
