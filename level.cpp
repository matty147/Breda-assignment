#include "level.h"
#include "tmpl8/surface.h"
#include <windows.h>
#include <cstdio>
#include <vector>
#include <cmath>

std::vector<std::vector<int>> tiles;

namespace Tmpl8
{
    extern Sprite Tilessprite;
    int test = 0;

        Level::Level(int iwidth, int iheight)
        {
            width = iwidth;
            height = iheight;

            tiles.resize(height, std::vector<int>(width, 0));
        }

        void Level::CreateLevel()
        {
            for (int x = 0; x < width; x++)
            {
                tiles[10][x] = 1;
                tiles[11][x] = 2;
                tiles[12][x] = 2;
                tiles[13][x] = 2;
                tiles[14][x] = 2;
                tiles[15][x] = 2;

            }

            tiles[9][8] = 3;

            tiles[5][5] = 1;
            tiles[6][5] = 1;
            tiles[7][5] = 1;
            tiles[8][5] = 1;
            tiles[9][5] = 1;
            tiles[2][3] = 6;
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
                        int tile = tiles[y][x];

                        int drawWidth = 32;
                        int drawHeight = 32;

                        if (x * 32 + 32 > ScreenWidth) {
                            drawWidth = ScreenWidth - x * 32;
                        }

                        if (y * 32 + 32 > ScreenHeight) {
                            drawHeight = ScreenHeight - y * 32;
                        }

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

            if (tiles[ty][tx] == 3) // spike
            {
                return false;
            }

            return (tiles[ty][tx]);
        }
         
        double Level::clamp(double d, double min, double max)
        {
            const double t = d < min ? min : d;
            return t > max ? max : t;
        }

        int width, height;
}