#include "player.h"
#include "tmpl8/surface.h"
#include <windows.h>
#include <cstdio>
#include <cmath>
#include <algorithm>

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

        int tileSize = 32;

        float moveX = 0;
        if (GetAsyncKeyState(VK_LEFT))  moveX = -speed;
        if (GetAsyncKeyState(VK_RIGHT)) moveX = speed;

        float deltaX = moveX * (deltaTime / 10.0f);
        float nextX = x + deltaX;

        int leftTile = (int)nextX / tileSize;
        int rightTile = (int)(nextX + playerWidth - 1) / tileSize;
        int topTile = (int)y / tileSize;
        int bottomTile = (int)(y + playerHeight - 1) / tileSize;

        bool hitX = false;
        for (int r = topTile; r <= bottomTile; r++) {
            for (int c = leftTile; c <= rightTile; c++) {
                if (level.Collision(r * tileSize, c * tileSize)) {
                    hitX = true;
                    break;
                }
            }
            if (hitX) break;
        }

        if (hitX) {
            if (deltaX > 0) {
                x = (rightTile * tileSize) - playerWidth;
            }
            else if (deltaX < 0) {
                x = (leftTile * tileSize) + tileSize;
            }
        }
        else {
            x = nextX;
        }

        x = std::clamp(x, 0.0f, (float)ScreenWidth - playerWidth);

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

        leftTile = (int)x / tileSize;
        rightTile = (int)(x + playerWidth - 1) / tileSize;
        topTile = (int)nextY / tileSize;
        bottomTile = (int)(nextY + playerHeight - 1) / tileSize;

        bool hitY = false;
        for (int r = topTile; r <= bottomTile; r++) {
            for (int c = leftTile; c <= rightTile; c++) {
                //if (tiles[r, c] == 3)
                //{

                //}

                if (level.Collision(r * tileSize, c * tileSize)) {
                    hitY = true;
                    break;
                }
            }
            if (hitY) break;
        }

        if (hitY) {
            if (deltaY > 0) {
                y = (bottomTile * tileSize) - playerHeight;
                currentGravity = 0;
                jumptime = 0.3f;
                jumpAmount = 1;
            }
            else if (deltaY < 0) {
                y = (topTile * tileSize) + tileSize;
                currentGravity = 0;
            }
        }
        else {
            y = nextY;
        }
        
        grounded = level.Collision(y + playerHeight * 1.25, x) || level.Collision(y + playerHeight * 1.25, x + playerWidth -1);

        if (grounded)
        {
            coyotetime = 1;
        }
        coyotetime -= deltaTime / 100;
    }

	void Player::Draw(Surface* gameScreen)
	{
		Playersprite.Draw(gameScreen, x, y);
	}
}