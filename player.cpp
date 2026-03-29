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

    enum PlayerState {alive, dead};

    PlayerState state = alive;

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

        float deltaX = moveX * (deltaTime / 10.0f);
        float nextX = x + deltaX;

        int leftTile = (int)nextX / 32;
        int rightTile = (int)(nextX + playerWidth - 1) / 32;
        int topTile = (int)y / 32;
        int bottomTile = (int)(y + playerHeight - 1) / 32;

        bool hitX = false;
        for (int r = topTile; r <= bottomTile; r++) {
            for (int c = leftTile; c <= rightTile; c++) {
                if (level.Collision(r * 32, c * 32)) {
                    hitX = true;
                    break;
                }
            }
            if (hitX) break;
        }

        if (hitX) {
            if (deltaX > 0) {
                x = (rightTile * 32) - playerWidth;
            }
            else if (deltaX < 0) {
                x = (leftTile * 32) + 32;
            }
        }
        else {
            x = nextX;
        }

        bool jumpPressed = GetAsyncKeyState(VK_UP) & 0x8000;

        if (jumpPressed) {
            if (!jumplastframe && (grounded || jumpAmount > 0)) {
                currentGravity = -6.0f;
                if (!(grounded || coyotetime >= 0)) jumpAmount--;
                grounded = false;
            }
            jumplastframe = true;
        }
        else {
            jumplastframe = false;
        }

        currentGravity += gravity * (deltaTime / 100.0f);
        if (currentGravity > 15.0f) currentGravity = 15.0f;

        float deltaY = currentGravity * (deltaTime / 10.0f);
        float nextY = y + deltaY;

        leftTile = (int)x / 32;
        rightTile = (int)(x + playerWidth - 1) / 32;
        topTile = (int)nextY / 32;
        bottomTile = (int)(nextY + playerHeight - 1) / 32;

        bool hitY = false;
        for (int r = topTile; r <= bottomTile; r++) {
            for (int c = leftTile; c <= rightTile; c++) {
                //if (tiles[r, c] == 3)
                //{

                //}

                if (level.Collision(r * 32, c * 32)) {
                    hitY = true;
                    break;
                }
            }
            if (hitY) break;
        }

        if (hitY) {
            if (deltaY > 0) {
                y = (bottomTile * 32) - playerHeight;
                currentGravity = 0;
                jumptime = 0.3f;
                jumpAmount = 1;
            }
            else if (deltaY < 0) {
                y = (topTile * 32) + 32;
                currentGravity = 0;
            }
        }
        else {
            y = nextY;
        }

        grounded = level.Collision(y + 50, x) || level.Collision(y + 50, x + 16) || level.Collision(y + 50, x + 31);

        if (grounded)
        {
            coyotetime = 1;
        }coyotetime -= deltaTime / 100;

        //if (level.SpikeColision(y, x, y, x))
        //{
        //    printf("ded");
        //}
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