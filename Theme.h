// Theme.h
#pragma once

#include "sfml.h"

struct Theme
{
    Font  font;
    Color backgroundColor;
    Color textColor;
    Color accentColor;
    Color highlightColor;
    Color starColor;
    Color hudBackgroundColor;
};

bool loadTheme(Theme& theme);
