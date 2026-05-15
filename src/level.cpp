#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numbers>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

#include "../third_party/tinyxml2.h"
#include "../tmpl8/surface.h"

#include "../include/level.h"

using namespace tinyxml2;

using namespace std;

float changeTileSetCoordinate = 0;

namespace Tmpl8
{
extern Sprite tilesSprite;

/// <summary>
/// Constructs an empty level with the given tile dimensions and allocates the tile grid.
/// </summary>
/// <param name="iwidth">Width in tiles.</param>
/// <param name="iheight">Height in tiles.</param>
Level::Level(int iwidth, int iheight)
{
    width = iwidth;
    height = iheight;

    tiles.resize(height, std::vector<int>(width, 0));
}

/// <summary>
/// Decodes a raw Tiled GID into a tile index, packing rotation into the hundreds digit
/// </summary>
int Level::DecodeTileGID(unsigned long gid)
{
    int pieceMask = 0x1FFFFFFF;
    int rotationMask = 0xE0000000;

    unsigned long id = gid - 1;
    if (id > rotatedIdValue)
    {
        unsigned int rotation = (id & rotationMask) >> 29;
        return (int)((rotation * rotatedIdValue + id) & pieceMask);
    }
    return (int)id;
}

/// <summary>
/// Reads entity spawn points from the objectgroup layer
/// </summary>
void Level::ParseEntities(XMLElement* objectNode, vector<vector<int>>& entities)
{
    entities.clear();

    while (objectNode != nullptr)
    {
        int spawnX = 0;
        int spawnY = 0;

        objectNode->QueryIntAttribute("x", &spawnX);
        objectNode->QueryIntAttribute("y", &spawnY);

        printf("Spawn Enemy at X: %d, Y: %d\n", spawnX, spawnY);

        entities.push_back({spawnY, spawnX});

        objectNode = objectNode->NextSiblingElement("object");
    }
}

/// <summary>
/// Parses the CSV tile data from a TMX data node into the tiles array
/// </summary>
void Level::ParseTileCSV(const char* csvText, int width, int height)
{
    const char* start = csvText;
    char* end;

    int nextLine = 0;

    int x = 0, y = 0;

    while (*start != '\0')
    {
        unsigned long gid = std::strtoul(start, &end, 10);

        if (start == end)
            break;

        start = end;

        if (*start != '\0')
        {
            start++;
        }

        tiles[y][x] = DecodeTileGID(gid);

        start = end;
        while (*start == ',' || *start == '\r')
            start++;

        if (++x >= width)
        {
            x = 0;
            ++y;

            if (y >= height)
                break;
        }
    }
}

/// <summary>
/// Reads and loads data from a .tmx level file
/// </summary>
/// <param name="levelName"></param>
void Level::CreateLevel(string levelName, std::vector<std::vector<int>>& entities)
{
    tinyxml2::XMLDocument doc;
    string fullPath = "assets/levels/" + levelName + ".tmx";

    printf("Trying to load level %s\n", fullPath.c_str());

    if (doc.LoadFile(fullPath.c_str()) != XML_SUCCESS)
    {
        printf("Failed to load level file!\n %s\n", fullPath.c_str());
        return;
    }

    XMLElement* mapNode = doc.FirstChildElement("map");
    width = mapNode->IntAttribute("width");
    height = mapNode->IntAttribute("height");
    tiles.assign(height, std::vector<int>(width, 0));

    XMLElement* entityNode = mapNode->FirstChildElement("objectgroup");

    if (!entityNode)
    {
        printf("Could find any entities!\n");
    }

    ParseEntities(entityNode->FirstChildElement("object"), entities);

    XMLElement* dataNode = mapNode->FirstChildElement("layer")->FirstChildElement("data");
    if (!dataNode)
    {
        printf("Could not find tile data in XML!\n");
        return;
    }

    ParseTileCSV(dataNode->GetText(), width, height);
}

/// <summary>
/// Finds the Flag - player starting point
/// </summary>
/// <param name="outY"></param>
/// <param name="outX"></param>
void Level::FindFlag(int& outY, int& outX)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (tiles[y][x] == (int)TileType::Flag)
            {
                outX = x * tileSize;
                outY = y * tileSize;
                break;
            }
        }
    }
}

