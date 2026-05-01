#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

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

int currentScreen = 0;

Level level(100, 100);
Player myPlayer(0, 0);
Enemy myEnemy(100, 100);

int numberOfEntities = 0;

std::vector<Enemy> entities;
std::vector<std::vector<int>> entitySpawnPoints;

std::vector<std::string> levelNames = {"level1", "level2"};

int Game::currentLevelID = 0;
bool Game::updateLevel = false;

int seperateX = 10;
int seperateY = 10;

std::vector<std::vector<int>> vineList;

void Game::Init()
{
    grid.resize(seperateX, std::vector<Bucket>(seperateY));

    screenWidth = screen->GetWidth();
    screenHeight = screen->GetHeight();

    int playerX = 0, playerY = 0;

    level.CreateLevel(levelNames[Game::currentLevelID], entitySpawnPoints);
    level.FindFlag(playerY, playerX);
    level.FindTileInstances(vineList, (int)TileType::VineStump);

    for (auto& spawnPoint: entitySpawnPoints)
    {
        Enemy newEnemy(spawnPoint[0], spawnPoint[1]);

        entities.push_back(newEnemy);
    }

    myPlayer = Player(playerY, playerX);

    //printf("Y: %d. X: %d", playerY, playerX);
}

void Game::Shutdown() {}

/// <summary>
///  Main Game loop
/// </summary>
/// <param name="deltaTime"></param>
void Game::Tick(float deltaTime)
{
    deltaTime = std::min(deltaTime, 33.0f); // stop the objects from going through the floor

    screen->Clear(0);

    level.UpdateVines(vineList);

    level.Draw(screen);

    myPlayer.Update(deltaTime, level);
    myPlayer.Draw(screen);

    for (auto& entity : entities)
    {
        entity.Update(deltaTime, level);
        entity.Draw(screen);
    }

    SpatialHashing(screen);

    if (updateLevel) // cap the levels so it does not crash
    {
        updateLevel = false;
        Game::currentLevelID = std::clamp(Game::currentLevelID, 0, (int)levelNames.size() - 1);

        int newY = 0, newX = 0;

        printf("Loading level: %d\n", Game::currentLevelID);
        level.CreateLevel(levelNames[Game::currentLevelID], entitySpawnPoints);
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

    // see buckets
    int ScreenWidth = screen->GetWidth();
    int ScreenHeight = screen->GetHeight();

    int bucketXSize = ScreenWidth / seperateX;
    int bucketYSize = ScreenHeight / seperateY;

}

/// <summary>
///  seperate entities into sectors for collision detetion
/// </summary>
/// <param name="gameScreen"></param>
void Game::SpatialHashing(Surface* gameScreen)
{

    int ScreenWidth = gameScreen->GetWidth();
    int ScreenHeight = gameScreen->GetHeight();

    int bucketXSize = ScreenWidth / seperateX;
    int bucketYSize = ScreenHeight / seperateY;

    for (int y = 0; y < seperateY; y++)
    {
        for (int x = 0; x < seperateX; x++)
        {
            grid[y][x].entityIDs.clear();
        }
    }

    for (int i = 0; i < entities.size(); i++)
    {
        int x = std::clamp((int)entities[i].x / bucketXSize, 0, seperateX - 1);
        int y = std::clamp((int)entities[i].y / bucketYSize, 0, seperateY - 1);

        grid[y][x].entityIDs.push_back(i);
    }

    if (!entities.empty() || entities.size() > 1)
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
    int playerGridX = std::clamp((int)myPlayer.x / bucketXSize, 0, seperateX - 1);
    int playerGridY = std::clamp((int)myPlayer.y / bucketYSize, 0, seperateY - 1);

    auto& EntitiesInPlayersBucket = grid[playerGridY][playerGridX].entityIDs;

    std::vector<int> deadEntities;

    int neighborOffsets[4][2] = {
        {1, 0},
        {-1, 1},
        {0, 1},
        {1, 1}};

    for (int r = 0; r < seperateX; r++)
    {
        for (int c = 0; c < seperateY; c++)
        {
            for (int i = 0; i < 4; i++)
            {
                int neighborX = r + neighborOffsets[i][0];
                int neighborY = c + neighborOffsets[i][1];
                if (neighborX >= 0 && neighborX < seperateX && neighborY >= 0 && neighborY < seperateY)
                {
                    // collision for enemies
                    auto& neighborBucket = grid[neighborY][neighborX].entityIDs;
                    auto& CurrentBucket = grid[r][c].entityIDs;
                    for (int currentID : CurrentBucket)
                    {
                        for (int neighborID : neighborBucket)
                        {
                            int firstEnemyX = entities[currentID].x;
                            int firstEnemyY = entities[currentID].y;
                            int secondEnemyX = entities[neighborID].x;
                            int secondEnemyY = entities[neighborID].y;
                            float xdist = secondEnemyX - firstEnemyX;
                            float ydist = secondEnemyY - firstEnemyY;
                            float distSq = (xdist * xdist) + (ydist * ydist);
                            float radiusSum = 12.0f + 12.0f; // no magic numbers actualy fetch the player and entity sprite sizes
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

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int neighborX = playerGridX + i;
            int neighborY = playerGridY + j;

            if (neighborX >= 0 && neighborX < seperateX && neighborY >= 0 && neighborY < seperateY)
            {
                auto& neighborBucket = grid[neighborY][neighborX].entityIDs;

                for (int currentID : neighborBucket)
                {
                    float& enemyX = entities[currentID].x;
                    float& enemyY = entities[currentID].y;
                    int hitboxWidth = 24;
                    int hitboxHeight = 3;
                    int playerFeetY = myPlayer.y + 24;
                    int playerWidth = 24;
                    int playerFeetHeight = 6;

                    if (isOverlapping(myPlayer.x, playerFeetY, playerWidth, playerFeetHeight,
                                      enemyX - 5, enemyY, hitboxWidth, hitboxHeight))
                    {
                        myPlayer.currentGravity = -6;
                        deadEntities.push_back(currentID);
                        continue;
                    }
                    float xdist = enemyX - myPlayer.x;
                    float ydist = enemyY - myPlayer.y;
                    float distSq = (xdist * xdist) + (ydist * ydist);
                    float radiusSum = 12.0f + 12.0f; // no magic numbers actualy fetch the player and entity sprite sizes
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

bool Game::isOverlapping(int box1X, int box1Y, int box1Width, int box1Height,
                         int box2X, int box2Y, int box2Width, int box2Height)
{
    return (box1X < box2X + box2Width &&  // Box 1's Left edge is left of Box 2's Right edge
            box1X + box1Width > box2X &&  // Box 1's Right edge is right of Box 2's Left edge
            box1Y < box2Y + box2Height && // Box 1's Top edge is above Box 2's Bottom edge
            box1Y + box1Height > box2Y);  // Box 1's Bottom edge is below Box 2's Top edge
}

}; // namespace Tmpl8
