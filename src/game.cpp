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

int numberOfEntities = 5;

std::vector<Enemy> entities;

int playerX = 0, playerY = 0;

std::vector<std::string> levelNames = {"level1", "level2"};

int Game::currentLevelID = 0;
bool Game::updateLevel = false;

int seperateX = 5;
int seperateY = 5;

void Game::Init()
{
    for (int i = 0; i < numberOfEntities; i++)
    {
        Enemy newEnemy(100, 100 + i * 50);

        entities.push_back(newEnemy);
    }

    grid.resize(seperateX, std::vector<Bucket>(seperateY));

    screenWidth = screen->GetWidth();
    screenHeight = screen->GetHeight();
    level.CreateLevel(levelNames[Game::currentLevelID]);
    level.FindFlag(playerY, playerX);

    myPlayer = Player(playerY, playerX);

    printf("Y: %d. X: %d", playerY, playerX);
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

        printf("Loading level: %d\n", Game::currentLevelID);
        level.CreateLevel(levelNames[Game::currentLevelID]);
        level.FindFlag(playerY, playerX);
    }
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
    // todo: also clamp
    int playerGridX = myPlayer.x / bucketXSize;
    int playerGridY = myPlayer.y / bucketYSize;

    for (auto& row : grid)
    {
        for (auto& currentBucket : row)
        {
            int sizeOfBucket = currentBucket.entityIDs.size();

            if (sizeOfBucket >= 2)
            {
                // mabey put into a function
                for (auto& firstEntity : currentBucket.entityIDs)
                {
                    for (auto& secondEntity : currentBucket.entityIDs)
                    {
                        int firstEnemyX = entities[firstEntity].x;
                        int firstEnemyY = entities[firstEntity].y;

                        int secondEnemyX = entities[secondEntity].x;
                        int secondEnemyY = entities[secondEntity].y;

                        float xdist = secondEnemyX - firstEnemyX;
                        float ydist = secondEnemyY - firstEnemyY;

                        float distSq = (xdist * xdist) + (ydist * ydist);
                        float radiusSum = 12.0f + 12.0f; // no magic numbers actualy fetch the player and entity sprite sizes
                        if (distSq < (radiusSum * radiusSum))
                        {

                        }
                    }
                }
            }
        }
    }

    auto& EntitiesInPlayersBucket = grid[playerGridY][playerGridX].entityIDs;

    if (EntitiesInPlayersBucket.size() >= 1)
    {

        for (auto& entityId : EntitiesInPlayersBucket)
        {
            int enemyX = entities[entityId].x;
            int enemyY = entities[entityId].y;

            float xdist = enemyX - myPlayer.x;
            float ydist = enemyY - myPlayer.y;

            float distSq = (xdist * xdist) + (ydist * ydist);
            float radiusSum = 12.0f + 12.0f; // no magic numbers actualy fetch the player and entity sprite sizes
            if (distSq < (radiusSum * radiusSum))
            {
                printf("collision with object"); // call player to deal with it
                myPlayer.Kill();                 // temp function
            }
        }
    }
}

}; // namespace Tmpl8
