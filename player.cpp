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

    enum PlayerState {Alive, Dead};

    PlayerState playerStatus = Alive;

	Player::Player(float iy, float ix, float ispeed, int idirection, float igravity)
	{
		y = iy;
		x = ix;
        spawnX = x;
        spawnY = y;
		speed = ispeed;
		direction = idirection;
		gravity = igravity;
	}

    void Player::Update(float deltaTime, Level& level)
    {
        playerWidth = Playersprite.GetWidth();
        playerHeight = Playersprite.GetHeight();

        bool jumpPressed = false;
        float moveX = 0;

        ProcessInput(moveX, jumpPressed);

        UpdateX(deltaTime, level, moveX);

        x = std::clamp(x, 0.0f, (float)ScreenWidth - playerWidth);

        UpdateY(deltaTime, level, jumpPressed);

        UpdateTimers(deltaTime, level);

        if (level.Collision(x, y))
        {

        }

        if (playerStatus == Dead)
        {
            playerStatus = Alive;
            x = spawnX;
            y = spawnY;
        }
    }

    void Player::ProcessInput(float& moveX, bool& jumpPressed)
    {
        int jumpmask = 0X8000;

        if (GetAsyncKeyState(VK_LEFT))  moveX = -speed;
        if (GetAsyncKeyState(VK_RIGHT)) moveX = speed;

        jumpPressed = GetAsyncKeyState(VK_UP) & jumpmask;
    }

    void Player::UpdateX(float deltaTime, Level& level, float moveX)
    {
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
    }

    void Player::UpdateY(float deltaTime, Level& level, bool& jumpPressed)
    {
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

        int leftTile = (int)x / tileSize;
        int rightTile = (int)(x + playerWidth - 1) / tileSize;
        int topTile = (int)nextY / tileSize;
        int bottomTile = (int)(nextY + playerHeight - 1) / tileSize;

        bool hitY = false;
        for (int r = topTile; r <= bottomTile; r++) {
            for (int c = leftTile; c <= rightTile; c++) {

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
    }

    void Player::UpdateTimers(float deltaTime, Level& level)
    {
        grounded = level.Collision(y + playerHeight * 1.25, x) || level.Collision(y + playerHeight * 1.25, x + playerWidth - 1);

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