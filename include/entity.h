#pragma once


#include "../tmpl8/surface.h"

namespace Tmpl8
{
class Surface;
class Level;

class Entity
{
  public:
    Entity(int startY, int startX, float startSpeed, int startDirection, float startGravity);

    int GetX() const { return (int)x; }
    int GetY() const { return (int)y; }

  protected:
    float x, y;
    float currentGravity = -1.0f, speed, gravity = 1.0f;
    int direction = 1;
    int tileSize = 32;
    int width, height;
    bool grounded = false;
};
} // namespace Tmpl8
