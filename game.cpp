#include "game.h"
#include "tmpl8/surface.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "enemy.h"
#include "level.h"
#include "player.h"

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

int PlayerX = 0, PlayerY = 0;

std::vector<std::string> levelNames = {"level1", "level2", "level3", "level4"};

int Game::currentLevelID = 0;

void Game::Init()
{
    screenWidth = screen->GetWidth();
    screenHeight = screen->GetHeight();
    level.CreateLevel(levelNames[Game::currentLevelID]);
    level.FindFlag(PlayerY, PlayerX);

    myPlayer = Player(PlayerY, PlayerX);

    printf("Y: %d. X: %d", PlayerY, PlayerX);
}

void Game::Shutdown() {}

void Game::Tick(float deltaTime)
{
    screen->Clear(0);

    level.Draw(screen);

    myPlayer.Update(deltaTime, level);
    myEnemy.Update(deltaTime, level);
    myPlayer.Draw(screen);
    myEnemy.Draw(screen);

    // if (Game::currentLevelID <= 1)
    //{
    //	level.CreateLevel(levelNames[Game::currentLevelID]);
    //	level.FindFlag(PlayerY,PlayerX);
    // }

    // printf("%d\n", Game::currentLevelID);
}
}; // namespace Tmpl8
