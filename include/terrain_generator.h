#pragma once

class Chunk;
class AbstractTerrainGenerator {
   public:
    virtual void fill(Chunk *chunk) = 0;
};

class FlatTerrainGenerator : public AbstractTerrainGenerator {
   public:
    void fill(Chunk *chunk) override;
};
