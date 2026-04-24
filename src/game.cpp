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

    int playerX = 0, playerY = 0;

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

        int newY = 0, newX = 0;

        printf("Loading level: %d\n", Game::currentLevelID);
        level.CreateLevel(levelNames[Game::currentLevelID]);
        level.FindFlag(newY, newX);

        level.currentDay = timeOfDay::Day;

        // reset the level
        myPlayer.ResetPlayerValues(newY, newX);
        entities.clear();

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
            float& enemyX = entities[entityId].x;
            float& enemyY = entities[entityId].y;

            int hitboxWidth = 24;
            int hitboxHeight = 3;

            int playerFeetY = myPlayer.y + 30;
            int playerWidth = 24;
            int playerFeetHeight = 2;

            // is buggy
            //if (isOverlapping(myPlayer.x, playerFeetY, playerWidth, playerFeetHeight,
            //                  enemyX - 5, enemyY, hitboxWidth, hitboxHeight))
            //{
            //    printf("touched him\n");
            //    
            //    enemyX = -100;
            //    enemyY = -100;
            //    continue;

            //}

            float xdist = enemyX - myPlayer.x;
            float ydist = enemyY - myPlayer.y;

            float distSq = (xdist * xdist) + (ydist * ydist);
            float radiusSum = 12.0f + 12.0f; // no magic numbers actualy fetch the player and entity sprite sizes
            if (distSq < (radiusSum * radiusSum))
            {
                myPlayer.Kill();                 // temp function
            }
        }
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
