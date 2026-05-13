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

class Player: public Entity
{
  public:
    Player(int startY, int startX, float startSpeed = 3.0f, int startDirection = 0, float startGravity = 2.5f);

    void Update(float deltaTime, Level& level, bool& leftPressed, bool& rightPressed, bool& upPressed);
    void Draw(Surface* gameScreen);
    void Kill();
    void ResetPlayerValues(int newY, int newX);
    void BounceOffObject();
    void ChangeSpawnPosition(int newY, int newX);

    bool IsDead() const { return (playerStatus == PlayerState::Dead); }
    int DeathCount() { return totalDeaths; }

  private:
    void UpdateTimers(float deltaTime, Level& level);
    void UpdateY(float deltaTime, Level& level, bool& jumpjumpPressed);
    void UpdateX(float deltaTime, Level& level, float moveX);
    void ProcessInput(float& moveX, bool& jumpPressed, bool& leftPressed, bool& rightPressed, bool& upPressed);
    bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);

    float jumptime = 0.3f, coyotetime = 1.0f;
    int jumpAmount = 0;
    int spawnY = 0, spawnX = 0;
    int totalDeaths = 0;
    bool jumplastframe = false;

    PlayerState playerStatus = Alive;
};
} // namespace Tmpl8