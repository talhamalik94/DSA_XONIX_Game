// Enemy.cpp
#include <cstdlib>
#include <ctime>
using namespace std;

#include "sfml.h"
#include "Config.h"
#include "Grid.h"
#include "Enemy.h"

// Selected level from Screens.cpp (0 = easy, 1 = medium, 2 = hard)
extern int g_selectedLevel;

Enemy::Enemy()
{
    // random starting position inside the inner area
    int gx = 2 + rand() % (N - 4);
    int gy = 2 + rand() % (M - 4);

    x = gx * ts;
    y = gy * ts;

    int baseSpeed;
    if (g_selectedLevel == 0)       // Easy
        baseSpeed = 2;
    else if (g_selectedLevel == 1)  // Medium
        baseSpeed = 3;
    else                            // Hard
        baseSpeed = 4;

    int sx = (rand() % 2 == 0) ? -1 : 1;
    int sy = (rand() % 2 == 0) ? -1 : 1;

    dx = baseSpeed * sx;
    dy = baseSpeed * sy;
}

void Enemy::move()
{
    // move in x
    x += dx;
    int gx = x / ts;
    int gy = y / ts;

    if (gx < 0)
    {
        gx = 0;
        x = gx * ts;
        dx = -dx;
    }
    if (gx >= N)
    {
        gx = N - 1;
        x = gx * ts;
        dx = -dx;
    }

    if (gy < 0)
    {
        gy = 0;
        y = gy * ts;
        dy = -dy;
    }
    if (gy >= M)
    {
        gy = M - 1;
        y = gy * ts;
        dy = -dy;
    }

    if (grid[gy][gx] == 1)
    {
        dx = -dx;
        x += dx;
    }

    // move in y
    y += dy;
    gx = x / ts;
    gy = y / ts;

    if (gx < 0)
    {
        gx = 0;
        x = gx * ts;
        dx = -dx;
    }
    if (gx >= N)
    {
        gx = N - 1;
        x = gx * ts;
        dx = -dx;
    }

    if (gy < 0)
    {
        gy = 0;
        y = gy * ts;
        dy = -dy;
    }
    if (gy >= M)
    {
        gy = M - 1;
        y = gy * ts;
        dy = -dy;
    }

    if (grid[gy][gx] == 1)
    {
        dy = -dy;
        y += dy;
    }
}
