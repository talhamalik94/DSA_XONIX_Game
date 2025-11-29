// Starfield.cpp
#include <cstdlib>
#include <ctime>
#include "Starfield.h"

using namespace std;

Starfield::Starfield(int w, int h)
{
    width  = w;
    height = h;

    srand(time(0));

    for (int i = 0; i < STAR_COUNT; i++)
    {
        stars[i].x = rand() % width;
        stars[i].y = rand() % height;
        stars[i].speed = 20.f + (rand() % 40); // 20–60
    }
}

void Starfield::update(float dt)
{
    for (int i = 0; i < STAR_COUNT; i++)
    {
        stars[i].y += stars[i].speed * dt;

        if (stars[i].y > height)
        {
            stars[i].y = 0;
            stars[i].x = rand() % width;
        }
    }
}

void Starfield::draw(RenderWindow& window, Color starColor)
{
    for (int i = 0; i < STAR_COUNT; i++)
    {
        Circle starShape;
        starShape.setRadius(1.5f);
        starShape.setFillColor(starColor);
        starShape.setPosition(stars[i].x, stars[i].y);
        window.draw(starShape);
    }
}
