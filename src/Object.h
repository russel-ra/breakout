#pragma once

#include "glm/glm.hpp"

#include "Texture.h"
#include "SpriteRenderer.h"

class Object
{
public:
    // position is top-left
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    float       Rotation;
    bool        IsSolid;
    bool        Destroyed;

    Texture    Sprite;

    Object(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f), float rotation = 0.0f);

    virtual void Draw(SpriteRenderer& renderer);
};

