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

Enemy::Enemy(int startY, int startX, float startSpeed, int startDirection, float startGravity)
    : Entity(startY, startX, startSpeed, startDirection, startGravity)
{
}

void Enemy::Update(float deltaTime, Level& level)
{
    width = enemySprite.GetWidth();
    height = enemySprite.GetHeight();

    UpdateX(deltaTime, level, 1.0);

    x = std::clamp(x, 0.0f, (float)screenWidth - width);

    UpdateY(deltaTime, level);
}

void Enemy::UpdateX(float deltaTime, Level& level, float moveX)
{
    moveX *= direction;

    float deltaX = moveX * (deltaTime / 10.0f);
    float nextX = x + deltaX;

    int leftTile = (int)nextX / tileSize;
    int rightTile = (int)(nextX + width - 1) / tileSize;
    int topTile = (int)y / tileSize;
    int bottomTile = (int)(y + width - 1) / tileSize;

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
            x = (float) (leftTile * tileSize) + tileSize;
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
    int rightTile = (int)(x + width - 1) / tileSize;
    int topTile = (int)nextY / tileSize;
    int bottomTile = (int)(nextY + width - 1) / tileSize;

    bool hitY = TileCollision(topTile, bottomTile, leftTile, rightTile, level);

    if (hitY)
    {
        if (deltaY > 0)
        {
            y = (float)(bottomTile * tileSize) - width;
            currentGravity = 0;
        }
        else if (deltaY < 0)
        {
            y = (float) (topTile * tileSize) + tileSize;
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

void Enemy::Draw(Surface* gameScreen) { enemySprite.Draw(gameScreen, GetX(), GetY()); }
} // namespace Tmpl8
