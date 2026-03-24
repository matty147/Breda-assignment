#include "player.h"
#include "tmpl8/surface.h"
#include <windows.h>
#include <cstdio>
#include <cmath>

#include "level.h"

namespace Tmpl8
{
	extern Sprite Playersprite;
	extern int ScreenHeight, ScreenWidth;

	Player::Player(float ix, float iy, float ispeed, int idirection, float igravity)
	{
		x = ix;
		y = iy;
		speed = ispeed;
		direction = idirection;
		gravity = igravity;
	}

	void Player::Move(float deltaTime, Level& level)
	{
		playerWidth = Playersprite.GetWidth();
		playerHeight = Playersprite.GetHeight();

		float moveX = 0;
		if (GetAsyncKeyState(VK_LEFT))  moveX = -speed;
		if (GetAsyncKeyState(VK_RIGHT)) moveX = speed;

		float bottomX = x + moveX * (deltaTime / 10.0f); // rename
		if (level.Collision(y, bottomX + 32) || level.Collision(y, bottomX))
		{
			float tileEdge = level.GetTileEdge(y, bottomX + 32, true);

			if (moveX > 0) {
				x = tileEdge - playerWidth;
			}
			else if (moveX < 0) {
				x = tileEdge;
			}
		}
		else {
			x = bottomX;
		}

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

		float bottom = nextY + playerHeight; // rename

		// vertical
		if (level.Collision(bottom,x) || level.Collision(bottom - 32, x))
		{
			y = level.GetTileEdge(bottom, x, false) - playerHeight;
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
	}

	void Player::Draw(Surface* gameScreen)
	{
		Playersprite.Draw(gameScreen, x, y);
	}

	double Player::clamp(double d, double min, double max)
	{
		const double t = d < min ? min : d;
		return t > max ? max : t;
	}
}