/// <summary>
/// Locate all instances of a block in the curent room
/// </summary>
/// <param name="listOfTilesInstances"></param>
/// <param name="tileId"></param>
void Level::FindTileInstances(std::vector<std::vector<int>>& listOfTilesInstances, int tileId)
{
    listOfTilesInstances.clear();
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (tiles[y][x] == tileId)
            {
                listOfTilesInstances.push_back({y, x});
            }
        }
    }
}

/// <summary>
/// Updates the state of the vine stump
/// </summary>
/// <param name="listOfVines"></param>
void Level::UpdateVines(std::vector<std::vector<int>>& listOfVines)
{
    for (int i = 0; i < listOfVines.size(); i++)
    {
        int vineY = listOfVines[i][0];
        int vineX = listOfVines[i][1];

        int vineHeight = 1;

        if (currentTime == TimeOfDay::Day)
        {
            tiles[vineY][vineX] = (int)TileType::VineBody;

            while (vineY - vineHeight >= 0 && tiles[vineY - vineHeight][vineX] == 0)
            {
                tiles[vineY - vineHeight][vineX] = (int)TileType::VineBody;
                vineHeight++;
            }
        }
        else
        {
            tiles[vineY][vineX] = (int)TileType::VineStump;

            while (vineY - vineHeight >= 0 && tiles[vineY - vineHeight][vineX] == (int)TileType::VineBody)
            {
                tiles[vineY - vineHeight][vineX] = 0;
                vineHeight++;
            }
        }
    }
}

/// <summary>
/// Draws the tiles on the game screen
/// </summary>
/// <param name="gameScreen"></param>
void Level::Draw(Surface* gameScreen, float deltaTime)
{
    int screenShiftSize = 595; // amount to shift to get to the next row on the screen

    int ScreenWidth = gameScreen->GetWidth();
    int ScreenHeight = gameScreen->GetHeight();

    if (currentTime == TimeOfDay::Day)
    {
        changeTileSetCoordinate -= 1.0f * deltaTime / 10;
    }
    else
    {
        changeTileSetCoordinate += 1.0f * deltaTime / 10;
    }

    changeTileSetCoordinate = std::clamp(changeTileSetCoordinate, 0.0f, (float)width);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (x * tileSize >= 0 && x * tileSize < ScreenWidth && y * tileSize >= 0 && y * tileSize < ScreenHeight)
            {
                int backgroundTileSet = 0;

                if (std::floor(changeTileSetCoordinate) > x)
                {
                    backgroundTileSet = 1;
                }

                if (tiles[y][x] > rotatedIdValue)
                {
                    DrawRotatedSprite(gameScreen, y, x, ScreenHeight, ScreenWidth, backgroundTileSet);
                    continue;
                }

                Pixel* src = tilesSprite.GetBuffer() + 1 + tiles[y][x] * (tileSize + 1) + (1 + backgroundTileSet * (tileSize + 1)) * screenShiftSize;
                Pixel* dst = gameScreen->GetBuffer() + x * tileSize + y * tileSize * ScreenWidth;
                for (int i = 0; i < tileSize; i++)
                {
                    for (int j = 0; j < tileSize; j++)
                    {
                        dst[j] = src[j];
                    }
                    src += screenShiftSize, dst += ScreenWidth;
                }
            }
        }
    }
}

