#include <algorithm>
#include <cmath>
#include <cstdio>

#include "../include/game.h"
#include "../include/level.h"
#include "../include/player.h"
#include "../tmpl8/surface.h"
#include <windows.h>

namespace Tmpl8
{
extern Sprite playerSprite;
extern int screenHeight, screenWidth;

enum PlayerState
{
    Alive,
    Dead
};

PlayerState playerStatus = Alive;

Player::Player(float iy, float ix, float ispeed, int idirection, float igravity)
{
    y = iy;
    x = ix;
    spawnX = x;
    spawnY = y;
    speed = ispeed;
    direction = idirection;
    gravity = igravity;
}

/// <summary>
/// Manages the player's overall state, movement, and lifecycle for the current frame.
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
void Player::Update(float deltaTime, Level& level)
{
    playerWidth = playerSprite.GetWidth();
    playerHeight = playerSprite.GetHeight();

    bool jumpPressed = false;
    float moveX = 0;

    ProcessInput(moveX, jumpPressed);

    UpdateX(deltaTime, level, moveX);

    x = std::clamp(x, 0.0f, (float)screenWidth - playerWidth);

    UpdateY(deltaTime, level, jumpPressed);

    UpdateTimers(deltaTime, level);

    if (playerStatus == Dead)
    {
        playerStatus = Alive;
        deaths++;
        x = spawnX;
        y = spawnY;
    }
}

/// <summary>
/// Reads player input to determine intended movement and actions.
/// </summary>
/// <param name="moveX"></param>
/// <param name="jumpPressed"></param>
void Player::ProcessInput(float& moveX, bool& jumpPressed)
{
    int jumpmask = 0X8000;

    if (GetAsyncKeyState(VK_LEFT))
        moveX = -speed;
    if (GetAsyncKeyState(VK_RIGHT))
        moveX = speed;

    jumpPressed = GetAsyncKeyState(VK_UP) & jumpmask;
}

/// <summary>
/// Handles horizontal movement and prevents moving through walls.
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
/// <param name="moveX"></param>
void Player::UpdateX(float deltaTime, Level& level, float moveX)
{
    float deltaX = moveX * (deltaTime / 10.0f);
    float nextX = x + deltaX;

    int leftTile = (int)nextX / tileSize;
    int rightTile = (int)(nextX + playerWidth - 1) / tileSize;
    int topTile = (int)y / tileSize;
    int bottomTile = (int)(y + playerHeight - 1) / tileSize;

    bool hitX = TileCollision(topTile, bottomTile, leftTile, rightTile, level);

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

/// <summary>
/// Handles vertical movement, gravity, jumping, and floor/ceiling collisions.
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
/// <param name="jumpPressed"></param>
void Player::UpdateY(float deltaTime, Level& level, bool& jumpPressed)
{
    if (jumpPressed)
    {
        if (!jumplastframe && (grounded || jumpAmount > 0))
        {
            currentGravity = -6.0f;
            if (!(grounded || coyotetime >= 0))
                jumpAmount--;
            grounded = false;
        }
        jumplastframe = true;
    }
    else
    {
        jumplastframe = false;
    }

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
            jumptime = 0.3f;
            jumpAmount = 1;
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

/// <summary>
/// Checks if a specified area intersects with solid or interactive level tiles.
/// </summary>
/// <param name="topTile"></param>
/// <param name="bottomTile"></param>
/// <param name="leftTile"></param>
/// <param name="rightTile"></param>
/// <param name="level"></param>
/// <returns>True if collision ocures false if not</returns>
bool Player::TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level)
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

                    if (level.SpikeColision(bottomY, leftX, r, c) || level.SpikeColision(bottomY, rightX, r, c) ||
                        level.SpikeColision(bottomY, centerX, r, c) || level.SpikeColision(centerY, leftX, r, c) ||
                        level.SpikeColision(centerY, rightX, r, c))
                    {
                        playerStatus = Dead;
                    }
                    continue;
                }

                case (int)TileType::Portal:
                    Game::currentLevelID++;
                    Game::updateLevel = true;
                    playerStatus = Dead;
                    return false;

                    // case (int)TileType::Water:
                    // case (int)TileType::Flag:
                    //     continue;
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
/// Updates internal state timers for player mechanics (e.g., jump grace periods).
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
void Player::UpdateTimers(float deltaTime, Level& level)
{
    grounded = 0 < level.Collision(y + playerHeight * 1.25, x) ||
               0 < level.Collision(y + playerHeight * 1.25, x + playerWidth - 1);

    if (grounded)
    {
        coyotetime = 1;
    }
    coyotetime -= deltaTime / 100;
}

/// <summary>
/// Renders the player sprite onto the screen.
/// </summary>
/// <param name="gameScreen"></param>
void Player::Draw(Surface* gameScreen)
{
    playerSprite.Draw(gameScreen, x, y); 
}
} // namespace Tmpl8