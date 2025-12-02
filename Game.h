// Game.h
#pragma once

#include "sfml.h"
#include "Theme.h"
#include "SaveGame.h"

int runSinglePlayerGame(RenderWindow& window, const Theme& theme,  const GameState *loadedState = nullptr);

int runMultiplayerGame(RenderWindow& window, const Theme& theme, const GameState *loadedState = nullptr);