#pragma once
#include <string>
#include <tuple>
#include <vector>

namespace Tmpl8
{
class Surface;

enum class TileType
{
    Empty = 0,
    Grass = 1,
    Ground = 2,
    Spike = 3,
    Flag = 4,
    Portal = 5,
    Water = 6,
    Cement = 7,
    Moon = 8,
    Sun = 9
};

enum timeOfDay
{
    Day,
    Night
};

class Level
{
  public:
    Level(int iwidth, int iheight);

    void CreateLevel(std::string levelname);
    void Draw(Surface* gameScreen);
    void FindFlag(int& outY, int& outX);
    int Collision(int y, int x);
    bool SpikeColision(int py, int px, int sy, int sx);

    timeOfDay currentDay = timeOfDay::Day;
  private:
    // double clamp(double d, double min, double max);
    void DrawRotatedSprite(Surface* gameScreen, int y, int x, int ScreenHeight, int ScreenWidth, int backgroundTileSet);
    float sign(int p1y, int p1x, int p2y, int p2x, int p3y, int p3x);
    int width, height, tileSize = 32;
    std::vector<std::vector<int>> tiles;
};
} // namespace Tmpl8