#pragma once
#include <vector>

namespace Tmpl8
{
	class Surface;

	class Level
	{
	public:
		Level(int iwidth, int iheight);

		void CreateLevel();
		void Draw(Surface* gameScreen);
		bool Collision(int x, int y);

	private:
		int width, height;
		std::vector<std::vector<int>> tiles;
	};
}