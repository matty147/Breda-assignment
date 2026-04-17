#include "enemy.h"
#include "tmpl8/surface.h"
#include <windows.h>
#include <cstdio>
#include <cmath>
#include <algorithm>

#include "level.h"
#include "game.h"

namespace Tmpl8
{
	extern Sprite Enemysprite;
    extern int ScreenHeight, ScreenWidth;

	Enemy::Enemy(float iy, float ix, float ispeed, int idirection, float igravity)
	{
		y = iy;
		x = ix;
		speed = ispeed;
		direction = idirection;
		gravity = igravity;
	}

	void Enemy::Update(float deltaTime, Level& level)
	{
	}

	void Enemy::Draw(Surface* gameScreen)
	{
		Enemysprite.Draw(gameScreen, x, y);
	}
}