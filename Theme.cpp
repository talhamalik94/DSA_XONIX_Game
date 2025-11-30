// Theme.cpp
#include <iostream>
#include "Theme.h"

using namespace std;

bool loadTheme(Theme& theme)
{
    if (!theme.font.loadFromFile("fonts/MadisonStreet.otf"))
    {
        cout << "Could not load font file\n";
        return false;
    }
     if (!theme.font.loadFromFile("fonts/orbitron-bold.otf"))
    {
        cout << "Could not load OrbitronBold.otf\n";
        return false;
    }

    // Default colors (you can tweak)
    theme.backgroundColor    = Color(10, 10, 25);
    theme.textColor          = Color(220, 220, 220);
    theme.accentColor        = Color(120, 180, 255);
    theme.highlightColor     = Color(255, 200, 50);
    theme.starColor          = Color(140, 190, 255);
    theme.hudBackgroundColor = Color(0, 0, 0, 130);

    theme.hasMenuBackgroundTexture = false;
    theme.hasGameBackgroundTexture = false;

    return true;
}

bool loadMenuBackground(Theme& theme, const std::string& path)
{
    theme.hasMenuBackgroundTexture = false;

    if (!path.empty() && theme.menuBackgroundTexture.loadFromFile(path))
    {
        theme.hasMenuBackgroundTexture = true;
        return true;
    }
    return false;
}

bool loadGameBackground(Theme& theme, const std::string& path)
{
    theme.hasGameBackgroundTexture = false;

    if (!path.empty() && theme.gameBackgroundTexture.loadFromFile(path))
    {
        theme.hasGameBackgroundTexture = true;
        return true;
    }
    return false;
}

void drawMenuBackground(RenderWindow& window, const Theme& theme)
{
    window.clear(theme.backgroundColor);

    if (theme.hasMenuBackgroundTexture)
    {
        Sprite bg;
        bg.setTexture(theme.menuBackgroundTexture);

        float texW = (float) theme.menuBackgroundTexture.getSize().x;
        float texH = (float) theme.menuBackgroundTexture.getSize().y;
        float winW = (float) window.getSize().x;
        float winH = (float) window.getSize().y;

        float scaleX = winW / texW;
        float scaleY = winH / texH;

        bg.setScale(scaleX, scaleY);
        bg.setPosition(0.f, 0.f);

        window.draw(bg);
    }
}

void drawGameBackground(RenderWindow& window, const Theme& theme)
{
    window.clear(theme.backgroundColor);

    if (theme.hasGameBackgroundTexture)
    {
        Sprite bg;
        bg.setTexture(theme.gameBackgroundTexture);

        float texW = (float) theme.gameBackgroundTexture.getSize().x;
        float texH = (float) theme.gameBackgroundTexture.getSize().y;
        float winW = (float) window.getSize().x;
        float winH = (float) window.getSize().y;

        float scaleX = winW / texW;
        float scaleY = winH / texH;

        bg.setScale(scaleX, scaleY);
        bg.setPosition(0.f, 0.f);

        window.draw(bg);
    }
}
