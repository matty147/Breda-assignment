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
    Player(float iy, float ix, float ispeed = 3.0f, int idirection = 0, float igravity = 2.5f);

    void Update(float deltaTime, Level& level, bool& leftPressed, bool& rightPressed, bool& upPressed);
    void Draw(Surface* gameScreen);
    void Kill();
    void ResetPlayerValues(float newY, float newX);
    void BounceOffObject();
    void ChangeSpawnPosition(float newY, float newX);

    float GetX() const { return x; }
    float GetY() const { return y; }
    bool IsDead() const { return (playerStatus == PlayerState::Dead);}
    int DeadthAmout() { return totalDeaths; }

  private:
    void UpdateTimers(float deltaTime, Level& level);
    void UpdateY(float deltaTime, Level& level, bool& jumpjumpPressed);
    void UpdateX(float deltaTime, Level& level, float moveX);
    void ProcessInput(float& moveX, bool& jumpPressed, bool& leftPressed, bool& rightPressed, bool& upPressed);
    bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);

    float x, y, currentGravity = -1, speed, gravity, jumptime = 0.3f, coyotetime;
    int direction, jumpAmount = 0, playerWidth, playerHeight, tileSize = 32, spawnY = 0, spawnX = 0, totalDeaths = 0;
    bool grounded = false, jumplastframe = false;

    PlayerState playerStatus = Alive;
};
} // namespace Tmpl8