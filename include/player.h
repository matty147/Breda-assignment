#pragma once

#include "entity.h"

namespace Tmpl8
{
class Surface;
class Level;

enum PlayerState
{
    Alive,
    Dead
};

class Player : public Entity
{
  public:
    /// <summary>Constructs a player at the given spawn position.</summary>
    /// <param name="startY">Initial Y position in pixels.</param>
    /// <param name="startX">Initial X position in pixels.</param>
    /// <param name="startSpeed">Horizontal movement speed.</param>
    /// <param name="startDirection">Initial facing direction.</param>
    /// <param name="startGravity">Per-frame gravity acceleration.</param>
    Player(int startY, int startX, float startSpeed = 3.0f, int startDirection = 0, float startGravity = 2.5f);

    /// <summary>Updates the player for one frame: input, movement, collisions, timers.</summary>
    /// <param name="deltaTime">Frame delta time in milliseconds.</param>
    /// <param name="level">Current level for tile lookups.</param>
    /// <param name="leftPressed">True while the left key is held.</param>
    /// <param name="rightPressed">True while the right key is held.</param>
    /// <param name="upPressed">True while the jump key is held.</param>
    void Update(float deltaTime, Level& level, bool& leftPressed, bool& rightPressed, bool& upPressed);

    /// <summary>Renders the player sprite to the given surface.</summary>
    /// <param name="gameScreen">Target screen surface.</param>
    void Draw(Surface* gameScreen);

    /// <summary>Marks the player as dead so the level can reset.</summary>
    void Kill();

    /// <summary>Restores the player to the alive state at the given position.</summary>
    /// <param name="newY">New Y position in pixels.</param>
    /// <param name="newX">New X position in pixels.</param>
    void ResetPlayerValues(int newY, int newX);

    /// <summary>Applies an upward force, as if the player just jumped on something.</summary>
    void BounceOffObject();

    /// <summary>Records a new spawn position used on respawn.</summary>
    /// <param name="newY">Spawn Y position in pixels.</param>
    /// <param name="newX">Spawn X position in pixels.</param>
    void ChangeSpawnPosition(int newY, int newX);

    /// <summary>Returns true if the player is currently dead.</summary>
    bool IsDead() const { return (playerStatus == PlayerState::Dead); }

    /// <summary>Returns how many times the player has died this session.</summary>
    int DeathCount() { return totalDeaths; }

  private:
    /// <summary>Updates ground-state and coyote-time timers.</summary>
    /// <param name="deltaTime">Frame delta time in milliseconds.</param>
    /// <param name="level">Current level for tile lookups.</param>
    void UpdateTimers(float deltaTime, Level& level);

    /// <summary>Handles vertical movement: gravity, jumping, and floor/ceiling collisions.</summary>
    /// <param name="deltaTime">Frame delta time in milliseconds.</param>
    /// <param name="level">Current level for tile lookups.</param>
    /// <param name="jumpjumpPressed">True if the jump key is held this frame.</param>
    void UpdateY(float deltaTime, Level& level, bool& jumpjumpPressed);

    /// <summary>Handles horizontal movement and wall collisions.</summary>
    /// <param name="deltaTime">Frame delta time in milliseconds.</param>
    /// <param name="level">Current level for tile lookups.</param>
    /// <param name="moveX">Signed horizontal velocity for this frame.</param>
    void UpdateX(float deltaTime, Level& level, float moveX);

    /// <summary>Translates raw key state into intended movement and jump flags.</summary>
    /// <param name="moveX">Out: signed horizontal velocity.</param>
    /// <param name="jumpPressed">Out: true if a jump was requested.</param>
    /// <param name="leftPressed">True if the left key is held.</param>
    /// <param name="rightPressed">True if the right key is held.</param>
    /// <param name="upPressed">True if the jump key is held.</param>
    void ProcessInput(float& moveX, bool& jumpPressed, bool& leftPressed, bool& rightPressed, bool& upPressed);

    /// <summary>Checks the rectangle defined by the given tile bounds for solid or interactive tiles.</summary>
    /// <param name="topTile">Top tile row (inclusive).</param>
    /// <param name="bottomTile">Bottom tile row (inclusive).</param>
    /// <param name="leftTile">Left tile column (inclusive).</param>
    /// <param name="rightTile">Right tile column (inclusive).</param>
    /// <param name="level">Current level for tile lookups.</param>
    /// <returns>True if a solid tile was hit.</returns>
    bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);

    float jumptime = 0.3f, coyotetime = 1.0f;
    int jumpAmount = 0;
    int spawnY = 0, spawnX = 0;
    int totalDeaths = 0;
    bool jumplastframe = false;

    PlayerState playerStatus = Alive;
};
} // namespace Tmpl8