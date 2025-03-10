#pragma once

#include "glm/glm.hpp"

#include "Object.h"
#include "Texture.h"

class Ball : public Object
{
public:
    // ball state	
    float     Radius;
    bool      Stuck;

    Ball(glm::vec2 pos, float radius, glm::vec2 velocity, Texture sprite);

    glm::vec2 Move(float dt, unsigned int window_width);
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};
