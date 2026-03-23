#include "level.h"
#include "tmpl8/surface.h"
#include <windows.h>
#include <cstdio>
#include <vector>

std::vector<std::vector<int>> tiles;

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
            for (int x = 0; x < width; x++)
            {
                tiles[height - 1][x] = 1;
            }
        }

        void Level::Draw(Surface* gameScreen) {

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int tile = tiles[y][x];

                    Pixel* src = Tilessprite.GetBuffer() + 1 + tile * 33 + (1 + 0 * 33) * 595;
                    Pixel* dst = gameScreen->GetBuffer() + x * 32 + y * 32 * 800;
                    for (int i = 0; i < 32; i++)
                    {
                        for (int j = 0; j < 32; j++)
                            dst[j] = src[j];
                        src += 595, dst += 800;
                    }
                }
            }

        }

        bool Level::Collision(int x, int y)
        {
            int tx = x / 32, ty = y / 32;

            return (tiles[ty][tx]);
        }

        int width, height;
}