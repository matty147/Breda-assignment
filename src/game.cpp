#include <algorithm>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

#define MINIAUDIO_IMPLEMENTATION

#include "../third_party/miniaudio.h"

#include "../include/enemy.h"
#include "../include/game.h"
#include "../include/level.h"
#include "../include/player.h"
#include "../tmpl8/surface.h"

namespace Tmpl8
{
Sprite playerSprite{new Surface("assets/sprites/sprPlayer.png"), 1};
Sprite tilesSprite{new Surface("assets/sprites/nc2tiles.png"), 1};
Sprite enemySprite{new Surface("assets/sprites/sprEnemy.png"), 1};

ma_engine audioEngine;
const char* deathSound = "assets/audio/deathSound.wav";
const char* jumpSound = "assets/audio/jumpSound.wav";
const char* timeChangeSound = "assets/audio/timeChangeSound.wav";
const char* enemyDeathSound = "assets/audio/enemyDeathSound.wav";
const char* levelClearedSound = "assets/audio/levelClearedSound.wav";
const char* screenshotSound = "assets/audio/screenshotSound.wav";

constexpr int KEY_W = 26, KEY_UP = 82;
constexpr int KEY_A = 4, KEY_LEFT = 80;
constexpr int KEY_D = 7, KEY_RIGHT = 79;
constexpr int KEY_P = 19;

Level level(100, 100);
Player myPlayer(0, 0);
TGAHeader header;

int screenHeight = 0, screenWidth = 0;

std::vector<Enemy> entities;
std::vector<std::vector<int>> entitySpawnPoints;

const std::vector<std::string> levelNames = {"level1", "level2", "level3"};

int Game::currentLevelID = 0;
bool Game::updateLevel = false;

// amout of collision buckets per row/column on screen
const int bucketCountX = 10;
const int bucketCountY = 10;

std::vector<std::vector<int>> vineList; // [y,x] coordinates of the stumps of vines

/// <summary>
/// Initializes audio, loads the first level, and spawns the player and enemies.
/// </summary>
void Game::Init()
{
    if (ma_engine_init(NULL, &audioEngine) != MA_SUCCESS)
    {
        printf("Failed to initialize audio engine.\n");
    }

    DefineScreenshotParameters(screen);

    grid.resize(bucketCountY, std::vector<Bucket>(bucketCountX));

    screenWidth = screen->GetWidth();
    screenHeight = screen->GetHeight();

    int playerX = 0, playerY = 0;

    level.CreateLevel(levelNames[Game::currentLevelID], entitySpawnPoints);
    level.FindFlag(playerY, playerX);
    level.FindTileInstances(vineList, (int)TileType::VineStump);

    myPlayer = Player(playerY, playerX);
    myPlayer.ChangeSpawnPosition(playerY, playerX);

    entities.reserve(entitySpawnPoints.size());

    for (auto& spawnPoint : entitySpawnPoints)
    {
        Enemy newEnemy(spawnPoint[0], spawnPoint[1]);

        entities.push_back(newEnemy);
    }
}

/// <summary>
/// Shuts down the audio engine and releases its resources.
/// </summary>
void Game::Shutdown()
{
    ma_engine_uninit(&audioEngine);
}

/// <summary>
///  Main Game loop
/// </summary>
/// <param name="deltaTime"></param>
void Game::Tick(float deltaTime)
{
    screen->Clear(0);

    const float deltaTimeMaximumDelay = 33.0f;

    deltaTime = (std::min)(deltaTime, deltaTimeMaximumDelay); // stop the objects from lagging through the floor

    level.UpdateVines(vineList);

    level.Draw(screen, deltaTime);

    SpatialHashing(screen);

    myPlayer.Update(deltaTime, level, leftPressed, rightPressed, upPressed);
    myPlayer.Draw(screen);

    for (auto& entity : entities)
    {
        entity.Update(deltaTime, level);
        entity.Draw(screen);
    }

    if (myPlayer.IsDead())
    {
        printf("deaths: %d\n", myPlayer.DeathCount());
        ResetLevel();
    }

    if (updateLevel)
    {
        ma_engine_play_sound(&audioEngine, levelClearedSound, NULL);

        updateLevel = false;

        GoToNextLevel();
        ResetLevel();
    }

    if (screenshotPressed && !screenshotLastFrame)
    {
        ma_engine_play_sound(&audioEngine, screenshotSound, NULL);
        TakeScreenshot();
    }

    screenshotLastFrame = screenshotPressed;
}

/// <summary>
///  Reset values and loads the next level
/// </summary>
void Game::GoToNextLevel()
{
    Game::currentLevelID++;
    Game::currentLevelID = std::clamp(Game::currentLevelID, 0, (int)levelNames.size() - 1);
    printf("Loading level: %d\n", Game::currentLevelID);
    level.CreateLevel(levelNames[Game::currentLevelID], entitySpawnPoints);

    vineList.clear();
}

/// <summary>
/// reset all values and entities
/// </summary>
void Game::ResetLevel()
{
    int newY = 0, newX = 0;

    level.currentTime = TimeOfDay::Night;
    level.UpdateVines(vineList);
    level.currentTime = TimeOfDay::Day;

    level.FindTileInstances(vineList, (int)TileType::VineStump);

    level.UpdateVines(vineList);

    level.FindFlag(newY, newX);

    // reset the level
    myPlayer.ResetPlayerValues(newY, newX);
    myPlayer.ChangeSpawnPosition(newY, newX);
    entities.clear();

    for (auto& spawnPoint : entitySpawnPoints)
    {
        Enemy newEnemy(spawnPoint[0], spawnPoint[1]);

        entities.push_back(newEnemy);
    }
}

/// <summary>
///  separate entities into sectors for collision detetion
/// </summary>
/// <param name="gameScreen"></param>
void Game::SpatialHashing(Surface* gameScreen)
{
    int bucketXSize = screenWidth / bucketCountX;
    int bucketYSize = screenHeight / bucketCountY;

    for (int y = 0; y < bucketCountY; y++)
    {
        for (int x = 0; x < bucketCountX; x++)
        {
            grid[y][x].entityIDs.clear();
        }
    }

    for (int i = 0; i < entities.size(); i++)
    {
        int x = std::clamp(entities[i].GetX() / bucketXSize, 0, bucketCountX - 1);
        int y = std::clamp(entities[i].GetY() / bucketYSize, 0, bucketCountY - 1);

        grid[y][x].entityIDs.push_back(i);
    }

    if (!entities.empty())
    {
        CheckEntityCollision(bucketYSize, bucketXSize);
    }
}

/// <summary>
/// check for entities collision in each sector of the screen
/// </summary>
/// <param name="bucketYSize"></param>
/// <param name="bucketXSize"></param>
void Game::CheckEntityCollision(int bucketYSize, int bucketXSize)
{
    int playerSpriteWidth = playerSprite.GetWidth();
    int playerSpriteHeight = playerSprite.GetHeight();
    int enemySpriteWidth = enemySprite.GetWidth();

    int playerGridX = std::clamp((int)myPlayer.GetX() / bucketXSize, 0, bucketCountX - 1);
    int playerGridY = std::clamp((int)myPlayer.GetY() / bucketYSize, 0, bucketCountY - 1);

    auto& entitiesInPlayersBucket = grid[playerGridY][playerGridX].entityIDs;

    std::vector<int> deadEntities;

    int neighborOffsets[4][2] = {
        {1, 0},
        {-1, 1},
        {0, 1},
        {1, 1}};

    // player
    int hitboxWidth = playerSpriteWidth;
    int hitboxHeight = playerSpriteHeight / 8;
    int playerFeetY = (int)myPlayer.GetY() + playerSpriteHeight;
    int playerFeetHeight = 6;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int neighborX = playerGridX + i;
            int neighborY = playerGridY + j;

            if (neighborX >= 0 && neighborX < bucketCountX && neighborY >= 0 && neighborY < bucketCountY)
            {
                auto& neighborBucket = grid[neighborY][neighborX].entityIDs;

                for (int currentID : neighborBucket)
                {
                    int enemyX = entities[currentID].GetX();
                    int enemyY = entities[currentID].GetY();

                    if (IsOverlapping(myPlayer.GetX(), playerFeetY, playerSpriteWidth, playerFeetHeight,
                                      enemyX - 5, enemyY, hitboxWidth, hitboxHeight))
                    {
                        myPlayer.BounceOffObject();
                        deadEntities.push_back(currentID);
                        continue;
                    }
                    int xdist = enemyX - myPlayer.GetX();
                    int ydist = enemyY - myPlayer.GetY();
                    int distSq = (xdist * xdist) + (ydist * ydist);
                    int radiusSum = playerSpriteWidth / 2 + enemySpriteWidth / 2;
                    if (distSq < (radiusSum * radiusSum))
                    {
                        myPlayer.Kill();
                    }
                }
            }
        }
    }

    std::sort(deadEntities.rbegin(), deadEntities.rend());
    deadEntities.erase(std::unique(deadEntities.begin(), deadEntities.end()), deadEntities.end());

    // destroy enemies marked for death
    for (int d = 0; d < deadEntities.size(); d++)
    {
        ma_engine_play_sound(&audioEngine, enemyDeathSound, NULL);
        entities[deadEntities[d]] = entities.back();
        entities.pop_back();
    }
}

