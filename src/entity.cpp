#include "entity.h"


namespace Tmpl8
{

Entity::Entity(int startY, int startX, float startSpeed, int startDirection, float startGravity)
{
    y = (float)startY;
    x = (float)startX;
    speed = startSpeed;
    direction = startDirection;
    gravity = startGravity;
}

} // namespace Tmpl8