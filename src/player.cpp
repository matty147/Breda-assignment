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

extern const char* deathSound;
extern const char* jumpSound;
extern const char* timeChangeSound;
extern const char* enemyDeathSound;
extern ma_engine audioEngine;

/// <summary>
/// Constructs a player at the given spawn position with the given movement parameters.
/// </summary>
/// <param name="startY">Initial Y position in pixels.</param>
/// <param name="startX">Initial X position in pixels.</param>
/// <param name="startSpeed">Horizontal movement speed.</param>
/// <param name="startDirection">Initial facing direction.</param>
/// <param name="startGravity">Per-frame gravity acceleration.</param>
Player::Player(int startY, int startX, float startSpeed, int startDirection, float startGravity)
    : Entity(startY, startX, startSpeed, startDirection, startGravity)
{
}

/// <summary>
/// Manages the player's overall state, movement, and lifecycle for the current frame.
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
void Player::Update(float deltaTime, Level& level, bool& leftPressed, bool& rightPressed, bool& upPressed)
{
    width = playerSprite.GetWidth();
    height = playerSprite.GetHeight();

    bool jumpPressed = false;
    float moveX = 0;

    ProcessInput(moveX, jumpPressed, leftPressed, rightPressed, upPressed);

    UpdateX(deltaTime, level, moveX);

    x = std::clamp(x, 0.0f, (float)screenWidth - width);

    UpdateY(deltaTime, level, jumpPressed);

    UpdateTimers(deltaTime, level);
}

/// <summary>
/// Reads player input to determine intended movement and actions.
/// </summary>
/// <param name="moveX"></param>
/// <param name="jumpPressed"></param>
void Player::ProcessInput(float& moveX, bool& jumpPressed, bool& leftPressed, bool& rightPressed, bool& upPressed)
{
    if (leftPressed)
        moveX = -speed;
    if (rightPressed)
        moveX = speed;

    jumpPressed = upPressed;
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
    int rightTile = (int)(nextX + width - 1) / tileSize;
    int topTile = (int)y / tileSize;
    int bottomTile = (int)(y + height - 1) / tileSize;

    bool hitX = TileCollision(topTile, bottomTile, leftTile, rightTile, level);

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
/// Handles vertical movement, gravity, jumping, and floor/ceiling collisions.
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
/// <param name="jumpPressed"></param>
void Player::UpdateY(float deltaTime, Level& level, bool& jumpPressed)
{
    float jumpGravity = -6.0f; // bounce velocity
    float maxGravity = 15.0f;

    if (jumpPressed)
    {
        if (!jumplastframe && (grounded || jumpAmount > 0))
        {
            currentGravity = jumpGravity;
            ma_engine_play_sound(&audioEngine, jumpSound, NULL);
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
    if (currentGravity > maxGravity)
        currentGravity = maxGravity;

    float deltaY = currentGravity * (deltaTime / 10.0f);
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
            jumptime = 0.3f;
            jumpAmount = 1;
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

            int tileId = level.GetTileID(r * tileSize, c * tileSize);

            switch (std::abs(tileId))
            {
                case (int)TileType::Spike:
                {
                    int leftX = (int)x;
                    int rightX = (int)x + width - 1;
                    int centerX = (int)x + (width / 2);

                    int bottomY = (int)y + height - 1;
                    int centerY = (int)y + (height / 2);

                    if (level.SpikeCollision(bottomY, leftX, r, c) || level.SpikeCollision(bottomY, rightX, r, c) ||
                        level.SpikeCollision(bottomY, centerX, r, c) || level.SpikeCollision(centerY, leftX, r, c) ||
                        level.SpikeCollision(centerY, rightX, r, c))
                    {
                        playerStatus = Dead;
                    }
                    continue;
                }

                case (int)TileType::Sun:
                    level.currentTime = TimeOfDay::Day;
                    continue;

                case (int)TileType::Moon:
                    level.currentTime = TimeOfDay::Night;
                    continue;

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

                case (int)TileType::VineStump:
                    continue;

                case (int)TileType::Portal:
                    Game::updateLevel = true;
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
/// Updates internal state timers for player mechanics (e.g., jump grace periods).
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="level"></param>
void Player::UpdateTimers(float deltaTime, Level& level)
{
    float raycastLength = 1.25; // raycasts 1.25x sprite height below
    int under_player = (int)(y + height * raycastLength);

    grounded = 0 < level.GetTileID(under_player, GetX()) || 0 < level.GetTileID(under_player, GetX() + width - 1);

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
    playerSprite.Draw(gameScreen, (int)x, (int)y);
}

/// <summary>
/// Resets the player position and his values
/// </summary>
/// <param name="newY"></param>
/// <param name="newX"></param>
void Player::ResetPlayerValues(int newY, int newX)
{
    if (playerStatus == Dead)
    {
        ma_engine_play_sound(&audioEngine, deathSound, NULL);

        totalDeaths++;
    }

    playerStatus = Alive;

    currentGravity = -1;
    y = (float)newY;
    x = (float)newX;
}

/// <summary>
/// Bounces the player upwards
/// </summary>
void Player::BounceOffObject()
{
    currentGravity = -6;
    jumpAmount = 1;
}

/// <summary>
/// kills the player
/// </summary>
void Player::Kill() { playerStatus = Dead; }

/// <summary>
///  changes where the player will respawn
/// </summary>
/// <param name="newY"></param>
/// <param name="newX"></param>
void Tmpl8::Player::ChangeSpawnPosition(int newY, int newX)
{
    spawnY = newY;
    spawnX = newX;
}

} // namespace Tmpl8