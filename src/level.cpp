#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <numbers>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

#include "../include/level.h"
#include "../include/tinyxml2.h"
#include "../tmpl8/surface.h"

std::vector<std::vector<int>> tiles;

using namespace tinyxml2;

enum timeOfDay
{
    Day,
    Night
};

//// should rename to current time of day?
//timeOfDay currentDay = timeOfDay::Day;

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
    VineStump = 11,
    VineBody = 12
};

using namespace std;

float changeTileSetCoordinate = 0;

namespace Tmpl8
{
extern Sprite tilesSprite;

Level::Level(int iwidth, int iheight)
{
    width = iwidth;
    height = iheight;

    tiles.resize(height, std::vector<int>(width, 0));
}

/// <summary>
/// Reads and loads data from a .tmx level file
/// </summary>
/// <param name="levelName"></param>
void Level::CreateLevel(string levelName)
{
    tinyxml2::XMLDocument doc;

    int pieceMask = 0x1FFFFFFF;
    int rotationMask = 0xE0000000;

    string fullPath = "levels/" + levelName + ".tmx";

    printf("Trying to load level %s\n", fullPath.c_str());

    if (doc.LoadFile(fullPath.c_str()) != XML_SUCCESS)
    {
        printf("Failed to load level file!\n %s\n", fullPath.c_str());
        return;
    }
    else
        printf("loaded level");

    XMLElement* mapNode = doc.FirstChildElement("map");
    width = mapNode->IntAttribute("width");
    height = mapNode->IntAttribute("height");

    tiles.resize(height, std::vector<int>(width, 0));

    XMLElement* layerNode = mapNode->FirstChildElement("layer");
    XMLElement* dataNode = layerNode->FirstChildElement("data");

    if (!dataNode)
    {
        printf("Could not find tile data in XML!\n");
        return;
    }

    const char* tileDataText = dataNode->GetText();
    std::string dataString(tileDataText);

    std::replace(dataString.begin(), dataString.end(), '\n', ',');
    std::replace(dataString.begin(), dataString.end(), '\r', ',');

    std::stringstream ss(dataString);
    std::string token;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            while (std::getline(ss, token, ','))
            {
                token.erase(0, token.find_first_not_of(" \t\r\n"));

                if (!token.empty())
                {
                    if (std::stoul(token) - 1 > 100) // rotation - last 3 bits are the rotation data
                    {
                        unsigned int rotation = (std::stoul(token) - 1 & rotationMask) >> 29;

                        tiles[y][x] = rotation * 100 + std::stoul(token) - 1 & pieceMask;
                        break;
                    }

                    tiles[y][x] = std::stoi(token) - 1;
                    break;
                }
            }
        }
    }

    printf("Level loaded successfully from TMX!\n");
}

/// <summary>
/// Finds the Flag - player starting point
/// </summary>
/// <param name="outY"></param>
/// <param name="outX"></param>
void Level::FindFlag(int& outY, int& outX) //this is probably not really needed and can use the fun under it
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
                listOfTilesInstances.push_back({y,x});
            }
        }
    }
}

void Level::UpdateVines(std::vector<std::vector<int>>& listOfVines)
{

    // destroy them all
    std::vector<std::vector<int>> vineDestroy;

    FindTileInstances(vineDestroy, (int)TileType::VineBody);

    for (std::vector<int> vine : vineDestroy)
    {
        tiles[vine[0]][vine[1]] = 0;
    }

    for (int i = 0; i < listOfVines.size(); i++)
    {
        int vineY = listOfVines[i][0];
        int vineX = listOfVines[i][1];

        if (currentDay == timeOfDay::Night)
        {
            tiles[vineY][vineX] = (int)TileType::VineStump;
            continue;
        }

        int height = 1;

        tiles[vineY][vineX] = (int)TileType::VineBody;

        while (tiles[vineY - height][vineX] == 0)
        {
            tiles[vineY - height][vineX] = (int)TileType::VineBody;
            height++;

            if (vineY - height < 0)
            {
                break;
            }
        }
            
    }
}