/// <summary>
/// check if two entities are colliding with each other
/// </summary>
/// <param name="box1X"></param>
/// <param name="box1Y"></param>
/// <param name="box1Width"></param>
/// <param name="box1Height"></param>
/// <param name="box2X"></param>
/// <param name="box2Y"></param>
/// <param name="box2Width"></param>
/// <param name="box2Height"></param>
/// <returns></returns>
bool Game::IsOverlapping(int box1X, int box1Y, int box1Width, int box1Height,
                         int box2X, int box2Y, int box2Width, int box2Height)
{
    return (box1X < box2X + box2Width &&  // Box 1's Left edge is left of Box 2's Right edge
            box1X + box1Width > box2X &&  // Box 1's Right edge is right of Box 2's Left edge
            box1Y < box2Y + box2Height && // Box 1's Top edge is above Box 2's Bottom edge
            box1Y + box1Height > box2Y);  // Box 1's Bottom edge is below Box 2's Top edge
}

/// <summary>
/// create and fill data for the .tga header
/// </summary>
/// <param name="gameScreen"></param>
void Game::DefineScreenshotParameters(Surface* gameScreen)
{
    header.ID = header.colmapt = 0;
    header.type = 2;

    for (int i = 0; i < 5; i++)
    {
        header.colmap[i] = 0;
    }

    header.xorigin = header.yorigin = 0;
    header.width = screenWidth;
    header.height = screenHeight;
    header.bpp = 32;
    header.idesc = 0x28;
}

