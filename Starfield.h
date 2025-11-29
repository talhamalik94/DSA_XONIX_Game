// Starfield.h
#pragma once

#include "sfml.h"

// Simple star structure
struct Star
{
    float x;
    float y;
    float speed;
};

const int STAR_COUNT = 120;

// Starfield class draws moving stars in background
class Starfield
{
private:
    Star stars[STAR_COUNT];
    int width;
    int height;

public:
    Starfield(int w = 0, int h = 0);

    void update(float dt);
    void draw(RenderWindow& window, Color starColor);
};
