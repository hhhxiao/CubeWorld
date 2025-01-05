#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

#include <cstddef>
#include <cstdint>
#include <queue>
#include <tuple>
#include <unordered_map>
#include "chunk.h"
#include "position.h"
#include "renderable.h"

class ChunkMesh;
class RenderContext;
class ClientLevel;

struct LevelChunkRenderOrder {
    bool operator()(const LevelChunk& c1, const LevelChunk& c2);
    static ChunkPos camera;
};

class LevelRenderer {
   public:
    explicit LevelRenderer(ClientLevel* clientLevel);
    ~LevelRenderer();

    void init();

    void updateMesh(RenderContext& ctx);
    void renderOneFrame(RenderContext& ctx);
    void renderBlockWorld(RenderContext& ctx);

    BlockType getBlock(int x, int y, int z);
    LevelChunk* getChunkData(const ChunkPos& pos);

   private:
    ClientLevel* client_level_{nullptr};

    //
    std::unordered_map<ChunkPos, ChunkMesh*> meshes_;
    std::unordered_map<ChunkPos, LevelChunk> data_;
    std::unordered_map<ChunkPos, std::tuple<uint8_t, uint8_t>> masks_;
    std::priority_queue<LevelChunk, std::vector<LevelChunk>, LevelChunkRenderOrder> render_chunk_queue_;
    ChunkMesh* current_mesh_{nullptr};
    // data
    // debug
    Light light;
};
#endif
