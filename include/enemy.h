#pragma once

#include "../tmpl8/surface.h"

namespace Tmpl8
{
class Level;
class Surface;

class Enemy
{
  public:
    Enemy(float startY, float startX, float startSpeed = 3.0f, int startDirection = 1, float startGravity = 2.5f);

    void Update(float deltaTime, Level& level);
    void Draw(Surface* gameScreen);

    float GetX() const { return x; }
    float GetY() const { return y; }

  private:
    // void UpdateTimers(float deltaTime, Level& level);
    bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);
    void UpdateY(float deltaTime, Level& level);
    void UpdateX(float deltaTime, Level& level, float moveX);
    float x, y,  currentGravity = -1;
    int speed = 1, direction = 1, gravity = 1, enemyWidth, enemyHeight, tileSize = 32;
    bool grounded = false;
};
} // namespace Tmpl8
