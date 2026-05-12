#pragma once
#include <string>
#include <tuple>
#include <vector>

#include "../include/tinyxml2.h"

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
    Sun = 9,
    MoonBlock = 10,
    SunBlock = 11,
    VineStump = 12,
    VineBody = 13
};

enum class TimeOfDay // rename
{
    Day,
    Night
};

class Level
{
  public:
    Level(int iwidth, int iheight);

    void CreateLevel(std::string levelname, std::vector<std::vector<int>>& entities);
    void Draw(Surface* gameScreen, float deltaTime);
    void FindFlag(int& outY, int& outX);
    void FindTileInstances(std::vector<std::vector<int>>& listOfTilesInstances, int tileId);
    void UpdateVines(std::vector<std::vector<int>>& listOfVines);
    int GetTileID(int y, int x);
    bool SpikeCollision(int py, int px, int sy, int sx);

    TimeOfDay currentTime = TimeOfDay::Day;

  private:
    int DecodeTileGID(unsigned long gid);
    void ParseEntities(tinyxml2::XMLElement* objectNode, std::vector<std::vector<int>>& entities);
    void ParseTileCSV(const char* csvText, int width, int height);

    void DrawRotatedSprite(Surface* gameScreen, int y, int x, int ScreenHeight, int ScreenWidth, int backgroundTileSet);
    float Sign(int p1y, int p1x, int p2y, int p2x, int p3y, int p3x);
    int width, height, tileSize = 32;
    std::vector<std::vector<int>> tiles;
    int rotatedIdValue = 100; // if higher then this value the sprite should be rotated
};
} // namespace Tmpl8