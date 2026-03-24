#include "game.h"
#include "tmpl8/surface.h"
#include <cstdio>
#include <algorithm>
#include <vector>

#include "player.h"
#include "level.h"

namespace Tmpl8
{
	Sprite Playersprite{ new Surface("assets/sprPlayer.png"), 1 }; // can be either
	Sprite Tilessprite{ new Surface("assets/nc2tiles.png"), 1 }; // can be either
	//Sprite Playersprite{ new Surface("tmpl8/assets/sprPlayer.png"), 1 }; // can be either

	int ScreenHeight = 0, ScreenWidth = 0;

	Player myPlayer(64, 64);
	Level level(10, 10);

	void Game::Init()
	{
		ScreenWidth = screen->GetWidth();
		ScreenHeight = screen->GetHeight();
		level.CreateLevel();
	}
	
	void Game::Shutdown()
	{
	}

	void Game::Tick(float deltaTime)
	{
		screen->Clear(0);

		screen->Line(0,ScreenHeight - 200,ScreenWidth, ScreenHeight - 200,0x00FF00);
		screen->Line(600,0 ,600, ScreenHeight,0x00FF00);

		level.Draw(screen);

		myPlayer.Move(deltaTime,level);
		myPlayer.Draw(screen);
	}
};