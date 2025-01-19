#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

#include <cstdint>
#include <queue>
#include <tuple>
#include <unordered_map>
#include "chunk.h"
#include "chunk_mesh.h"
#include "imgui_debug_info.h"
#include "position.h"
#include "cube_map.h"
#include "renderable.h"

class ChunkMeshOld;
class RenderContext;
class ClientLevel;

struct LevelChunkRendererOrder {
    bool operator()(const LevelChunk& c1, const LevelChunk& c2);
    static ChunkPos camera;
};

class LevelRenderer {
    friend class ChunkRenderer;

   public:
    explicit LevelRenderer(ClientLevel* clientLevel);
    ~LevelRenderer();
    void init();

    void updateMesh(RenderContext& ctx);
    void renderOneFrame(RenderContext& ctx);
    void renderBlockWorld(RenderContext& ctx);

    BlockType getBlock(int x, int y, int z);
    LevelChunk* getChunkData(const ChunkPos& pos);

    ChunkRenderer& chunkRender() { return chunk_render_; }

   private:
    ClientLevel* client_level_{nullptr};
    //
    // std::unordered_map<ChunkPos, ChunkMeshOld*> meshes_;
    // std::unordered_map<ChunkPos, LevelChunk> data_;
    // std::unordered_map<ChunkPos, std::tuple<uint8_t, uint8_t>> masks_;
    // std::priority_queue<LevelChunk, std::vector<LevelChunk>, LevelChunkRendererOrder> render_chunk_queue_;
    // ChunkMeshOld* current_mesh_{nullptr};
    // data
    // debug
    Light light;
    CubeMap* skybox{nullptr};
    ChunkRenderer chunk_render_;
};
#endif
