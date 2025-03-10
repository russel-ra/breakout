#pragma once

#include <vector>

#include "Object.h"

class Level
{
public:
    std::vector<Object> Bricks;
    Level() { }
    void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
    void Draw(SpriteRenderer& renderer);
private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData,
              unsigned int levelWidth, unsigned int levelHeight);
};
