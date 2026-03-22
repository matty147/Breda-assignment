#include "game.h"
#include "tmpl8/surface.h"
#include <windows.h>
#include <cstdio>
#include <algorithm>

#include "tmpl8/surface.h"

namespace Tmpl8
{
	Sprite Playersprite{ new Surface("assets/sprPlayer.png"), 1 };
	Surface tiles{ "assets/nc2tiles.png" };
	int ScreenHeight = 0, ScreenWidth = 0;

	class Player
	{

	public:
		Player(float ix, float iy, float ispeed = 3.0f, int idirection = 0, float igravity = 2.0f)
		{
			x = ix;
			y = iy;
			speed = ispeed;
			direction = idirection;
			gravity = igravity;
		}

		void Move(float deltaTime)
		{
			playerWidth = Playersprite.GetWidth();
			playerHeight = Playersprite.GetHeight();

			float moveX = 0;
			if (GetAsyncKeyState(VK_LEFT))  moveX = -speed;
			if (GetAsyncKeyState(VK_RIGHT)) moveX = speed;

			x += moveX * (deltaTime / 10.0f);

			bool jumpPressed = GetAsyncKeyState(VK_UP) & 0x8000;

			if (jumpPressed)
			{
				if (!jumplastframe && (grounded || jumpAmount > 0))
				{
					currentGravity = -6.0f;
					jumpAmount--;
					grounded = false;
				}
				jumplastframe = true;
			}
			else
			{
				jumplastframe = false;
			}

			currentGravity += gravity * (deltaTime / 100.0f);

			if (currentGravity > 15.0f) currentGravity = 15.0f;

			float nextY = y + (currentGravity * (deltaTime / 10.0f));

			if (nextY + playerHeight >= ScreenHeight - 200) // replace with normal collision checks for objects
			{
				y = (float)(ScreenHeight - 200 - playerHeight);
				currentGravity = 0;
				grounded = true;
				jumptime = 0.3f;
				jumpAmount = 2;
			}
			else
			{
				y = nextY;
				grounded = false;
			}

			x = clamp(x, 0.0, (double)(600 - playerWidth));

			printf("jt%.4f\nja %d\n", jumptime, jumpAmount);
		}

		void Draw(Surface* gameScreen)
		{
			Playersprite.Draw(gameScreen, x, y);
		}

		double clamp(double d, double min, double max) {
			const double t = d < min ? min : d;
			return t > max ? max : t;
		}


		float x, y, speed, gravity, currentGravity = -1, jumptime = 0.3f;
		int direction, jumpAmount = 2, playerWidth, playerHeight;
		bool grounded = false, jumplastframe = false;
	};

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