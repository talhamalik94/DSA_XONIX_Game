// main.cpp
#include <iostream>
using namespace std;

#include "sfml.h"
#include "Theme.h"
#include "Screens.h"
#include "Game.h"
#include "PlayerDatabase.h"
#include "Config.h"
#include "FriendSystem.h"
#include "ThemeInventory.h"

// database and current player created in Screens.cpp
extern PlayerDatabase g_playerDb;
extern int g_currentPlayer;

int main()
{
    Theme theme;
    if (!loadTheme(theme))
    {
        cout << "Failed to load theme font.\n";
        return 1;
    }
    initDefaultThemes();
    const int tileSize = ts; // from Config.h

    RenderWindow window(VideoMode(N * tileSize, M * tileSize), "Xonix Game");
    window.setFramerateLimit(60);

    AppState state = AppState::LANDING_MENU;
    bool running = true;

    while (running && window.isOpen())
    {
        if (state == AppState::LANDING_MENU)
        {
            state = showLandingMenu(window, theme);
        }
        else if (state == AppState::LOGIN_FORM)
        {
            int ok = runLoginScreen(window, theme);
            if (ok)
                state = AppState::PLAYER_MENU;
            else
                state = AppState::LANDING_MENU;
        }
        else if (state == AppState::REGISTER_FORM)
        {
            runRegisterScreen(window, theme);
            state = AppState::LANDING_MENU;
        }
        else if (state == AppState::FORGOT_PASSWORD_FORM)
        {
            runForgotPasswordScreen(window, theme);
            state = AppState::LANDING_MENU;
        }
        else if (state == AppState::PLAYER_MENU)
        {
            state = showPlayerMenu(window, theme);
        }
        else if (state == AppState::SINGLE_PLAYER)
        {
            int score = runSinglePlayerGame(window, theme);

            // update current player's totalScore for leaderboard
            if (g_currentPlayer != -1 && score > 0)
            {
                g_playerDb.updateScore(g_currentPlayer, score);
            }

            state = AppState::PLAYER_MENU; // back to menu after game
        }
        else if (state == AppState::MULTIPLAYER)
        {
            // local turn-by-turn 2-player mode
            runMultiplayerGame(window, theme);
            state = AppState::PLAYER_MENU;
        }
        else if (state == AppState::LEADERBOARD)
        {
            state = showLeaderboardScreen(window, theme);
        }
        else if (state == AppState::MATCHMAKING)
        {
            state = showMatchmakingScreen(window, theme);
        }
        else if (state == AppState::FRIEND_SYSTEM)
        {
            state = showFriendSystemScreen(window, theme);
        }
        else if (state == AppState::VIEW_FRIEND_REQUESTS)
        {
            state = showFriendRequestsScreen(window, theme);
        }
        else if (state == AppState::SEND_FRIEND_REQUEST)
        {
            state = showSendFriendRequestScreen(window, theme);
        }
        else if (state == AppState::VIEW_FRIENDS)
        {
            state = showViewFriendsScreen(window, theme);
        }
        else if (state == AppState::SELECT_LEVEL)
        {
            state = showLevelSelectScreen(window, theme);
        }
        else if (state == AppState::SETTINGS)
        {
            state = showSettingsScreen(window, theme);
        }
        else if (state == AppState::INSTRUCTIONS)
        {
            state = showInstructionsScreen(window, theme);
        }
        else if (state == AppState::PROFILE_SCREEN)
        {
            state = showProfileScreen(window, theme);
        }
        else if (state == AppState::MATCH_HISTORY_SCREEN)
        {
            state = showMatchHistoryScreen(window, theme);
        }
        else if (state == AppState::THEME_INVENTORY)
        {
            state = showThemeInventoryScreen(window, theme);
        }

        else if (state == AppState::EXIT_APP)
        {
            running = false;
        }
    }

    return 0;
}
