// Enemy.h
#pragma once

#include "Config.h"   // for M, N, ts

class Enemy
{
public:
    int x;
    int y;
    int dx;
    int dy;

    Enemy();
    void move();
};
