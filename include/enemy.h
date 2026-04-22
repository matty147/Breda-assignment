#pragma once

#include "../tmpl8/surface.h"

namespace Tmpl8
{
class Level;
class Surface;

class Enemy
{
  public:
    Enemy(float iy, float ix, float ispeed = 3.0f, int idirection = 1, float igravity = 2.5f);
    void Update(float deltaTime, Level& level);
    void Draw(Surface* Gamescreen);

  private:
    // void UpdateTimers(float deltaTime, Level& level);
    bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);
    void UpdateY(float deltaTime, Level& level);
    void UpdateX(float deltaTime, Level& level, float moveX);
    float currentGravity = -1, x, y;
    int speed = 1, direction = 1, gravity = 1, playerWidth, playerHeight, tileSize = 32;
    bool grounded = false;
};
} // namespace Tmpl8
