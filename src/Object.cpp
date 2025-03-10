#include "Object.h"

Object::Object(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec3 color, glm::vec2 velocity, float rotation)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(rotation), Sprite(sprite), IsSolid(false), Destroyed(false) { }

void Object::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}