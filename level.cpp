#include "level.h"
#include "tmpl8/surface.h"
#include <windows.h>
#include <cstdio>
#include <vector>
#include <cmath>

#include "tinyxml2.h"
#include <sstream>
#include <iostream>
#include <algorithm>

std::vector<std::vector<int>> tiles;

using namespace tinyxml2;

namespace Tmpl8
{

    extern Sprite Tilessprite;

        Level::Level(int iwidth, int iheight)
        {
            width = iwidth;
            height = iheight;

            tiles.resize(height, std::vector<int>(width, 0));
        }

        void Level::CreateLevel()
        {
            tinyxml2::XMLDocument doc;

            int peicemask = 0x1FFFFFFF;
            int rotationmask = 0xE0000000;

            if (doc.LoadFile("levels/map.tmx") != XML_SUCCESS) { // is in the wrong place?
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

        void Level::Draw(Surface* gameScreen) {

            int ScreenWidth = gameScreen->GetWidth();
            int ScreenHeight = gameScreen->GetHeight();

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    if (x * 32 >= 0 && x * 32 < ScreenWidth && y * 32 >= 0 && y * 32 < ScreenHeight)
                    {
                        int tile = tiles[y][x] % 100;

                        int drawWidth = 32;
                        int drawHeight = 32;

                        if (x * 32 + 32 > ScreenWidth) {
                            drawWidth = ScreenWidth - x * 32;
                        }

                        if (y * 32 + 32 > ScreenHeight) {
                            drawHeight = ScreenHeight - y * 32;
                        }

                        if (tile < 0) continue;

                        float rad = 90 * (3.14159f / 180.0f); // Convert degrees to radians
                        //float cosTheta = cos(rad);
                        //float sinTheta = sin(rad);

                        //int newX = (x * cosTheta) - (y * sinTheta);
                        //int newY = (x * sinTheta) + (y * cosTheta);

                        Pixel* src = Tilessprite.GetBuffer() + 1 + tile * 33 + (1 + 0 * 33) * 595;
                        Pixel* dst = gameScreen->GetBuffer() + x * 32 + y * 32 * ScreenWidth;
                        for (int i = 0; i < drawHeight; i++)
                        {
                            for (int j = 0; j < drawWidth; j++)
                                dst[j] = src[j];
                            src += 595, dst += ScreenWidth;
                        }
                    }
                }
            }
        }


        bool Level::Collision(int y, int x)
        {
            int tx = clamp(x / 32, 0, width - 1), ty = clamp(y / 32, 0, height - 1);

            if (tiles[ty][tx] % 100 == 3) // spike
            {
                printf("%d\n", tiles[ty][tx]);

                return false;
            }

            return (tiles[ty][tx] % 100);
        }

        bool Level::SpikeColision(int playerY, int playerX, int gridRow, int gridCol)
        {
            float side1, side2, side3;
            bool has_neg, has_pos;
            int bottomLeftY, bottomLeftX, bottomRightY, bottomRightX, topCenterY, topCenterX;
            int ty = gridRow * 32, tx = gridCol * 32;

            bottomLeftY = ty + 32, bottomLeftX = tx;
            bottomRightY = ty + 32, bottomRightX = tx + 32;
            topCenterY = ty, topCenterX = tx + 16;

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

        double Level::clamp(double d, double min, double max)
        {
            const double t = d < min ? min : d;
            return t > max ? max : t;
        }

        int width, height;
}