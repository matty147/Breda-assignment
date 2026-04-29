#include <algorithm>
#include <cmath>
#include <cstdio>
#include <windows.h>

#include "../include/enemy.h"
#include "../include/game.h"
#include "../include/level.h"
#include "../tmpl8/surface.h"

namespace Tmpl8
{
extern Sprite enemySprite;
extern int screenHeight, screenWidth;

Enemy::Enemy(float iy, float ix, float ispeed, int idirection, float igravity)
{
    y = iy;
    x = ix;
    speed = ispeed;
    direction = idirection;
    gravity = igravity;
}

void Enemy::Update(float deltaTime, Level& level)
{
    playerWidth = enemySprite.GetWidth();
    playerHeight = enemySprite.GetHeight();

    int moveX = 1;

    UpdateX(deltaTime, level, moveX);

    x = std::clamp(x, 0.0f, (float)screenWidth - playerWidth);

    UpdateY(deltaTime, level);
}

void Enemy::UpdateX(float deltaTime, Level& level, float moveX)
{
    moveX *= direction;

    float deltaX = moveX * (deltaTime / 10.0f);
    float nextX = x + deltaX;

    int leftTile = (int)nextX / tileSize;
    int rightTile = (int)(nextX + playerWidth - 1) / tileSize;
    int topTile = (int)y / tileSize;
    int bottomTile = (int)(y + playerHeight - 1) / tileSize;

    bool hitX = TileCollision(topTile, bottomTile, leftTile, rightTile, level);

    bool edgeHitX = 1;

    if (level.currentDay == timeOfDay::Day)
    {

        if (direction == 1)
        {
            edgeHitX = TileCollision(bottomTile + 1, bottomTile + 1, leftTile + direction, leftTile + direction, level);
        }
        else edgeHitX = TileCollision(bottomTile + 1, bottomTile + 1, rightTile + direction, rightTile + direction, level);
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
            x = (rightTile * tileSize) - playerWidth;
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
    currentGravity += gravity * (deltaTime / 100.0f);
    if (currentGravity > 15.0f)
        currentGravity = 15.0f;

    float deltaY = currentGravity * (deltaTime / 10.0f);
    float nextY = y + deltaY;

    int leftTile = (int)x / tileSize;
    int rightTile = (int)(x + playerWidth - 1) / tileSize;
    int topTile = (int)nextY / tileSize;
    int bottomTile = (int)(nextY + playerHeight - 1) / tileSize;

    bool hitY = TileCollision(topTile, bottomTile, leftTile, rightTile, level);

    if (hitY)
    {
        if (deltaY > 0)
        {
            y = (bottomTile * tileSize) - playerHeight;
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

            int tileId = level.Collision(r * tileSize, c * tileSize);

            switch (tileId)
            {
                case (int)TileType::Spike:
                {
                    int leftX = x;
                    int rightX = x + playerWidth - 1;
                    int centerX = x + (playerWidth / 2);

                    int bottomY = y + playerHeight - 1;
                    int centerY = y + (playerHeight / 2);

                    // if (level.SpikeColision(bottomY, leftX, r, c) || level.SpikeColision(bottomY, rightX, r, c) ||
                    //     level.SpikeColision(bottomY, centerX, r, c) || level.SpikeColision(centerY, leftX, r, c) ||
                    //     level.SpikeColision(centerY, rightX, r, c))
                    //{
                    //     playerStatus = Dead;
                    // }
                    continue;
                }

                case (int)TileType::Portal:
                    Game::currentLevelID++;
                    Game::updateLevel = true;
                    // playerStatus = Dead;
                    return false;

                case (int)TileType::Water:
                {
                    if (level.currentDay == timeOfDay::Day)
                    {
                        continue;
                    }
                    break;
                }

                case (int)TileType::MoonBlock:
                    if (level.currentDay == timeOfDay::Night)
                    {
                        return true;
                    }
                    continue;

                case (int)TileType::Flag:
                case (int)TileType::Sun:
                case (int)TileType::Moon:
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
