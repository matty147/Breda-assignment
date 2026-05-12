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

Enemy::Enemy(float startY, float startX, float startSpeed, int startDirection, float startGravity)
{
    y = startY;
    x = startX;
    speed = startSpeed;
    direction = startDirection;
    gravity = startGravity;
}

void Enemy::Update(float deltaTime, Level& level)
{
    enemyWidth = enemySprite.GetWidth();
    enemyHeight = enemySprite.GetHeight();

    int moveX = 1;

    UpdateX(deltaTime, level, moveX);

    x = std::clamp(x, 0.0f, (float)screenWidth - enemyWidth);

    UpdateY(deltaTime, level);
}

void Enemy::UpdateX(float deltaTime, Level& level, float moveX)
{
    moveX *= direction;

    float deltaX = moveX * (deltaTime / 10.0f);
    float nextX = x + deltaX;

    int leftTile = (int)nextX / tileSize;
    int rightTile = (int)(nextX + enemyWidth - 1) / tileSize;
    int topTile = (int)y / tileSize;
    int bottomTile = (int)(y + enemyWidth - 1) / tileSize;

    bool hitX = TileCollision(topTile, bottomTile, leftTile, rightTile, level);

    bool edgeHitX = 1;

    if (level.currentTime == TimeOfDay::Day)
    {

        if (direction == 1)
        {
            edgeHitX = TileCollision(bottomTile + 1, bottomTile + 1, leftTile + direction, leftTile + direction, level);
        }
        else
            edgeHitX = TileCollision(bottomTile + 1, bottomTile + 1, rightTile + direction, rightTile + direction, level);
    }

    if (0 < hitX || nextX < 0)
    {
        direction *= -1;
    }
    else if (0 >= edgeHitX)
    {
        direction *= -1;
    }

    if (hitX)
    {
        if (deltaX > 0)
        {
            x = (rightTile * tileSize) - enemyWidth;
        }
        else if (deltaX < 0)
        {
            x = (leftTile * tileSize) + tileSize;
        }
    }
    else
    {
        x = nextX;
    }
}

void Enemy::UpdateY(float deltaTime, Level& level)
{
    float maxGravity = 15.0f;

    currentGravity += gravity * (deltaTime / 100.0f);
    if (currentGravity > maxGravity)
        currentGravity = maxGravity;

    float deltaY = currentGravity * (deltaTime / 10.0f);
    float nextY = y + deltaY;

    int leftTile = (int)x / tileSize;
    int rightTile = (int)(x + enemyWidth - 1) / tileSize;
    int topTile = (int)nextY / tileSize;
    int bottomTile = (int)(nextY + enemyWidth - 1) / tileSize;

    bool hitY = TileCollision(topTile, bottomTile, leftTile, rightTile, level);

    if (hitY)
    {
        if (deltaY > 0)
        {
            y = (bottomTile * tileSize) - enemyWidth;
            currentGravity = 0;
        }
        else if (deltaY < 0)
        {
            y = (topTile * tileSize) + tileSize;
            currentGravity = 0;
        }
    }
    else
    {
        y = nextY;
    }
}

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
                    if (level.currentTime == TimeOfDay::Night)
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
                    // case (int)TileType::Sun:
                    // case (int)TileType::Moon:
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

void Enemy::Draw(Surface* gameScreen) { enemySprite.Draw(gameScreen, x, y); }
} // namespace Tmpl8
