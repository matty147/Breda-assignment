#pragma once
#include "../include/miniaudio.h"

#include <vector>

namespace Tmpl8
{

class Surface;

struct Bucket
{
    std::vector<int> entityIDs;
};

#pragma pack(push, 1)
struct TGAHeader
{
    unsigned char ID, colmapt;
    unsigned char type;
    unsigned char colmap[5];
    unsigned short xorigin, yorigin;
    unsigned short width, height;
    unsigned char bpp;
    unsigned char idesc;
};
#pragma pack(pop)

class Game
{
  public:
    static int currentLevelID;
    static bool updateLevel;

    void SetTarget(Surface* surface) { screen = surface; }
    void Init();
    void Shutdown();
    void Tick(float deltaTime);
    void GoToNextLevel();
    void ResetLevel();
    void MouseUp(int button) { /* implement if you want to detect mouse button presses */ }
    void MouseDown(int button) { /* implement if you want to detect mouse button presses */ }
    void MouseMove(int x, int y) { /* implement if you want to detect mouse movement */ }
    void KeyUp(int key);
    void KeyDown(int key);

    void SpatialHashing(Surface* gameScreen);
    void CheckEntityCollision(int bucketYSize, int bucketXSize);

  private:
    ma_engine audioEngine;
    ma_sound deathSound;
    ma_sound jumpSound;

    void DefineScreenshotParameters(Surface* gameScreen);
    void TakeScreenshot();
    bool IsOverlapping(int box1X, int box1Y, int box1Width, int box1Height, int box2X, int box2Y, int box2Width, int box2Height);

    bool leftPressed = false, rightPressed = false, upPressed = false, screenshotPressed = false;
    int moveX = 0;

    Surface* screen;
    std::vector<std::vector<Bucket>> grid;
};
}; // namespace Tmpl8