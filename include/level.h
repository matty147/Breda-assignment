#pragma once
#include <string>
#include <tuple>
#include <vector>

#include "../third_party/tinyxml2.h"

namespace Tmpl8
{

static constexpr int tileSize = 32;

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

enum class TimeOfDay
{
    Day,
    Night
};

class Level
{
  public:
    /// <summary>Constructs an empty level with the given tile dimensions.</summary>
    /// <param name="iwidth">Width in tiles.</param>
    /// <param name="iheight">Height in tiles.</param>
    Level(int iwidth, int iheight);

    /// <summary>Loads a .tmx file into the tile grid and fills the entity spawn list.</summary>
    /// <param name="levelname">Level name (file is assets/levels/levelname.tmx).</param>
    /// <param name="entities">Out: list of {y, x} entity spawn coordinates.</param>
    void CreateLevel(std::string levelname, std::vector<std::vector<int>>& entities);

    /// <summary>Renders all visible tiles on the screen.</summary>
    /// <param name="gameScreen">Target screen surface.</param>
    /// <param name="deltaTime">Frame delta time in milliseconds.</param>
    void Draw(Surface* gameScreen, float deltaTime);

    /// <summary>Finds the player-spawn flag tile and returns its pixel position.</summary>
    /// <param name="outY">Out: Y position in pixels.</param>
    /// <param name="outX">Out: X position in pixels.</param>
    void FindFlag(int& outY, int& outX);

    /// <summary>Collects {y, x} positions of every tile matching the given id.</summary>
    /// <param name="listOfTilesInstances">Out: list of tile coordinates.</param>
    /// <param name="tileId">Tile id to search for.</param>
    void FindTileInstances(std::vector<std::vector<int>>& listOfTilesInstances, int tileId);

    /// <summary>Grows vines upward during day and removes them at night.</summary>
    /// <param name="listOfVines">List of vine stump {y, x} positions.</param>
    void UpdateVines(std::vector<std::vector<int>>& listOfVines);

    /// <summary>Returns the tile id at the given pixel coordinates (negative for non-solid interactive tiles).</summary>
    /// <param name="y">Y coordinate in pixels.</param>
    /// <param name="x">X coordinate in pixels.</param>
    /// <returns>Tile id; negative values denote non-solid interactive tiles.</returns>
    int GetTileID(int y, int x);

    /// <summary>Tests whether a point lies inside the triangular spike at the given tile.</summary>
    /// <param name="py">Point Y in pixels.</param>
    /// <param name="px">Point X in pixels.</param>
    /// <param name="sy">Spike tile row.</param>
    /// <param name="sx">Spike tile column.</param>
    /// <returns>True if the point is inside the spike triangle.</returns>
    bool SpikeCollision(int py, int px, int sy, int sx);

    TimeOfDay currentTime = TimeOfDay::Day;

  private:
    /// <summary>Decodes a raw Tiled GID into a tile index, packing rotation into the hundreds digit.</summary>
    /// <param name="gid">Raw GID from the .tmx file.</param>
    /// <returns>Decoded tile id with rotation encoded.</returns>
    int DecodeTileGID(unsigned long gid);

    /// <summary>Reads entity spawn coordinates from the objectgroup layer.</summary>
    /// <param name="objectNode">First &lt;object&gt; XML node in the object group.</param>
    /// <param name="entities">Out: list of {y, x} entity spawn coordinates.</param>
    void ParseEntities(tinyxml2::XMLElement* objectNode, std::vector<std::vector<int>>& entities);

    /// <summary>Parses the CSV tile data from a TMX data node into the tiles array.</summary>
    /// <param name="csvText">Raw CSV text from the data node.</param>
    /// <param name="width">Level width in tiles.</param>
    /// <param name="height">Level height in tiles.</param>
    void ParseTileCSV(const char* csvText, int width, int height);

    /// <summary>Draws a tile rotated by 90/180/270 degrees based on its encoded rotation.</summary>
    /// <param name="gameScreen">Target screen surface.</param>
    /// <param name="y">Tile row.</param>
    /// <param name="x">Tile column.</param>
    /// <param name="ScreenHeight">Screen height in pixels.</param>
    /// <param name="ScreenWidth">Screen width in pixels.</param>
    /// <param name="backgroundTileSet">Which background variant of the tileset to use.</param>
    void DrawRotatedSprite(Surface* gameScreen, int y, int x, int ScreenHeight, int ScreenWidth, int backgroundTileSet);

    /// <summary>Cross-product helper for the point-in-triangle test.</summary>
    /// <param name="p1y">Point 1 Y.</param>
    /// <param name="p1x">Point 1 X.</param>
    /// <param name="p2y">Point 2 Y.</param>
    /// <param name="p2x">Point 2 X.</param>
    /// <param name="p3y">Point 3 Y.</param>
    /// <param name="p3x">Point 3 X.</param>
    /// <returns>Signed value indicating which side of the line p2-p3 the point p1 lies on.</returns>
    int Sign(int p1y, int p1x, int p2y, int p2x, int p3y, int p3x);

    int width, height;
    std::vector<std::vector<int>> tiles;
    static constexpr int rotatedIdValue = 100; // if higher then this value the sprite should be rotated
};
} // namespace Tmpl8