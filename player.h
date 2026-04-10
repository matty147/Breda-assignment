#pragma once

namespace Tmpl8
{
	class Surface;
	class Level;

	class Player
	{
	public:
		Player(float iy, float ix, float ispeed = 3.0f, int idirection = 0, float igravity = 2.5f);

		void Update(float deltaTime, Level& level);
		void Draw(Surface* gameScreen);

	private:
		void UpdateTimers(float deltaTime, Level& level);
		void UpdateY(float deltaTime, Level& level, bool& jumpjumpPressed);
		void UpdateX(float deltaTime, Level& level, float moveX);
		void ProcessInput(float& moveX, bool& jumpPressed);
		bool TileCollision(int topTile, int bottomTile, int leftTile, int rightTile, Level& level);


		float x, y, speed, gravity, currentGravity = -1, jumptime = 0.3f, coyotetime;
		int direction, jumpAmount = 0, playerWidth, playerHeight, tileSize = 32,spawnY = 0, spawnX = 0;
		bool grounded = false, jumplastframe = false;
	};
}