/// <summary>
/// Calculates and draws the correctly rotated sprite
/// </summary>
/// <param name="gameScreen"></param>
/// <param name="y"></param>
/// <param name="x"></param>
/// <param name="ScreenHeight"></param>
/// <param name="ScreenWidth"></param>
/// <param name="backgroundType"></param>
void Level::DrawRotatedSprite(Surface* gameScreen, int y, int x, int ScreenHeight, int ScreenWidth, int backgroundTileSet)
{
    int screenShiftSize = 595;
    float angle = 0;

    int tile = tiles[y][x] % rotatedIdValue;

    int tilesid = (tiles[y][x] / rotatedIdValue);

    if (tiles[y][x] > rotatedIdValue)
    {
        switch (tilesid)
        {
            case 3:
                angle = std::numbers::pi_v<float> / 2.0f; // 90 degrees
                break;
            case 5:
                angle = 3.0f * std::numbers::pi_v<float> / 2.0f; // 270 degrees
                break;
            default:
                angle = std::numbers::pi_v<float>; // 180 degrees
                break;
        }
    }

    int drawWidth = tileSize;
    int drawHeight = tileSize;

    if (x * tileSize + tileSize > ScreenWidth)
    {
        drawWidth = ScreenWidth - x * tileSize;
    }

    if (y * tileSize + tileSize > ScreenHeight)
    {
        drawHeight = ScreenHeight - y * tileSize;
    }

    if (tile < 0)
        return;

    float cos = std::cos(angle);
    float sin = std::sin(angle);

    int off = (tileSize) / 2;

    Pixel* dst = gameScreen->GetBuffer() + x * tileSize + y * tileSize * ScreenWidth;
    for (int i = 0; i < drawHeight; i++)
    {
        for (int j = 0; j < drawWidth; j++)
        {
            int dx = j - off, dy = i - off;

            int newX = std::round((dx * cos) - (dy * sin) + tileSize / 2);
            int newY = std::round((dx * sin) + (dy * cos) + tileSize / 2);

            newX = std::clamp(newX, 0, tileSize - 1);
            newY = std::clamp(newY, 0, tileSize - 1);

            Pixel* src = tilesSprite.GetBuffer() + tile * (tileSize + 1) + newX + (backgroundTileSet * (tileSize + 1) + newY) * screenShiftSize;
            dst[j] = *src;
        }
        dst += ScreenWidth;
    }
}

/// <summary>
/// Returns the id the entity is currently on
/// </summary>
/// <param name="y"></param>
/// <param name="x"></param>
/// <returns></returns>
int Level::GetTileID(int y, int x)
{
    int tx = std::clamp(x / tileSize, 0, width - 1), ty = std::clamp(y / tileSize, 0, height - 1);

    int currentTile = tiles[ty][tx] % rotatedIdValue;

    switch (currentTile)
    {
        case (int)TileType::Water:
            if (currentTime == TimeOfDay::Day)
            {
                return -currentTile;
            }
            break;

        case (int)TileType::MoonBlock:
            if (currentTime == TimeOfDay::Day)
            {
                return -currentTile;
            }
            break;

        case (int)TileType::SunBlock:
            if (currentTime == TimeOfDay::Night)
            {
                return -currentTile;
            }
            break;

        case (int)TileType::VineStump:
        case (int)TileType::Flag:
            return -currentTile;
    }

    return (tiles[ty][tx] % rotatedIdValue);
}

/// <summary>
/// Detects if a point is in a triangle
/// </summary>
/// <param name="playerY"></param>
/// <param name="playerX"></param>
/// <param name="gridRow"></param>
/// <param name="gridCol"></param>
/// <returns></returns>
bool Level::SpikeCollision(int playerY, int playerX, int gridRow, int gridCol) // TODO: needs rotation
{
    int ty = gridRow * tileSize, tx = gridCol * tileSize;

    int bottomLeftY = ty + tileSize, bottomLeftX = tx;
    int bottomRightY = ty + tileSize, bottomRightX = tx + tileSize;
    int topCenterY = ty, topCenterX = tx + tileSize / 2;

    int side1 = Sign(playerY, playerX, bottomLeftY, bottomLeftX, bottomRightY, bottomRightX);
    int side2 = Sign(playerY, playerX, bottomRightY, bottomRightX, topCenterY, topCenterX);
    int side3 = Sign(playerY, playerX, topCenterY, topCenterX, bottomLeftY, bottomLeftX);

    bool has_neg = (side1 < 0) || (side2 < 0) || (side3 < 0);
    bool has_pos = (side1 > 0) || (side2 > 0) || (side3 > 0);

    return !(has_neg && has_pos);
}

/// <summary>
/// checks on what side ofthe line the point is
/// </summary>
/// <param name="p1y"></param>
/// <param name="p1x"></param>
/// <param name="p2y"></param>
/// <param name="p2x"></param>
/// <param name="p3y"></param>
/// <param name="p3x"></param>
/// <returns></returns>
int Tmpl8::Level::Sign(int p1y, int p1x, int p2y, int p2x, int p3y, int p3x)
{
    return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
}

} // namespace Tmpl8
