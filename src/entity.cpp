#include "entity.h"

namespace Tmpl8
{

/// <summary>
/// Constructs an entity at the given position with movement parameters.
/// </summary>
/// <param name="startY">Initial Y position in pixels.</param>
/// <param name="startX">Initial X position in pixels.</param>
/// <param name="startSpeed">Horizontal movement speed.</param>
/// <param name="startDirection">Initial facing direction (1 = right, -1 = left).</param>
/// <param name="startGravity">Per-frame gravity acceleration.</param>
Entity::Entity(int startY, int startX, float startSpeed, int startDirection, float startGravity)
{
    y = (float)startY;
    x = (float)startX;
    speed = startSpeed;
    direction = startDirection;
    gravity = startGravity;
}

} // namespace Tmpl8