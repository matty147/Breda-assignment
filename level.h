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
		bool Collision(int y, int x);
	private:
		double clamp(double d, double min, double max);
		int width, height;
		std::vector<std::vector<int>> tiles;
	};
}