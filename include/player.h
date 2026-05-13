#pragma once

namespace Tmpl8
{
class Surface;
class Level;

enum PlayerState
{
    Alive,
    Dead
};

class Player
{
  public:
    Player(int startY, int startX, float startSpeed = 3.0f, int startDirection = 0, float startGravity = 2.5f);

    void Update(float deltaTime, Level& level, bool& leftPressed, bool& rightPressed, bool& upPressed);
    void Draw(Surface* gameScreen);
    void Kill();
    void ResetPlayerValues(int newY, int newX);
    void BounceOffObject();
    void ChangeSpawnPosition(int newY, int newX);

    int GetX() const { return (int)x; }
    int GetY() const { return (int)y; }
    bool IsDead() const { return (playerStatus == PlayerState::Dead); }
    int DeathCount() { return totalDeaths; }

  private:
    void UpdateTimers(float deltaTime, Level& level);
    void UpdateY(float deltaTime, Level& level, bool& jumpjumpPressed);
    void UpdateX(float deltaTime, Level& level, float moveX);
    void ProcessInput(float& moveX, bool& jumpPressed, bool& leftPressed, bool& rightPressed, bool& upPressed);
    bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);

    float x, y;
    float currentGravity = -1.0f, speed, gravity, jumptime = 0.3f, coyotetime = 1.0f;
    int direction, jumpAmount = 0, playerWidth, playerHeight, tileSize = 32;
    int spawnY = 0, spawnX = 0;
    int totalDeaths = 0;
    bool grounded = false, jumplastframe = false;

    PlayerState playerStatus = Alive;
};
} // namespace Tmpl8