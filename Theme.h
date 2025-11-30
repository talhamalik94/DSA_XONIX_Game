// Theme.h
#pragma once

#include "sfml.h"
#include <string>

struct Theme
{
    Font  font;

    // Colors
    Color backgroundColor;
    Color textColor;
    Color accentColor;
    Color highlightColor;
    Color starColor;
    Color hudBackgroundColor;

    // Background textures
    Texture menuBackgroundTexture;   // used on menus
    Texture gameBackgroundTexture;   // used during gameplay
    bool hasMenuBackgroundTexture;
    bool hasGameBackgroundTexture;
};

// Load font and set default colors
bool loadTheme(Theme& theme);

// Draw menu background (used in Screens.cpp)
void drawMenuBackground(RenderWindow& window, const Theme& theme);

// Draw game background (used in Game.cpp when you want image behind grid)
void drawGameBackground(RenderWindow& window, const Theme& theme);

// Load/update images
bool loadMenuBackground(Theme& theme, const std::string& path);
bool loadGameBackground(Theme& theme, const std::string& path);
