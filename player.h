#pragma once

namespace Tmpl8
{
	class Surface;
	class Level;

	class Player
	{
	public:
		Player(float ix, float iy, float ispeed = 3.0f, int idirection = 0, float igravity = 2.5f);

		void Move(float deltaTime, Level& level);
		void Draw(Surface* gameScreen);

	private:
		double clamp(double d, double min, double max);

		float x, y, speed, gravity, currentGravity = -1, jumptime = 0.3f, coyotetime;
		int direction, jumpAmount = 0, playerWidth, playerHeight;
		bool grounded = false, jumplastframe = false;
	};
}