/// <summary>
/// Takes a screenshot of the screen and saves it with the current date the name
/// </summary>
void Game::TakeScreenshot()
{
    std::time_t result = std::time(nullptr);
    std::tm* now = std::localtime(&result);

    char timeBuffer[80];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d_%H-%M-%S", now);

    std::string screenshotName = "screenshot " + std::string(timeBuffer) + ".tga";

    FILE* f = fopen(screenshotName.c_str(), "wb");

    if (f == NULL)
    {
        printf("failed making a screenshot\n");
        return;
    }

    fwrite(&header, sizeof(TGAHeader), 1, f);

    unsigned int* buffer = screen->GetBuffer();

    int buffer_size = screenWidth * screenHeight * 4;

    std::vector<unsigned char> colorBuffer(buffer_size, 0);

    for (int i = 0; i < screenWidth * screenHeight; i++)
    {
        unsigned int c = buffer[i];

        colorBuffer[i * 4 + 2] = (c >> 16) & 255; // R
        colorBuffer[i * 4 + 1] = (c >> 8) & 255;  // G
        colorBuffer[i * 4 + 0] = c & 255;         // B
        colorBuffer[i * 4 + 3] = 255;             // A
    }

    fwrite(colorBuffer.data(), buffer_size, 1, f);

    fclose(f);
}

/// <summary>
/// Handles a key release event and clears the corresponding input flag.
/// </summary>
/// <param name="key">HID usage code of the released key.</param>
void Game::KeyUp(int key)
{
    // Player controls
    if (key == KEY_W || key == KEY_UP)
        upPressed = false;
    if (key == KEY_A || key == KEY_LEFT)
        leftPressed = false;
    if (key == KEY_D || key == KEY_RIGHT)
        rightPressed = false;

    // Screenshots
    if (key == KEY_P)
        screenshotPressed = false;
}

/// <summary>
/// Handles a key press event and sets the corresponding input flag.
/// </summary>
/// <param name="key">HID usage code of the pressed key.</param>
void Game::KeyDown(int key)
{
    // Player constrols
    if (key == KEY_W || key == KEY_UP)
        upPressed = true;
    if (key == KEY_A || key == KEY_LEFT)
        leftPressed = true;
    if (key == KEY_D || key == KEY_RIGHT)
        rightPressed = true;

    // Screenshot
    if (key == KEY_P)
        screenshotPressed = true;
}

}; // namespace Tmpl8
