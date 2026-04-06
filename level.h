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
		bool SpikeColision(int py, int px, int sy, int sx);
	private:
		//double clamp(double d, double min, double max);
		float sign(int p1y, int p1x, int p2y, int p2x, int p3y, int p3x);
		int width, height;
		std::vector<std::vector<int>> tiles;
	};
}