/// <summary>
/// Draws the tiles on the game screen
/// </summary>
/// <param name="gameScreen"></param>
void Level::Draw(Surface* gameScreen)
{
    int ScreenWidth = gameScreen->GetWidth();
    int ScreenHeight = gameScreen->GetHeight();

    if (currentDay == timeOfDay::Day)
    {
        changeTileSetCoordinate -= 1.0f;
    }
    else changeTileSetCoordinate += 1.0f;

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

                if (tiles[y][x] > 100)
                {
                    DrawRotatedSprite(gameScreen, y, x, ScreenHeight, ScreenWidth, backgroundTileSet);
                    continue;
                }

                Pixel* src = tilesSprite.GetBuffer() + 1 + tiles[y][x] * (tileSize + 1) + (1 + backgroundTileSet * (tileSize + 1)) * 595;
                Pixel* dst = gameScreen->GetBuffer() + x * tileSize + y * tileSize * ScreenWidth;
                for (int i = 0; i < tileSize; i++)
                {
                    for (int j = 0; j < tileSize; j++)
                        dst[j] = src[j];
                    src += 595, dst += ScreenWidth;
                }
            }
        }
    }
}

/// <summary>
/// Calculates and draws the correctly rotated sprite
/// </summary>
/// <param name="gameScreen"></param>
/// <param name="y"> - Tile Y position</param>
/// <param name="x"> - Tile X position</param>
/// <param name="ScreenHeight"></param>
/// <param name="ScreenWidth"></param>
/// <param name="backgroundType"></param>
void Level::DrawRotatedSprite(Surface* gameScreen, int y, int x, int ScreenHeight, int ScreenWidth, int backgroundTileSet)
{
    int tile = tiles[y][x] % 100;

    float angle = 0;

    int tilesid = (tiles[y][x] / 100);

    if (tiles[y][x] > 100)
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

    Pixel* dst = gameScreen->GetBuffer() + x * tileSize + y * tileSize * ScreenWidth;
    for (int i = 0; i < drawHeight; i++)
    {
        for (int j = 0; j < drawWidth; j++)
        {
            float off = 15.5f;

            int dx = j - off, dy = i - off;

            int newX = std::round((dx * cos) - (dy * sin) + 16.0f);
            int newY = std::round((dx * sin) + (dy * cos) + 16.0f);

            newX = std::clamp(newX, 0, tileSize - 1);
            newY = std::clamp(newY, 0, tileSize - 1);

            Pixel* src = tilesSprite.GetBuffer() + tile * (tileSize + 1) + newX + (backgroundTileSet * (tileSize + 1) + newY) * 595;
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
int Level::Collision(int y, int x)
{
    int tx = std::clamp(x / tileSize, 0, width - 1), ty = std::clamp(y / tileSize, 0, height - 1);

    int currentTile = tiles[ty][tx] % 100;

    switch (currentTile)
    {
        //// move?
        //case (int)TileType::Sun:
        //    currentDay = timeOfDay::Day;
        //    return -1;

        //case (int)TileType::Moon:
        //    currentDay = timeOfDay::Night;
        //    return -1;

        case (int)TileType::Water:
            if (currentDay == timeOfDay::Day)
            {
                return -1;
            }
            break;

        case (int)TileType::Flag:
            // case (int)TileType::Portal:
            return -1;
    }

    return (tiles[ty][tx] % 100);
}

/// <summary>
/// Detects if a point is in a triangle
/// </summary>
/// <param name="playerY"></param>
/// <param name="playerX"></param>
/// <param name="gridRow"></param>
/// <param name="gridCol"></param>
/// <returns></returns>
bool Level::SpikeColision(int playerY, int playerX, int gridRow, int gridCol) // TODO: needs rotation
{
    float side1, side2, side3;
    bool has_neg, has_pos;
    int bottomLeftY, bottomLeftX, bottomRightY, bottomRightX, topCenterY, topCenterX;
    int ty = gridRow * tileSize, tx = gridCol * tileSize;

    bottomLeftY = ty + tileSize, bottomLeftX = tx;
    bottomRightY = ty + tileSize, bottomRightX = tx + tileSize;
    topCenterY = ty, topCenterX = tx + tileSize / 2;

    side1 = sign(playerY, playerX, bottomLeftY, bottomLeftX, bottomRightY, bottomRightX);
    side2 = sign(playerY, playerX, bottomRightY, bottomRightX, topCenterY, topCenterX);
    side3 = sign(playerY, playerX, topCenterY, topCenterX, bottomLeftY, bottomLeftX);

    has_neg = (side1 < 0) || (side2 < 0) || (side3 < 0);
    has_pos = (side1 > 0) || (side2 > 0) || (side3 > 0);

    printf("%d\n", (has_neg && has_pos));

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
float Level::sign(int p1y, int p1x, int p2y, int p2x, int p3y, int p3x)
{
    return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
}

int width, height;
} // namespace Tmpl8
