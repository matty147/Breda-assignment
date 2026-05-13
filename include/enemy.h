#pragma once

#include "../tmpl8/surface.h"
#include "entity.h"

namespace Tmpl8
{
class Level;
class Surface;

class Enemy: public Entity 
{
  public:
    Enemy(int startY, int startX, float startSpeed = 3.0f, int startDirection = 1, float startGravity = 2.5f);

    void Update(float deltaTime, Level& level);
    void Draw(Surface* gameScreen);

  private:
    // void UpdateTimers(float deltaTime, Level& level);
    bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);
    void UpdateY(float deltaTime, Level& level);
    void UpdateX(float deltaTime, Level& level, float moveX);
};
} // namespace Tmpl8
