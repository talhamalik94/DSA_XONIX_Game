// Screens.h
#pragma once

#include "sfml.h"
#include "Theme.h"

// Application states
enum class AppState
{
    LANDING_MENU,          // first screen: Login / Register / Forgot / Exit
    LOGIN_FORM,            // login page
    REGISTER_FORM,         // register page
    FORGOT_PASSWORD_FORM,  // change / reset password page

    PLAYER_MENU,           // menu shown after successful login

    SINGLE_PLAYER,         // single player Xonix
    MULTIPLAYER,           // local 2-player (turn by turn)
    SELECT_LEVEL,          // level selection screen
    LEADERBOARD,           // top players screen
    MATCHMAKING,           // matchmaking / game rooms screen
    FRIEND_SYSTEM,         // friend list and requests
    THEME_INVENTORY,       // theme inventory (AVL) screen
    SAVE_LOAD_GAME,        // save and load game screen
    INSTRUCTIONS,          // how to play
    SETTINGS,              // general settings

    VIEW_FRIENDS,
    VIEW_FRIEND_REQUESTS,
    SEND_FRIEND_REQUEST,

    PROFILE_SCREEN,
    MATCH_HISTORY_SCREEN,

    EXIT_APP
};

// First screen (Login / Register / Forgot / Exit)
AppState showLandingMenu(RenderWindow& window, const Theme& theme);

// Menu after successful login (Start Game / Settings / etc.)
AppState showPlayerMenu(RenderWindow& window, const Theme& theme);

// Leaderboard screen
AppState showLeaderboardScreen(RenderWindow& window, const Theme& theme);

// Matchmaking screen
AppState showMatchmakingScreen(RenderWindow& window, const Theme& theme);

// Friend system screens
AppState showFriendSystemScreen(RenderWindow& window, const Theme& theme);
AppState showFriendRequestsScreen(RenderWindow& window, const Theme& theme);
AppState showSendFriendRequestScreen(RenderWindow& window, const Theme& theme);
AppState showViewFriendsScreen(RenderWindow& window, const Theme& theme);

// Level / settings / instructions
AppState showLevelSelectScreen(RenderWindow& window, const Theme& theme);
AppState showSettingsScreen(RenderWindow& window, const Theme& theme);
AppState showInstructionsScreen(RenderWindow& window, const Theme& theme);

// Profile / match history
AppState showProfileScreen(RenderWindow& window, const Theme& theme);
AppState showMatchHistoryScreen(RenderWindow& window, const Theme& theme);

// Multiplayer wrapper screen
AppState showMultiplayerScreen(RenderWindow& window, const Theme& theme);

// Theme inventory
AppState showThemeInventoryScreen(RenderWindow& window, Theme& theme);

AppState showSaveLoadGameScreen(RenderWindow &window, const Theme &theme);


// Forms
int runLoginScreen(RenderWindow& window, Theme& theme);
int runRegisterScreen(RenderWindow& window, Theme& theme);
int runForgotPasswordScreen(RenderWindow& window, Theme& theme);
