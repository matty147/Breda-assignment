#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

#include "../include/enemy.h"
#include "../include/game.h"
#include "../include/level.h"
#include "../include/player.h"
#include "../tmpl8/surface.h"

namespace Tmpl8
{
Sprite playerSprite{new Surface("assets/sprPlayer.png"), 1};
Sprite tilesSprite{new Surface("assets/nc2tiles.png"), 1};
Sprite enemySprite{new Surface("assets/sprEnemy.png"), 1};

int screenHeight = 0, screenWidth = 0;

Level level(100, 100);
Player myPlayer(0, 0);
Enemy myEnemy(100, 100);

std::vector<Enemy> entities;
std::vector<std::vector<int>> entitySpawnPoints;

const std::vector<std::string> levelNames = {"level1", "level2", "level3"};

int Game::currentLevelID = 0;
bool Game::updateLevel = false;

const int separateX = 10;
const int separateY = 10;

std::vector<std::vector<int>> vineList;

TGAHeader header;

void Game::Init()
{
    grid.resize(separateY, std::vector<Bucket>(separateX));

    screenWidth = screen->GetWidth();
    screenHeight = screen->GetHeight();

    int playerX = 0, playerY = 0;

    DefineScreenshotParameters(screen);

    level.CreateLevel(levelNames[Game::currentLevelID], entitySpawnPoints);
    level.FindFlag(playerY, playerX);
    level.FindTileInstances(vineList, (int)TileType::VineStump);

    entities.reserve(entitySpawnPoints.size());

    for (auto& spawnPoint : entitySpawnPoints)
    {
        Enemy newEnemy(spawnPoint[0], spawnPoint[1]);

        entities.push_back(newEnemy);
    }

    myPlayer = Player(playerY, playerX);

    // printf("Y: %d. X: %d", playerY, playerX);
}

void Game::Shutdown() {}

/// <summary>
///  Main Game loop
/// </summary>
/// <param name="deltaTime"></param>
void Game::Tick(float deltaTime)
{
    const float deltaTimeMaximumDelay = 33.0f;

    deltaTime = (std::min)(deltaTime, deltaTimeMaximumDelay); // stop the objects from lagging through the floor

    screen->Clear(0);

    level.UpdateVines(vineList);

    level.Draw(screen, deltaTime);

    myPlayer.Update(deltaTime, level, leftPressed, rightPressed, upPressed);
    myPlayer.Draw(screen);

    if (leftPressed || rightPressed || upPressed)
    {
        printf("l:%d, r:%d, u: %d [game::92]\n", leftPressed, rightPressed, upPressed);
    }
    for (auto& entity : entities)
    {
        entity.Update(deltaTime, level);
        entity.Draw(screen);
    }

    SpatialHashing(screen);

    if (updateLevel) // cap the levels so it does not crash
    {
        updateLevel = false;

        GoToNextLevel();
        ResetLevel();
    }

    if (screenshotPressed)
    {
        TakeScreenshot();
    }
}

void Game::GoToNextLevel()
{
    Game::currentLevelID++;
    Game::currentLevelID = std::clamp(Game::currentLevelID, 0, (int)levelNames.size() - 1);
    printf("Loading level: %d\n", Game::currentLevelID);
    level.CreateLevel(levelNames[Game::currentLevelID], entitySpawnPoints);
}

void Game::ResetLevel()
{
    int newY = 0, newX = 0;

    level.FindFlag(newY, newX);
    level.FindTileInstances(vineList, (int)TileType::VineStump);
    level.currentDay = timeOfDay::Day;

    // reset the level
    myPlayer.ResetPlayerValues(newY, newX);
    entities.clear();

    for (auto& spawnPoint : entitySpawnPoints)
    {
        Enemy newEnemy(spawnPoint[0], spawnPoint[1]);

        entities.push_back(newEnemy);
    }
}

/// <summary>
///  seperate entities into sectors for collision detetion
/// </summary>
/// <param name="gameScreen"></param>
void Game::SpatialHashing(Surface* gameScreen)
{
    int bucketXSize = screenWidth / separateX;
    int bucketYSize = screenHeight / separateY;

    for (int y = 0; y < separateY; y++)
    {
        for (int x = 0; x < separateX; x++)
        {
            grid[y][x].entityIDs.clear();
        }
    }

    for (int i = 0; i < entities.size(); i++)
    {
        int x = std::clamp((int)entities[i].GetX() / bucketXSize, 0, separateX - 1);
        int y = std::clamp((int)entities[i].GetY() / bucketYSize, 0, separateY - 1);

        grid[y][x].entityIDs.push_back(i);
    }

    if (!entities.empty())
    {
        CheckEntityCollision(bucketYSize, bucketXSize);
    }
}

/// <summary>
/// check for collision
/// </summary>
/// <param name="bucketYSize"></param>
/// <param name="bucketXSize"></param>
void Game::CheckEntityCollision(int bucketYSize, int bucketXSize)
{
    int playerSpriteWidth = playerSprite.GetWidth();
    int enemySpriteWidth = enemySprite.GetWidth();

    int playerGridX = std::clamp((int)myPlayer.GetX() / bucketXSize, 0, separateX - 1);
    int playerGridY = std::clamp((int)myPlayer.GetY() / bucketYSize, 0, separateY - 1);

    auto& EntitiesInPlayersBucket = grid[playerGridY][playerGridX].entityIDs;

    std::vector<int> deadEntities;

    int neighborOffsets[4][2] = {
        {1, 0},
        {-1, 1},
        {0, 1},
        {1, 1}};

    for (int r = 0; r < separateX; r++)
    {
        for (int c = 0; c < separateY; c++)
        {
            for (int i = 0; i < 4; i++)
            {
                int neighborX = r + neighborOffsets[i][0];
                int neighborY = c + neighborOffsets[i][1];
                if (neighborX >= 0 && neighborX < separateX && neighborY >= 0 && neighborY < separateY)
                {
                    // collision for enemies
                    auto& neighborBucket = grid[neighborY][neighborX].entityIDs;
                    auto& CurrentBucket = grid[r][c].entityIDs;
                    for (int currentID : CurrentBucket)
                    {
                        for (int neighborID : neighborBucket)
                        {
                            int firstEnemyX = entities[currentID].GetX();
                            int firstEnemyY = entities[currentID].GetY();
                            int secondEnemyX = entities[neighborID].GetX();
                            int secondEnemyY = entities[neighborID].GetY();
                            float xdist = secondEnemyX - firstEnemyX;
                            float ydist = secondEnemyY - firstEnemyY;
                            float distSq = (xdist * xdist) + (ydist * ydist);
                            float radiusSum = enemySpriteWidth + enemySpriteWidth; // no magic numbers actualy fetch the player and entity sprite sizes
                            if (distSq < (radiusSum * radiusSum))
                            {
                                // do smthing
                            }
                        }
                    }
                }
            }
        }
    }

    int hitboxWidth = 24;
    int hitboxHeight = 3;
    int playerFeetY = myPlayer.GetY() + 24;
    int playerFeetHeight = 6;

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int neighborX = playerGridX + i;
            int neighborY = playerGridY + j;

            if (neighborX >= 0 && neighborX < separateX && neighborY >= 0 && neighborY < separateY)
            {
                auto& neighborBucket = grid[neighborY][neighborX].entityIDs;

                for (int currentID : neighborBucket)
                {
                    float enemyX = entities[currentID].GetX();
                    float enemyY = entities[currentID].GetY();

                    if (IsOverlapping(myPlayer.GetX(), playerFeetY, playerSpriteWidth, playerFeetHeight,
                                      enemyX - 5, enemyY, hitboxWidth, hitboxHeight))
                    {
                        myPlayer.BounceOffObject();
                        deadEntities.push_back(currentID);
                        continue;
                    }
                    float xdist = enemyX - myPlayer.GetX();
                    float ydist = enemyY - myPlayer.GetY();
                    float distSq = (xdist * xdist) + (ydist * ydist);
                    float radiusSum = playerSpriteWidth / 2 + enemySpriteWidth / 2; // no magic numbers actualy fetch the player and entity sprite sizes
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
        entities[deadEntities[d]] = entities.back();
        entities.pop_back();
    }
}

bool Tmpl8::Game::IsOverlapping(int box1X, int box1Y, int box1Width, int box1Height,
                                int box2X, int box2Y, int box2Width, int box2Height)
{
    return (box1X < box2X + box2Width &&  // Box 1's Left edge is left of Box 2's Right edge
            box1X + box1Width > box2X &&  // Box 1's Right edge is right of Box 2's Left edge
            box1Y < box2Y + box2Height && // Box 1's Top edge is above Box 2's Bottom edge
            box1Y + box1Height > box2Y);  // Box 1's Bottom edge is below Box 2's Top edge
}

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

void Game::TakeScreenshot()
{
    FILE* f = fopen("screenshot.tga", "wb");
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

// 26 - W 82
//  22 - S 81
//  4 - A 80
//  7 - D 79
// 19 - P

void Game::KeyUp(int key)
{
    // player movement
    if (key == 26 || key == 82)
        upPressed = false;
    if (key == 4 || key == 80) // 'A' and left arrow key
        leftPressed = false;
    if (key == 7 || key == 79) // 'D' and right arrow key
        rightPressed = false;

    // screenshot button
    if (key == 19)
        screenshotPressed = false;
}

void Game::KeyDown(int key)
{
    // player movement
    if (key == 26 || key == 82) // 'W' and up arrow key
        upPressed = true;
    if (key == 4 || key == 80) // 'A' and left arrow key
        leftPressed = true;
    if (key == 7 || key == 79) // 'D' and right arrow key
        rightPressed = true;
    
    // screenshot button
    if (key == 19)
        screenshotPressed = true;
}

}; // namespace Tmpl8
