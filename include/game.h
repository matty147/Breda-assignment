#pragma once
#include "../third_party/miniaudio.h"

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

    /// <summary>Sets the screen surface used as the render target.</summary>
    /// <param name="surface">Target surface to draw into.</param>
    void SetTarget(Surface* surface) { screen = surface; }

    /// <summary>Initializes audio, loads the first level, and spawns the player and enemies.</summary>
    void Init();

    /// <summary>Shuts down the audio engine and releases its resources.</summary>
    void Shutdown();

    /// <summary>Main game loop tick: updates world state, runs collision, and renders the frame.</summary>
    /// <param name="deltaTime">Frame delta time in milliseconds.</param>
    void Tick(float deltaTime);

    /// <summary>Advances the level counter and loads the next level.</summary>
    void GoToNextLevel();

    /// <summary>Resets the current level state and respawns the player and enemies.</summary>
    void ResetLevel();

    /// <summary>Handles a mouse button release event (currently unused).</summary>
    /// <param name="button">Mouse button identifier.</param>
    void MouseUp(int button) { /* implement if you want to detect mouse button presses */ }

    /// <summary>Handles a mouse button press event (currently unused).</summary>
    /// <param name="button">Mouse button identifier.</param>
    void MouseDown(int button) { /* implement if you want to detect mouse button presses */ }

    /// <summary>Handles mouse cursor movement (currently unused).</summary>
    /// <param name="x">Cursor X position.</param>
    /// <param name="y">Cursor Y position.</param>
    void MouseMove(int x, int y) { /* implement if you want to detect mouse movement */ }

    /// <summary>Handles a key release event.</summary>
    /// <param name="key">HID usage code of the released key.</param>
    void KeyUp(int key);

    /// <summary>Handles a key press event.</summary>
    /// <param name="key">HID usage code of the pressed key.</param>
    void KeyDown(int key);

    /// <summary>Partitions entities into spatial buckets and runs collision checks.</summary>
    /// <param name="gameScreen">Screen surface (currently unused).</param>
    void SpatialHashing(Surface* gameScreen);

    /// <summary>Checks player-vs-entity collisions across neighbouring buckets.</summary>
    /// <param name="bucketYSize">Height of one bucket in pixels.</param>
    /// <param name="bucketXSize">Width of one bucket in pixels.</param>
    void CheckEntityCollision(int bucketYSize, int bucketXSize);

  private:
    /// <summary>Fills the TGA header used when saving a screenshot.</summary>
    /// <param name="gameScreen">Screen surface whose dimensions are used.</param>
    void DefineScreenshotParameters(Surface* gameScreen);

    /// <summary>Writes the current screen to a timestamped .tga file in the working directory.</summary>
    void TakeScreenshot();

    /// <summary>Tests whether two axis-aligned bounding boxes overlap.</summary>
    /// <param name="box1X">Box 1 left X.</param>
    /// <param name="box1Y">Box 1 top Y.</param>
    /// <param name="box1Width">Box 1 width.</param>
    /// <param name="box1Height">Box 1 height.</param>
    /// <param name="box2X">Box 2 left X.</param>
    /// <param name="box2Y">Box 2 top Y.</param>
    /// <param name="box2Width">Box 2 width.</param>
    /// <param name="box2Height">Box 2 height.</param>
    /// <returns>True if the two boxes overlap, false otherwise.</returns>
    bool IsOverlapping(int box1X, int box1Y, int box1Width, int box1Height, int box2X, int box2Y, int box2Width, int box2Height);

    bool leftPressed = false, rightPressed = false, upPressed = false, screenshotPressed = false;
    bool screenshotLastFrame = false;
    int moveX = 0;

    Surface* screen;
    std::vector<std::vector<Bucket>> grid;
};
}; // namespace Tmpl8