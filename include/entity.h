#pragma once

#include "../tmpl8/surface.h"

namespace Tmpl8
{
class Surface;
class Level;

class Entity
{
  public:
    /// <summary>Constructs an entity at the given position with movement parameters.</summary>
    /// <param name="startY">Initial Y position in pixels.</param>
    /// <param name="startX">Initial X position in pixels.</param>
    /// <param name="startSpeed">Horizontal movement speed.</param>
    /// <param name="startDirection">Initial facing direction (1 = right, -1 = left).</param>
    /// <param name="startGravity">Per-frame gravity acceleration.</param>
    Entity(int startY, int startX, float startSpeed, int startDirection, float startGravity);

    /// <summary>Returns the current X position in pixels.</summary>
    int GetX() const { return (int)x; }

    /// <summary>Returns the current Y position in pixels.</summary>
    int GetY() const { return (int)y; }

  protected:
    float x, y;
    float currentGravity = -1.0f, speed, gravity = 1.0f;
    int direction = 1;
    int width, height;
    bool grounded = false;

    static constexpr float deltaTimeScale = 10.0f;
    static constexpr float deltaTimeGravityScale = 100.0f;
};
} // namespace Tmpl8
