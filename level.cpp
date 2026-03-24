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

            tiles[5][5] = 1;
            tiles[2][3] = 6;
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

        bool Level::Collision(int y, int x)
        {
            int tx = clamp(x / 32,0,width -1), ty = clamp(y / 32,0,height -1);

            return (tiles[ty][tx]);
        }

        float Level::GetTileEdge(float checkY, float checkX) {
            int topY = (int)(checkY / 32);
            int bottomY = topY + 1;

            float topEdge = topY * 32.0f;
            float bottomEdge = bottomY * 32.0f;

            if (fabs(checkY - topEdge) <= fabs(checkY - bottomEdge)) {
                return topEdge;
            }

            return bottomEdge;
        }

        double Level::clamp(double d, double min, double max)
        {
            const double t = d < min ? min : d;
            return t > max ? max : t;
        }

        int width, height;
}