#include "game.h"
#include "tmpl8/surface.h"
#include <cstdio>
#include <algorithm>
#include <vector>

#include "player.h"
#include "level.h"

#include <iostream>
#include <fstream>
#include <string>

namespace Tmpl8
{
	Sprite Playersprite{ new Surface("assets/sprPlayer.png"), 1 }; // can be either
	Sprite Tilessprite{ new Surface("assets/nc2tiles.png"), 1 }; // can be either
	//Sprite Playersprite{ new Surface("tmpl8/assets/sprPlayer.png"), 1 }; // can be either

	int ScreenHeight = 0, ScreenWidth = 0; 

	int currentScreen = 0;

	Level level(100, 100);
	Player myPlayer(0, 0);

	int PlayerX = 0, PlayerY = 0;

	void Game::Init()
	{
		ScreenWidth = screen->GetWidth();
		ScreenHeight = screen->GetHeight();
		level.CreateLevel();
		level.FindFlag(PlayerY,PlayerX);

		myPlayer = Player(PlayerY, PlayerX);

		printf("Y: %d. X: %d", PlayerY, PlayerX);
	}

	void Game::Shutdown()
	{
	}

	void Game::Tick(float deltaTime)
	{
		screen->Clear(0);

		level.Draw(screen);

		myPlayer.Move(deltaTime,level);
		myPlayer.Draw(screen);
	}
};