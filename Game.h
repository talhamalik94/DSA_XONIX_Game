// Game.h
#pragma once

#include "sfml.h"
#include "Theme.h"

// Run one single-player session, return final score
int runSinglePlayerGame(RenderWindow& window, const Theme& theme);

// Run a local 2-player match (players take turns on same PC)
int runMultiplayerGame(RenderWindow& window, const Theme& theme);
