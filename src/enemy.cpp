#include <algorithm>
#include <cmath>
#include <cstdio>
#include <windows.h>

#include "enemy.h"
#include "game.h"
#include "level.h"
#include "tmpl8/surface.h"

namespace Tmpl8
{
extern Sprite enemySprite;
extern int screenHeight, screenWidth;

/// <summary>
/// Constructs an enemy at the given spawn position with the given movement parameters.
/// </summary>
/// <param name="startY">Initial Y position in pixels.</param>
/// <param name="startX">Initial X position in pixels.</param>
/// <param name="startSpeed">Horizontal movement speed.</param>
/// <param name="startDirection">Initial facing direction (1 = right, -1 = left).</param>
/// <param name="startGravity">Per-frame gravity acceleration.</param>
Enemy::Enemy(int startY, int startX, float startSpeed, int startDirection, float startGravity)
    : Entity(startY, startX, startSpeed, startDirection, startGravity)
{
}

/// <summary>
/// Manages the Enemies's overall state, movement, and lifecycle for the current frame.
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
void Enemy::Update(float deltaTime, Level& level)
{
    width = enemySprite.GetWidth();
    height = enemySprite.GetHeight();

    UpdateX(deltaTime, level, 1.0);

    x = std::clamp(x, 0.0f, (float)screenWidth - width);

    UpdateY(deltaTime, level);
}

/// <summary>
/// calculates the enemy movement on the X axis
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
/// <param name="moveX"></param>
void Enemy::UpdateX(float deltaTime, Level& level, float moveX)
{
    moveX *= direction;

    float deltaX = moveX * (deltaTime / deltaTimeScale);
    float nextX = x + deltaX;

    int leftTile = (int)nextX / tileSize;
    int rightTile = (int)(nextX + width - 1) / tileSize;
    int topTile = (int)y / tileSize;
    int bottomTile = (int)(y + height - 1) / tileSize;

    bool hitX = TileCollision(topTile, bottomTile, leftTile, rightTile, level);

    bool edgeHitX = true;

    if (level.currentTime == TimeOfDay::Day)
    {
        int lr = (direction == 1) ? leftTile : rightTile;
        edgeHitX = TileCollision(bottomTile + 1, bottomTile + 1, lr + direction, lr + direction, level);
    }

    if (nextX < 0 || hitX)
    {
        direction *= -1;
    }
    else if (!edgeHitX)
    {
        direction *= -1;
    }

    if (hitX)
    {
        if (deltaX > 0)
        {
            x = (float)(rightTile * tileSize) - width;
        }
        else if (deltaX < 0)
        {
            x = (float)(leftTile * tileSize) + tileSize;
        }
    }
    else
    {
        x = nextX;
    }
}

/// <summary>
/// calculates the enemy movement on the Y axis
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
void Enemy::UpdateY(float deltaTime, Level& level)
{
    float maxGravity = 15.0f;

    currentGravity += gravity * (deltaTime / deltaTimeGravityScale);
    if (currentGravity > maxGravity)
        currentGravity = maxGravity;

    float deltaY = currentGravity * (deltaTime / deltaTimeScale);
    float nextY = y + deltaY;

    int leftTile = (int)x / tileSize;
    int rightTile = (int)(x + width - 1) / tileSize;
    int topTile = (int)nextY / tileSize;
    int bottomTile = (int)(nextY + height - 1) / tileSize;

    bool hitY = TileCollision(topTile, bottomTile, leftTile, rightTile, level);

    if (hitY)
    {
        if (deltaY > 0)
        {
            y = (float)(bottomTile * tileSize) - height;
            currentGravity = 0;
        }
        else if (deltaY < 0)
        {
            y = (float)(topTile * tileSize) + tileSize;
            currentGravity = 0;
        }
    }
    else
    {
        y = nextY;
    }
}

/// <summary>
/// Checks for what to do on collision with a object
/// </summary>
/// <param name="topTile"></param>
/// <param name="bottomTile"></param>
/// <param name="leftTile"></param>
/// <param name="rightTile"></param>
/// <param name="level"></param>
/// <returns></returns>
bool Enemy::TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level)
{
    bool hit = false;
    for (int r = topTile; r <= bottomTile; r++)
    {
        for (int c = leftTile; c <= rightTile; c++)
        {
            int tileId = level.GetTileID(r * tileSize, c * tileSize);

            switch (std::abs(tileId))
            {
                case (int)TileType::Spike:
                {
                    continue;
                }

                case (int)TileType::Water:
                {
                    if (level.currentTime == TimeOfDay::Day)
                    {
                        continue;
                    }
                    break;
                }

                case (int)TileType::MoonBlock:
                    if (level.currentTime == TimeOfDay::Night)
                    {
                        return true;
                    }
                    continue;

                case (int)TileType::SunBlock:
                    if (level.currentTime == TimeOfDay::Day)
                    {
                        return true;
                    }
                    continue;

                case (int)TileType::Sun:
                    level.currentTime = TimeOfDay::Day;
                    continue;

                case (int)TileType::Moon:
                    level.currentTime = TimeOfDay::Night;
                    continue;

                case (int)TileType::Flag:
                    continue;
            }

            if (tileId > 0)
            {
                return true;
            }
        }
    }
    return false;
}

/// <summary>
/// draws the enemy sprite
/// </summary>
/// <param name="gameScreen"></param>
void Enemy::Draw(Surface* gameScreen) { enemySprite.Draw(gameScreen, GetX(), GetY()); }
} // namespace Tmpl8
