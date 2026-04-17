#include "level.h"
#include "tmpl8/surface.h"
#include <windows.h>
#include <cstdio>
#include <vector>
#include <cmath>
#include <numbers> 
#include <string>

#include "tinyxml2.h"
#include <sstream>
#include <iostream>
#include <algorithm>

std::vector<std::vector<int>> tiles;

using namespace tinyxml2;

enum timeOfDay {Day, Night};

timeOfDay currentDay = timeOfDay::Day;

enum class TileType {
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

using namespace std;

namespace Tmpl8
{

    extern Sprite Tilessprite;

        Level::Level(int iwidth, int iheight)
        {
            width = iwidth;
            height = iheight;

            tiles.resize(height, std::vector<int>(width, 0));
        }

        void Level::CreateLevel(string levelName)
        {
            tinyxml2::XMLDocument doc;

            int peicemask = 0x1FFFFFFF;
            int rotationmask = 0xE0000000;

            std::vector<std::string> B = { "level1", "level2", "level3", "level4" };

            // level1.tmx
            string fullPath = "levels/" + levelName + ".tmx";

            printf("loaded level %s\n", fullPath.c_str());

            if (doc.LoadFile(fullPath.c_str()) != XML_SUCCESS) {
                printf("Failed to load level file!\n");
                return;
            }

            XMLElement* mapNode = doc.FirstChildElement("map");
            width = mapNode->IntAttribute("width");
            height = mapNode->IntAttribute("height");

            tiles.resize(height, std::vector<int>(width, 0));

            XMLElement* layerNode = mapNode->FirstChildElement("layer");
            XMLElement* dataNode = layerNode->FirstChildElement("data");

            if (!dataNode) {
                printf("Could not find tile data in XML!\n");
                return;
            }

            const char* tileDataText = dataNode->GetText();
            std::string dataString(tileDataText);

            std::replace(dataString.begin(), dataString.end(), '\n', ',');
            std::replace(dataString.begin(), dataString.end(), '\r', ',');

            std::stringstream ss(dataString);
            std::string token;

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {

                    while (std::getline(ss, token, ',')) {

                        token.erase(0, token.find_first_not_of(" \t\r\n"));

                        if (!token.empty()) {

                            if (std::stoul(token) - 1 > 100) // rotation - last 3 bits are the rotation data
                            {  
                                unsigned int rotation = (std::stoul(token) - 1 & rotationmask) >> 29;

                                printf("%d\n", rotation);

                                tiles[y][x] = rotation * 100 + std::stoul(token) - 1 & peicemask;
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

        void Level::FindFlag(int& outY, int& outX)
        {
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {

                    if (tiles[y][x] == (int)TileType::Flag) {
                        outX = x * tileSize;
                        outY = y * tileSize;
                        break;
                    }
                }
            }
        }

        void Level::Draw(Surface* gameScreen) {

            int ScreenWidth = gameScreen->GetWidth();
            int ScreenHeight = gameScreen->GetHeight();

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    if (x * tileSize >= 0 && x * tileSize < ScreenWidth && y * tileSize >= 0 && y * tileSize < ScreenHeight)
                    {
                        if (tiles[y][x] < 100)
                        {
                            Pixel* src = Tilessprite.GetBuffer() + 1 + tiles[y][x] * (tileSize + 1) + (1 + (int)currentDay * (tileSize + 1)) * 595;
                            Pixel* dst = gameScreen->GetBuffer() + x * tileSize + y * tileSize * ScreenWidth;
                            for (int i = 0; i < tileSize; i++)
                            {
                                for (int j = 0; j < tileSize; j++)
                                    dst[j] = src[j];
                                src += 595, dst += ScreenWidth;
                            }
                            continue;
                        }

                        int tile = tiles[y][x] % 100;

                        float angle = 0;

                        int tilesid = (tiles[y][x] / 100);

                        if (tiles[y][x] > 100)
                        {
                            switch (tilesid)
                            {
                            case 3:
                                angle = 90.0f * (std::numbers::pi / 180.f);
                                break;
                            case 5:
                                angle = 270.0f * (std::numbers::pi / 180.f);
                                break;
                            default:
                                angle = 180.0f * (std::numbers::pi / 180.f);
                                break;
                            }
                        }

                        int drawWidth = tileSize;
                        int drawHeight = tileSize;

                        if (x * tileSize + tileSize > ScreenWidth) {
                            drawWidth = ScreenWidth - x * tileSize;
                        }

                        if (y * tileSize + tileSize > ScreenHeight) {
                            drawHeight = ScreenHeight - y * tileSize;
                        }

                        if (tile < 0) continue;

                        float cos = std::cos(angle);
                        float sin = std::sin(angle);

                        Pixel* dst = gameScreen->GetBuffer() + x * tileSize + y * tileSize * ScreenWidth;
                        for (int i = 0; i < drawHeight; i++)
                        {
                            for (int j = 0; j < drawWidth; j++)
                            {
                                float off = 15.5f;

                                int dx = j - off, dy = i - off;

                                int newX = (int)((dx * cos) - (dy * sin) + 16);
                                int newY = (int)((dx * sin) + (dy * cos) + 16);

                                Pixel* src = Tilessprite.GetBuffer() + tile * (tileSize + 1) + newX + ((int)currentDay * (tileSize + 1) + newY) * 595;
                                dst[j] = *src;
                            }
                            dst += ScreenWidth;
                        }

                    }
                }
            }
        }


        int Level::Collision(int y, int x)
        {
            int tx = std::clamp(x / tileSize, 0, width - 1), ty = std::clamp(y / tileSize, 0, height - 1);

            int currentTile = tiles[ty][tx] % 100;

            switch (currentTile)
            {
                // move?
                case (int)TileType::Sun:
                    currentDay = timeOfDay::Day;
                    return -1;

                case (int)TileType::Moon:
                    currentDay = timeOfDay::Night;
                    return -1;

                case (int)TileType::Water:
                    if (currentDay == timeOfDay::Day)
                    {
                        return -1;
                    }
                    break;

                case (int)TileType::Flag:
                //case (int)TileType::Portal:
                    return -1;  
            }

            return (tiles[ty][tx] % 100);
        }

        // TODO: needs rotation
        bool Level::SpikeColision(int playerY, int playerX, int gridRow, int gridCol)
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

            printf("%d\n",(has_neg && has_pos));

            return !(has_neg && has_pos);
        }

        float Level::sign(int p1y, int p1x, int p2y, int p2x, int p3y, int p3x)
        {
            return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
        }

        int width, height;
}