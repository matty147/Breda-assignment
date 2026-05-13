#pragma once

#include "../tmpl8/surface.h"

namespace Tmpl8
{
class Level;
class Surface;

class Enemy
{
  public:
    Enemy(int startY, int startX, float startSpeed = 3.0f, int startDirection = 1, float startGravity = 2.5f);

    void Update(float deltaTime, Level& level);
    void Draw(Surface* gameScreen);

    int GetX() const { return (int)x; }
    int GetY() const { return (int)y; }

  private:
    // void UpdateTimers(float deltaTime, Level& level);
    bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);
    void UpdateY(float deltaTime, Level& level);
    void UpdateX(float deltaTime, Level& level, float moveX);
    float x, y, currentGravity = -1.0f, speed, gravity = 1.0f;
    int direction = 1, enemyWidth, enemyHeight, tileSize = 32;
    bool grounded = false;
};
} // namespace Tmpl8
