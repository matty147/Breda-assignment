#pragma once

#include "../tmpl8/surface.h"
#include "entity.h"

namespace Tmpl8
{
class Level;
class Surface;

class Enemy : public Entity
{
  public:
    /// <summary>Constructs an enemy at the given spawn position.</summary>
    /// <param name="startY">Initial Y position in pixels.</param>
    /// <param name="startX">Initial X position in pixels.</param>
    /// <param name="startSpeed">Horizontal movement speed.</param>
    /// <param name="startDirection">Initial facing direction (1 = right, -1 = left).</param>
    /// <param name="startGravity">Per-frame gravity acceleration.</param>
    Enemy(int startY, int startX, float startSpeed = 3.0f, int startDirection = 1, float startGravity = 2.5f);

    /// <summary>Updates the enemy for one frame: movement and collisions.</summary>
    /// <param name="deltaTime">Frame delta time in milliseconds.</param>
    /// <param name="level">Current level for tile lookups.</param>
    void Update(float deltaTime, Level& level);

    /// <summary>Renders the enemy sprite to the given surface.</summary>
    /// <param name="gameScreen">Target screen surface.</param>
    void Draw(Surface* gameScreen);

  private:
    /// <summary>Checks the rectangle defined by the given tile bounds for solid tiles.</summary>
    /// <param name="topTile">Top tile row (inclusive).</param>
    /// <param name="bottomTile">Bottom tile row (inclusive).</param>
    /// <param name="leftTile">Left tile column (inclusive).</param>
    /// <param name="rightTile">Right tile column (inclusive).</param>
    /// <param name="level">Current level for tile lookups.</param>
    /// <returns>True if a solid tile was hit.</returns>
    bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);

    /// <summary>Applies gravity and resolves vertical tile collisions.</summary>
    /// <param name="deltaTime">Frame delta time in milliseconds.</param>
    /// <param name="level">Current level for tile lookups.</param>
    void UpdateY(float deltaTime, Level& level);

    /// <summary>Moves the enemy horizontally, turns at walls and ledges (during day).</summary>
    /// <param name="deltaTime">Frame delta time in milliseconds.</param>
    /// <param name="level">Current level for tile lookups.</param>
    /// <param name="moveX">Base horizontal velocity (gets multiplied by direction).</param>
    void UpdateX(float deltaTime, Level& level, float moveX);
};
} // namespace Tmpl8
