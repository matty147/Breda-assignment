#include "game.h"
#include "tmpl8/surface.h"
#include <windows.h>
#include <cstdio>
#include <algorithm>

#include "player.h"

namespace Tmpl8
{
	Sprite Playersprite{ new Surface("assets/sprPlayer.png"), 1 };
	int ScreenHeight = 0, ScreenWidth = 0;

	void Game::Init()
	{
		ScreenWidth = screen->GetWidth();
		ScreenHeight = screen->GetHeight();

	}
	
	void Game::Shutdown()
	{
	}

	Player myPlayer(64, 64);
	Player myPlayer2(128, 64);
	Player myPlayer3(200, 64);

	void Game::Tick(float deltaTime)
	{
		screen->Clear(0);

		screen->Line(0,ScreenHeight - 200,ScreenWidth, ScreenHeight - 200,0x00FF00);
		screen->Line(600,0 ,600, ScreenHeight,0x00FF00);

		myPlayer.Move(deltaTime);
		myPlayer.Draw(screen);
		myPlayer2.Move(deltaTime);
		myPlayer2.Draw(screen);
		myPlayer3.Move(deltaTime);
		myPlayer3.Draw(screen);

	}
};