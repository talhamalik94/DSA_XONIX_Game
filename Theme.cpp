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

    theme.backgroundColor   = Color(10, 10, 25);
    theme.textColor         = Color(220, 220, 220);
    theme.accentColor       = Color(120, 180, 255);
    theme.highlightColor    = Color(255, 200, 50);
    theme.starColor         = Color(140, 190, 255);
    theme.hudBackgroundColor = Color(0, 0, 0, 130);

    return true;
}
