// Screens.cpp
#include <iostream>
using namespace std;

#include "sfml.h"
#include "Game.h"
#include "Theme.h"
#include "PlayerDatabase.h"
#include "Screens.h"
#include "Leaderboard.h"
#include "MatchMaking.h"
#include "FriendSystem.h"
#include "MatchHistory.h"  

// Global database and current player index
PlayerDatabase g_playerDb("players.txt");
int g_currentPlayer = -1;

// Global friend system. We pass address of database.
FriendSystem g_friendSystem(&g_playerDb);

MatchmakingSystem g_matchmaking;
extern PlayerDatabase g_playerDb;
extern int g_currentPlayer;

int g_selectedLevel = 0; // 0 = Easy, 1 = Medium, 2 = Hard

// ---------------------------------------------
// LANDING MENU  (first page)
// Options: Login / Register / Forgot Password / Exit
// ---------------------------------------------
AppState showLandingMenu(RenderWindow &window, const Theme &theme)
{
    int selectedIndex = 0;

    const int optionCount = 4;
    const char *options[optionCount] = {
        "Login",
        "Register",
        "Forgot Password",
        "Exit"};

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return AppState::EXIT_APP;
            }

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                {
                    selectedIndex = (selectedIndex - 1 + optionCount) % optionCount;
                }
                else if (event.key.code == Keyboard::Down)
                {
                    selectedIndex = (selectedIndex + 1) % optionCount;
                }
                else if (event.key.code == Keyboard::Enter)
                {
                    if (selectedIndex == 0)
                        return AppState::LOGIN_FORM;
                    if (selectedIndex == 1)
                        return AppState::REGISTER_FORM;
                    if (selectedIndex == 2)
                        return AppState::FORGOT_PASSWORD_FORM;
                    if (selectedIndex == 3)
                        return AppState::EXIT_APP;
                }
            }
        }

        window.clear(theme.backgroundColor);

        // title "XONIX LOGIN"
        Text title;
        title.setFont(theme.font);
        title.setString("XONIX LOGIN");
        title.setCharacterSize(56);
        title.setFillColor(theme.accentColor);

        FloatRect tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        title.setPosition(window.getSize().x / 2.f, 80.f);
        window.draw(title);

        // base button colors (normal)
        Color buttonColors[4] = {
            Color(180, 40, 40), // Login   - red
            Color(40, 90, 200), // Register - blue
            Color(40, 140, 60), // Forgot Password - green
            Color(70, 70, 70)   // Exit - dark gray
        };

        // loop to draw options as buttons
        // loop to draw options as buttons
        for (int i = 0; i < optionCount; i++)
        {
            // text for this option
            Text opt;
            opt.setFont(theme.font);
            opt.setString(options[i]);
            opt.setCharacterSize(28);

            // normal text color
            Color txtColor = Color::White;

            // position of text (centered)
            FloatRect ob = opt.getLocalBounds();
            float centerX = window.getSize().x / 2.f;
            float centerY = 170.f + i * 60.f; // vertical spacing

            opt.setOrigin(ob.left + ob.width / 2.f, ob.top + ob.height / 2.f);
            opt.setPosition(centerX, centerY);

            // button rectangle size (a bit larger than text)
            float paddingX = 40.f;
            float paddingY = 10.f;
            sf::Vector2f buttonSize(ob.width + paddingX, ob.height + paddingY);

            sf::RectangleShape button;
            button.setSize(buttonSize);
            button.setOrigin(buttonSize.x / 2.f, buttonSize.y / 2.f);
            button.setPosition(centerX, centerY);

            // base color for this button
            Color btnColor = buttonColors[i];

            // highlight selected
            if (i == selectedIndex)
            {
                btnColor = Color(
                    min(255, btnColor.r + 40),
                    min(255, btnColor.g + 40),
                    min(255, btnColor.b + 40));
                button.setOutlineThickness(2.f);
                button.setOutlineColor(theme.highlightColor);
                txtColor = Color::Yellow;
            }

            button.setFillColor(btnColor);
            opt.setFillColor(txtColor);

            window.draw(button);
            window.draw(opt);
        }

        window.display();
    }

    return AppState::EXIT_APP;
}

// Forward declaration for forgot screen (used by login)
int runForgotPasswordScreen(RenderWindow &window, Theme &theme);

// ---------------------------------------------
// LOGIN FORM
// Tab switches between username and password.
// Enter = attempt login
// Esc = back to landing menu
// F = open forgot password form
// ---------------------------------------------
int runLoginScreen(RenderWindow &window, Theme &theme)
{
    string username = "";
    string password = "";
    string message = "";

    Text title("LOGIN", theme.font, 42);
    title.setFillColor(theme.textColor);
    title.setPosition(200, 40);

    Text msgText("", theme.font, 22);
    msgText.setFillColor(Color::Red);
    msgText.setPosition(200, 280);

    Text promptUser("Username: ", theme.font, 26);
    promptUser.setPosition(200, 130);

    Text promptPass("Password: ", theme.font, 26);
    promptPass.setPosition(200, 190);

    Text userText("", theme.font, 26);
    userText.setFillColor(Color::White);
    userText.setPosition(350, 130);

    Text passText("", theme.font, 26);
    passText.setFillColor(Color::White);
    passText.setPosition(350, 190);

    Text backHint("ESC = Back", theme.font, 20);
    backHint.setFillColor(theme.textColor);
    backHint.setPosition(200, 330);

    Text forgotHint("F = Forgot Password", theme.font, 20);
    forgotHint.setFillColor(theme.textColor);
    forgotHint.setPosition(200, 360);

    bool typingUser = true;
    bool typingPass = false;

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                window.close();
                return 0;
            }

            if (e.type == Event::TextEntered)
            {
                char c = (char)e.text.unicode;

                if (typingUser)
                {
                    if (c == 8 && !username.empty())
                        username.pop_back();
                    else if (c >= 32 && c <= 126)
                        username.push_back(c);
                }
                else if (typingPass)
                {
                    if (c == 8 && !password.empty())
                        password.pop_back();
                    else if (c >= 32 && c <= 126)
                        password.push_back(c);
                }
            }

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Tab)
                {
                    typingUser = !typingUser;
                    typingPass = !typingPass;
                }
                if (e.key.code == Keyboard::Enter)
                {
                    string err;
                    int idx = g_playerDb.authenticate(username, password, err);
                    if (idx >= 0)
                    {
                        g_currentPlayer = idx;
                        message = "Login successful";
                        return 1; // success
                    }
                    else
                    {
                        message = err;
                    }
                }
                if (e.key.code == Keyboard::F)
                {
                    // Open forgot password screen
                    runForgotPasswordScreen(window, theme);
                    message = ""; // clear message after returning
                }
                if (e.key.code == Keyboard::Escape)
                {
                    return 0; // back to landing menu
                }
            }
        }

        msgText.setString(message);
        userText.setString(username);
        passText.setString(string(password.size(), '*'));

        window.clear(theme.backgroundColor);

        window.draw(title);
        window.draw(promptUser);
        window.draw(promptPass);
        window.draw(userText);
        window.draw(passText);
        window.draw(msgText);
        window.draw(backHint);
        window.draw(forgotHint);

        window.display();
    }

    return 0;
}

// ---------------------------------------------
// REGISTER FORM
// Up / Down / Tab move between fields
// Enter submits
// Esc back to landing
// ---------------------------------------------
int runRegisterScreen(RenderWindow &window, Theme &theme)
{
    string message = "";
    string values[4] = {"", "", "", ""}; // user, pass, nick, email
    int currentField = 0;

    Text title("REGISTER", theme.font, 42);
    title.setFillColor(theme.textColor);
    title.setPosition(180, 40);

    Text msgText("", theme.font, 22);
    msgText.setFillColor(Color::Red);
    msgText.setPosition(180, 360);

    Text labels[4] = {
        Text("Username: ", theme.font, 26),
        Text("Password: ", theme.font, 26),
        Text("Nickname: ", theme.font, 26),
        Text("Email: ", theme.font, 26)};

    for (int i = 0; i < 4; i++)
        labels[i].setPosition(150, 120 + i * 60);

    Text valueTexts[4];
    for (int i = 0; i < 4; i++)
    {
        valueTexts[i].setFont(theme.font);
        valueTexts[i].setFillColor(Color::White);
        valueTexts[i].setCharacterSize(26);
        valueTexts[i].setPosition(330, 120 + i * 60);
    }

    Text backHint("ESC = Back", theme.font, 20);
    backHint.setFillColor(theme.textColor);
    backHint.setPosition(180, 400);

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                window.close();
                return 0;
            }

            if (e.type == Event::TextEntered)
            {
                char c = (char)e.text.unicode;

                if (c == 8 && !values[currentField].empty())
                    values[currentField].pop_back();
                else if (c >= 32 && c <= 126)
                    values[currentField].push_back(c);
            }

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Tab || e.key.code == Keyboard::Down)
                    currentField = (currentField + 1) % 4;

                if (e.key.code == Keyboard::Up)
                    currentField = (currentField + 3) % 4;

                if (e.key.code == Keyboard::Enter)
                {
                    string err;
                    bool ok = g_playerDb.registerPlayer(
                        values[0], values[1], values[2], values[3], err);

                    if (ok)
                    {
                        message = "Registered successfully";

                        // New player added => refresh username → index hash table
                        g_friendSystem.buildHashTable();

                        return 1; // success
                    }
                    else
                    {
                        message = err;
                    }
                }

                if (e.key.code == Keyboard::Escape)
                    return 0;
            }
        }

        msgText.setString(message);

        for (int i = 0; i < 4; i++)
        {
            if (i == 1)
                valueTexts[i].setString(string(values[i].size(), '*'));
            else
                valueTexts[i].setString(values[i]);

            labels[i].setFillColor(i == currentField
                                       ? theme.highlightColor
                                       : theme.textColor);
        }

        window.clear(theme.backgroundColor);

        window.draw(title);
        for (int i = 0; i < 4; i++)
        {
            window.draw(labels[i]);
            window.draw(valueTexts[i]);
        }
        window.draw(msgText);
        window.draw(backHint);

        window.display();
    }

    return 0;
}

// ---------------------------------------------
// FORGOT PASSWORD / CHANGE PASSWORD FORM
// Fields: Username, New Password, Confirm Password
// ---------------------------------------------
int runForgotPasswordScreen(RenderWindow &window, Theme &theme)
{
    string values[3] = {"", "", ""}; // user, new pass, confirm
    int currentField = 0;
    string message = "";

    Text title("CHANGE PASSWORD", theme.font, 32);
    title.setFillColor(theme.textColor);
    title.setPosition(150, 40);

    Text msgText("", theme.font, 22);
    msgText.setFillColor(Color::Red);
    msgText.setPosition(150, 320);

    Text labels[3] = {
        Text("Username: ", theme.font, 24),
        Text("New Password: ", theme.font, 24),
        Text("Confirm Password: ", theme.font, 24)};

    for (int i = 0; i < 3; i++)
        labels[i].setPosition(140, 120 + i * 60);

    Text valueTexts[3];
    for (int i = 0; i < 3; i++)
    {
        valueTexts[i].setFont(theme.font);
        valueTexts[i].setFillColor(Color::White);
        valueTexts[i].setCharacterSize(24);
        valueTexts[i].setPosition(330, 120 + i * 60);
    }

    Text hint("ESC = Back   ENTER = Submit", theme.font, 20);
    hint.setFillColor(theme.textColor);
    hint.setPosition(150, 360);

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                window.close();
                return 0;
            }

            if (e.type == Event::TextEntered)
            {
                char c = (char)e.text.unicode;

                if (c == 8 && !values[currentField].empty())
                    values[currentField].pop_back();
                else if (c >= 32 && c <= 126)
                    values[currentField].push_back(c);
            }

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Tab || e.key.code == Keyboard::Down)
                    currentField = (currentField + 1) % 3;

                if (e.key.code == Keyboard::Up)
                    currentField = (currentField + 2) % 3;

                if (e.key.code == Keyboard::Enter)
                {
                    string username = values[0];
                    string newPass = values[1];
                    string confirm = values[2];

                    if (username.empty() || newPass.empty() || confirm.empty())
                        message = "All fields are required";
                    else if (newPass != confirm)
                        message = "Passwords do not match";
                    else
                    {
                        string err;
                        bool ok = g_playerDb.changePassword(username, newPass, err);
                        if (ok)
                        {
                            message = "Password changed";
                            return 1;
                        }
                        else
                        {
                            message = err;
                        }
                    }
                }

                if (e.key.code == Keyboard::Escape)
                    return 0;
            }
        }

        msgText.setString(message);

        for (int i = 0; i < 3; i++)
        {
            if (i == 0)
                valueTexts[i].setString(values[i]);
            else
                valueTexts[i].setString(string(values[i].size(), '*'));

            labels[i].setFillColor(i == currentField
                                       ? theme.highlightColor
                                       : theme.textColor);
        }

        window.clear(theme.backgroundColor);

        window.draw(title);
        for (int i = 0; i < 3; i++)
        {
            window.draw(labels[i]);
            window.draw(valueTexts[i]);
        }
        window.draw(msgText);
        window.draw(hint);

        window.display();
    }

    return 0;
}

// ---------------------------------------------
// PLAYER MENU  (shown AFTER successful login)
// Options in this order:
//  0 Start Game
//  1 Multiplayer
//  2 Select Level
//  3 Leaderboard
//  4 Matchmaking
//  5 Friend System
//  6 Theme Inventory
//  7 Save / Load Game
//  8 Instructions
//  9 Settings
// 10 Logout
// ---------------------------------------------
AppState showPlayerMenu(RenderWindow &window, const Theme &theme)
{
    int selectedIndex = 0;

    const int optionCount = 13;
    const char *options[optionCount] = {
        "Start Game",
        "Multiplayer",
        "Select Level",
        "Leaderboard",
        "Matchmaking",
        "Friend System",
        "Theme Inventory",
        "Save / Load Game",
        "Instructions",
        "Settings",
        "View Profile",
        "Match History",
        "Logout"};

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return AppState::EXIT_APP;
            }

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                {
                    selectedIndex = (selectedIndex - 1 + optionCount) % optionCount;
                }
                else if (event.key.code == Keyboard::Down)
                {
                    selectedIndex = (selectedIndex + 1) % optionCount;
                }
                else if (event.key.code == Keyboard::Enter)
                {
                    if (event.key.code == Keyboard::Enter)
                    {
                        if (selectedIndex == 0)
                            return AppState::SINGLE_PLAYER;
                        if (selectedIndex == 1)
                            return AppState::MULTIPLAYER;
                        if (selectedIndex == 2)
                            return AppState::SELECT_LEVEL;
                        if (selectedIndex == 3)
                            return AppState::LEADERBOARD;
                        if (selectedIndex == 4)
                            return AppState::MATCHMAKING;
                        if (selectedIndex == 5)
                            return AppState::FRIEND_SYSTEM;
                        if (selectedIndex == 6)
                            return AppState::THEME_INVENTORY;
                        if (selectedIndex == 7)
                            return AppState::SAVE_LOAD_GAME;
                        if (selectedIndex == 8)
                            return AppState::INSTRUCTIONS;
                        if (selectedIndex == 9)
                            return AppState::SETTINGS;
                        if (selectedIndex == 10)
                            return AppState::PROFILE_SCREEN;
                        if (selectedIndex == 11)
                            return AppState::MATCH_HISTORY_SCREEN;
                        if (selectedIndex == 12)
                            return AppState::EXIT_APP; // Logout
                    }
                }
                else if (event.key.code == Keyboard::Escape)
                {
                    // Esc logs out back to landing menu
                    return AppState::LANDING_MENU;
                }
            }
        }

        window.clear(theme.backgroundColor);

        // Title "XONIX"
        Text title;
        title.setFont(theme.font);
        title.setString("XONIX");
        title.setCharacterSize(56);
        title.setFillColor(theme.accentColor);

        FloatRect tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        title.setPosition(window.getSize().x / 2.f, 70.f);
        window.draw(title);

        // Draw options as a vertical list
        for (int i = 0; i < optionCount; i++)
        {
            Text opt;
            opt.setFont(theme.font);
            opt.setString(options[i]);
            opt.setCharacterSize(28);
            opt.setFillColor(i == selectedIndex ? theme.highlightColor : theme.textColor);

            FloatRect ob = opt.getLocalBounds();
            opt.setOrigin(ob.left + ob.width / 2.f, ob.top + ob.height / 2.f);
            opt.setPosition(window.getSize().x / 2.f, 160.f + i * 36.f);

            window.draw(opt);
        }

        window.display();
    }

    return AppState::EXIT_APP;
}

AppState showLeaderboardScreen(RenderWindow &window, const Theme &theme)
{
    // Build leaderboard from all players in the database
    Leaderboard lb;
    lb.buildFromDatabase(g_playerDb);

    LeaderboardEntry list[10];
    int count = 0;
    lb.toSortedArray(list, count);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return AppState::EXIT_APP;
            }

            if (event.type == Event::KeyPressed)
            {
                // ESC or ENTER go back to player menu
                if (event.key.code == Keyboard::Escape ||
                    event.key.code == Keyboard::Enter)
                {
                    return AppState::PLAYER_MENU;
                }
            }
        }

        window.clear(theme.backgroundColor);

        // Title
        Text title;
        title.setFont(theme.font);
        title.setString("LEADERBOARD");
        title.setCharacterSize(40);
        title.setFillColor(theme.accentColor);

        FloatRect tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        title.setPosition(window.getSize().x / 2.f, 40.f);
        window.draw(title);

        // Header
        Text header;
        header.setFont(theme.font);
        header.setCharacterSize(24);
        header.setFillColor(theme.textColor);
        header.setString("Rank    Username                Score");
        header.setPosition(80.f, 90.f);
        window.draw(header);

        if (count == 0)
        {
            Text noData;
            noData.setFont(theme.font);
            noData.setCharacterSize(22);
            noData.setFillColor(theme.textColor);
            noData.setString("No players yet. Play a game to appear here.");
            noData.setPosition(80.f, 130.f);
            window.draw(noData);
        }
        else
        {
            for (int i = 0; i < count; i++)
            {
                const Player &p = g_playerDb.getPlayer(list[i].playerIndex);

                string line = "";
                line += std::to_string(i + 1);
                line += ") ";
                line += p.username;
                line += "  -  ";
                line += std::to_string(list[i].score);

                Text row;
                row.setFont(theme.font);
                row.setCharacterSize(22);
                row.setFillColor(theme.textColor);
                row.setString(line);
                row.setPosition(80.f, 130.f + i * 28.f);
                window.draw(row);
            }
        }

        Text hint("ESC / ENTER = Back", theme.font, 20);
        hint.setFillColor(theme.textColor);
        hint.setPosition(80.f, window.getSize().y - 40.f);
        window.draw(hint);

        window.display();
    }

    return AppState::EXIT_APP;
}

AppState showMatchmakingScreen(RenderWindow &window, const Theme &theme)
{
    bool justJoined = false;
    string statusMessage = "Press ENTER to join matchmaking queue.";

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return AppState::EXIT_APP;
            }

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                {
                    return AppState::PLAYER_MENU;
                }
                else if (event.key.code == Keyboard::Enter)
                {
                    if (g_currentPlayer < 0)
                    {
                        statusMessage = "You must be logged in to join matchmaking.";
                    }
                    else
                    {
                        // Get current player's score from database
                        const Player &p = g_playerDb.getPlayer(g_currentPlayer);
                        int score = p.totalScore;

                        bool added = g_matchmaking.addPlayer(g_currentPlayer, score);
                        if (added)
                        {
                            statusMessage = "You joined the matchmaking queue. Waiting for opponent...";
                            justJoined = true;

                            // form matches if possible
                            g_matchmaking.createMatches();

                            // check if we already got matched
                            int opponentIndex = -1;
                            bool gotMatch = g_matchmaking.getMatchForPlayer(g_currentPlayer, opponentIndex);
                            if (gotMatch)
                            {
                                const Player &opp = g_playerDb.getPlayer(opponentIndex);
                                statusMessage = "Matched with: " + opp.username + ". Press ESC to go back.";
                            }
                        }
                        else
                        {
                            statusMessage = "You are already in the matchmaking queue.";
                        }
                    }
                }
            }
        }

        window.clear(theme.backgroundColor);

        // Title
        Text title;
        title.setFont(theme.font);
        title.setString("Matchmaking");
        title.setCharacterSize(40);
        title.setFillColor(theme.accentColor);
        FloatRect tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        title.setPosition(window.getSize().x / 2.f, 60.f);
        window.draw(title);

        // Instruction text
        Text info;
        info.setFont(theme.font);
        info.setCharacterSize(22);
        info.setFillColor(theme.textColor);
        info.setString("ENTER = Join queue, ESC = Back");
        info.setPosition(40.f, 120.f);
        window.draw(info);

        // Status message
        Text status;
        status.setFont(theme.font);
        status.setCharacterSize(20);
        status.setFillColor(theme.textColor);
        status.setString(statusMessage);
        status.setPosition(40.f, 170.f);
        window.draw(status);

        window.display();
    }

    return AppState::EXIT_APP;
}

extern int g_currentPlayer;
extern FriendSystem g_friendSystem;
extern PlayerDatabase g_playerDb;

AppState showFriendSystemScreen(RenderWindow &window, const Theme &theme)
{
    // Always refresh hash table when entering friend system
    g_friendSystem.buildHashTable();

    int selectedIndex = 0;

    const int optionCount = 4;
    const char *options[optionCount] = {
        "View Friends",
        "View Friend Requests",
        "Send Friend Request",
        "Back to Menu"};

    string statusMessage = "";

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                return AppState::EXIT_APP;

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                    selectedIndex = (selectedIndex - 1 + optionCount) % optionCount;

                else if (event.key.code == Keyboard::Down)
                    selectedIndex = (selectedIndex + 1) % optionCount;

                else if (event.key.code == Keyboard::Enter)
                {
                    if (selectedIndex == 0) // View friends
                        return AppState::VIEW_FRIENDS;

                    if (selectedIndex == 1) // View friend requests
                        return AppState::VIEW_FRIEND_REQUESTS;

                    if (selectedIndex == 2) // Send friend request
                        return AppState::SEND_FRIEND_REQUEST;

                    if (selectedIndex == 3) // Back
                        return AppState::PLAYER_MENU;
                }

                else if (event.key.code == Keyboard::Escape)
                    return AppState::PLAYER_MENU;
            }
        }

        window.clear(theme.backgroundColor);

        Text title("Friend System", theme.font, 38);
        title.setFillColor(theme.accentColor);
        title.setPosition(200, 40);
        window.draw(title);

        for (int i = 0; i < optionCount; i++)
        {
            Text opt(options[i], theme.font, 24);
            opt.setFillColor(i == selectedIndex ? theme.highlightColor : theme.textColor);
            opt.setPosition(200, 150 + i * 40);
            window.draw(opt);
        }

        window.display();
    }

    return AppState::EXIT_APP;
}

AppState showFriendRequestsScreen(RenderWindow &window, const Theme &theme)
{
    // Make sure hash table is up to date
    g_friendSystem.buildHashTable();

    string pending[50];
    int count = 0;

    g_friendSystem.getRequestUsernames(g_currentPlayer, pending, count);

    int selected = 0;
    string status = "Use Up/Down. ENTER = Accept, BACKSPACE = Reject, ESC = Back";

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                return AppState::EXIT_APP;

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                    return AppState::FRIEND_SYSTEM;

                if (count > 0)
                {
                    if (event.key.code == Keyboard::Up)
                    {
                        selected = (selected - 1 + count) % count;
                    }
                    else if (event.key.code == Keyboard::Down)
                    {
                        selected = (selected + 1) % count;
                    }
                    else if (event.key.code == Keyboard::Enter)
                    {
                        // ACCEPT
                        string msg;
                        g_friendSystem.acceptFriendRequest(
                            g_currentPlayer,
                            pending[selected],
                            msg);

                        status = msg;

                        // Reload list after change
                        g_friendSystem.getRequestUsernames(g_currentPlayer, pending, count);
                        if (selected >= count)
                            selected = count - 1;
                        if (selected < 0)
                            selected = 0;
                    }
                    else if (event.key.code == Keyboard::BackSpace)
                    {
                        // REJECT
                        string msg;
                        g_friendSystem.rejectFriendRequest(
                            g_currentPlayer,
                            pending[selected],
                            msg);

                        status = msg;

                        // Reload list
                        g_friendSystem.getRequestUsernames(g_currentPlayer, pending, count);
                        if (selected >= count)
                            selected = count - 1;
                        if (selected < 0)
                            selected = 0;
                    }
                }
            }
        }

        // ---------- DRAW UI ----------
        window.clear(theme.backgroundColor);

        // Title
        Text title("Pending Friend Requests", theme.font, 34);
        title.setFillColor(theme.accentColor);
        title.setPosition(120.f, 50.f);
        window.draw(title);

        // List of pending requests
        if (count == 0)
        {
            Text none("No pending friend requests.", theme.font, 24);
            none.setFillColor(theme.textColor);
            none.setPosition(140.f, 180.f);
            window.draw(none);
        }
        else
        {
            for (int i = 0; i < count; i++)
            {
                Text t(pending[i], theme.font, 26);
                t.setFillColor(i == selected ? theme.highlightColor : theme.textColor);
                t.setPosition(160.f, 160.f + i * 40.f);
                window.draw(t);
            }

            // Small label near the selected one (optional)
            Text selectedLabel("Selected: " + pending[selected], theme.font, 20);
            selectedLabel.setFillColor(theme.textColor);
            selectedLabel.setPosition(160.f, 160.f + count * 40.f + 20.f);
            window.draw(selectedLabel);
        }

        // Status / instructions
        Text help(status, theme.font, 20);
        help.setFillColor(theme.textColor);
        help.setPosition(40.f, 520.f);
        window.draw(help);

        window.display();
    }

    return AppState::EXIT_APP;
}

AppState showSendFriendRequestScreen(RenderWindow &window, const Theme &theme)
{
    string input = "";
    string status = "Type username and press ENTER. ESC = Back";

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                return AppState::EXIT_APP;

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                    return AppState::FRIEND_SYSTEM;

                if (event.key.code == Keyboard::Enter)
                {
                    if (input.empty())
                    {
                        status = "Username cannot be empty.";
                    }
                    else
                    {
                        string msg;
                        g_friendSystem.buildHashTable(); // ensure hash is fresh
                        g_friendSystem.sendFriendRequest(g_currentPlayer, input, msg);
                        status = msg;
                    }
                }
            }

            if (event.type == Event::TextEntered)
            {
                char c = static_cast<char>(event.text.unicode);

                if (c == 8) // Backspace
                {
                    if (!input.empty())
                        input.pop_back();
                }
                else if (c >= 32 && c <= 126) // Printable ASCII
                {
                    if (input.size() < 20)
                        input.push_back(c);
                }
            }
        }

        // ---------- DRAW UI ----------
        window.clear(theme.backgroundColor);

        Text title("Send Friend Request", theme.font, 34);
        title.setFillColor(theme.accentColor);
        title.setPosition(150.f, 50.f);
        window.draw(title);

        Text label("Enter username:", theme.font, 24);
        label.setFillColor(theme.textColor);
        label.setPosition(150.f, 180.f);
        window.draw(label);

        // "Input box"
        Text inputText(input, theme.font, 28);
        inputText.setFillColor(Color::White);
        inputText.setPosition(150.f, 225.f);
        window.draw(inputText);

        // Underline to make it look like a field
        Rectangle underline;
        underline.setSize(Vec2(300.f, 2.f));
        underline.setFillColor(theme.textColor);
        underline.setPosition(150.f, 260.f);
        window.draw(underline);

        // Status / instructions
        Text info(status, theme.font, 20);
        info.setFillColor(theme.textColor);
        info.setPosition(40.f, 520.f);
        window.draw(info);

        window.display();
    }

    return AppState::EXIT_APP;
}

AppState showViewFriendsScreen(RenderWindow &window, const Theme &theme)
{
    if (g_currentPlayer < 0)
        return AppState::FRIEND_SYSTEM;

    string friendsList[50];
    int count = 0;

    g_friendSystem.getFriendUsernames(g_currentPlayer, friendsList, count);

    int selected = 0;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                return AppState::EXIT_APP;

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                    return AppState::FRIEND_SYSTEM;

                if (event.key.code == Keyboard::Up && count > 0)
                    selected = (selected - 1 + count) % count;

                if (event.key.code == Keyboard::Down && count > 0)
                    selected = (selected + 1) % count;

                if (event.key.code == Keyboard::Enter)
                {
                    // Maybe show profile later – for now just return
                    return AppState::FRIEND_SYSTEM;
                }
            }
        }

        window.clear(theme.backgroundColor);

        Text title("Your Friends", theme.font, 34);
        title.setFillColor(theme.accentColor);
        title.setPosition(180, 50);
        window.draw(title);

        if (count == 0)
        {
            Text msg("You have no friends added yet.", theme.font, 24);
            msg.setFillColor(theme.textColor);
            msg.setPosition(160, 180);
            window.draw(msg);
        }
        else
        {
            for (int i = 0; i < count; i++)
            {
                Text t(friendsList[i], theme.font, 26);
                t.setFillColor(i == selected ? theme.highlightColor : theme.textColor);
                t.setPosition(160, 150 + i * 40);
                window.draw(t);
            }
        }

        Text help("ESC = Back", theme.font, 20);
        help.setFillColor(theme.textColor);
        help.setPosition(40, 520);
        window.draw(help);

        window.display();
    }

    return AppState::EXIT_APP;
}

extern PlayerDatabase g_playerDb;
extern int g_currentPlayer;
extern FriendSystem g_friendSystem;

AppState showProfileScreen(RenderWindow &window, const Theme &theme)
{
    if (g_currentPlayer < 0)
        return AppState::PLAYER_MENU;

    // Get current player info
    const Player &p = g_playerDb.getPlayer(g_currentPlayer);

    // Compute friend count
    string friendsArray[50];
    int friendsCount = 0;
    g_friendSystem.getFriendUsernames(g_currentPlayer, friendsArray, friendsCount);

    // Pending requests count
    string reqArray[50];
    int reqCount = 0;
    g_friendSystem.getRequestUsernames(g_currentPlayer, reqArray, reqCount);

    string line1 = "Username: " + p.username;
    string line2 = "Email: " + p.email;
    string line3 = "Total Score: " + to_string(p.totalScore);
    string line4 = "Friends: " + to_string(friendsCount);
    string line5 = "Pending Friend Requests: " + to_string(reqCount);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                return AppState::EXIT_APP;

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape ||
                    event.key.code == Keyboard::Enter)
                {
                    return AppState::PLAYER_MENU;
                }
            }
        }

        // -------- DRAW UI --------
        window.clear(theme.backgroundColor);

        Text title("Player Profile", theme.font, 36);
        title.setFillColor(theme.accentColor);
        title.setPosition(200.f, 60.f);
        window.draw(title);

        Text t1(line1, theme.font, 24);
        t1.setFillColor(theme.textColor);
        t1.setPosition(160.f, 150.f);
        window.draw(t1);

        Text t2(line2, theme.font, 24);
        t2.setFillColor(theme.textColor);
        t2.setPosition(160.f, 190.f);
        window.draw(t2);

        Text t3(line3, theme.font, 24);
        t3.setFillColor(theme.textColor);
        t3.setPosition(160.f, 230.f);
        window.draw(t3);

        Text t4(line4, theme.font, 24);
        t4.setFillColor(theme.textColor);
        t4.setPosition(160.f, 270.f);
        window.draw(t4);

        Text t5(line5, theme.font, 24);
        t5.setFillColor(theme.textColor);
        t5.setPosition(160.f, 310.f);
        window.draw(t5);

        Text hint("Press ENTER or ESC to go back", theme.font, 20);
        hint.setFillColor(theme.textColor);
        hint.setPosition(160.f, 420.f);
        window.draw(hint);

        window.display();
    }

    return AppState::EXIT_APP;
}

AppState showLevelSelectScreen(RenderWindow &window, const Theme &theme)
{
    const int levelCount = 3;
    const char *levels[levelCount] = {"Easy", "Medium", "Hard"};

    int selected = 0;
    string info = "Use Up/Down. Enter = Select. ESC = Back.";

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                return AppState::EXIT_APP;

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Escape)
                    return AppState::PLAYER_MENU;

                if (e.key.code == Keyboard::Up)
                    selected = (selected - 1 + levelCount) % levelCount;

                if (e.key.code == Keyboard::Down)
                    selected = (selected + 1) % levelCount;

                if (e.key.code == Keyboard::Enter)
                {
                    // Store the selected level
                    g_selectedLevel = selected;
                    return AppState::SINGLE_PLAYER;
                }
            }
        }

        window.clear(theme.backgroundColor);

        Text title("Select Level", theme.font, 36);
        title.setFillColor(theme.accentColor);
        title.setPosition(200, 50);
        window.draw(title);

        for (int i = 0; i < levelCount; i++)
        {
            Text t(levels[i], theme.font, 28);
            t.setFillColor(i == selected ? theme.highlightColor : theme.textColor);
            t.setPosition(200, 150 + i * 50);
            window.draw(t);
        }

        Text msg(info, theme.font, 20);
        msg.setFillColor(theme.textColor);
        msg.setPosition(40, 520);
        window.draw(msg);

        window.display();
    }

    return AppState::EXIT_APP;
}

AppState showSettingsScreen(RenderWindow &window, const Theme &theme)
{
    // Settings stored using simple array or struct
    static int soundOn = 1;
    static int volume = 5;
    static int selected = 0;

    const int optionCount = 3;
    const char *options[optionCount] = {
        "Toggle Sound",
        "Volume",
        "Back"};

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                return AppState::EXIT_APP;

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Up)
                    selected = (selected - 1 + optionCount) % optionCount;

                if (e.key.code == Keyboard::Down)
                    selected = (selected + 1) % optionCount;

                if (e.key.code == Keyboard::Escape)
                    return AppState::PLAYER_MENU;

                if (e.key.code == Keyboard::Enter)
                {
                    if (selected == 0)
                        soundOn = !soundOn;
                    if (selected == 1)
                        volume = (volume + 1) % 11;
                    if (selected == 2)
                        return AppState::PLAYER_MENU;
                }
            }
        }

        window.clear(theme.backgroundColor);

        Text title("Settings", theme.font, 36);
        title.setFillColor(theme.accentColor);
        title.setPosition(220, 50);
        window.draw(title);

        for (int i = 0; i < optionCount; i++)
        {
            string text = options[i];

            if (i == 0)
                text += soundOn ? " : ON" : " : OFF";
            if (i == 1)
                text += " : " + to_string(volume);

            Text t(text, theme.font, 26);
            t.setFillColor(i == selected ? theme.highlightColor : theme.textColor);
            t.setPosition(180, 150 + i * 50);
            window.draw(t);
        }

        window.display();
    }

    return AppState::EXIT_APP;
}

AppState showInstructionsScreen(RenderWindow &window, const Theme &theme)
{
    const int lineCount = 12;

    const char *textLines[lineCount] = {
        "Xonix Instructions:",
        "",
        "Use arrow keys to move.",
        "Avoid enemies and walls.",
        "Fill area to gain score.",
        "Complete 75 percent to win.",
        "",
        "Enemies kill you if touched.",
        "You have 3 lives.",
        "",
        "Press ESC to return."};

    int scroll = 0;

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                return AppState::EXIT_APP;

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Escape)
                    return AppState::PLAYER_MENU;

                if (e.key.code == Keyboard::Up)
                    scroll = max(scroll - 1, 0);

                if (e.key.code == Keyboard::Down)
                    scroll = min(scroll + 1, lineCount - 8);
            }
        }

        window.clear(theme.backgroundColor);

        Text title("Instructions", theme.font, 36);
        title.setFillColor(theme.accentColor);
        title.setPosition(200, 40);
        window.draw(title);

        // Draw visible lines
        for (int i = 0; i < 8; i++)
        {
            int idx = scroll + i;

            if (idx < lineCount)
            {
                Text t(textLines[idx], theme.font, 24);
                t.setFillColor(theme.textColor);
                t.setPosition(100, 150 + i * 40);
                window.draw(t);
            }
        }

        window.display();
    }

    return AppState::EXIT_APP;
}



AppState showMatchHistoryScreen(RenderWindow &window, const Theme &theme)
{
    if (g_currentPlayer < 0)
        return AppState::PLAYER_MENU;

    // Non-const because we load history into it
    Player &p = g_playerDb.getPlayerRef(g_currentPlayer);
    p.history.loadFromFile(p.username);

    int totalMatches   = p.history.size();
    int scroll         = 0;
    const int pageSize = 8; // rows visible at once

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                return AppState::EXIT_APP;

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Escape)
                    return AppState::PLAYER_MENU;

                if (e.key.code == Keyboard::Up && scroll > 0)
                    scroll--;

                if (e.key.code == Keyboard::Down &&
                    scroll + pageSize < totalMatches)
                    scroll++;
            }
        }

        window.clear(theme.backgroundColor);

        // Title
        Text title("Match History", theme.font, 40);
        title.setFillColor(theme.accentColor);
        FloatRect tb = title.getLocalBounds();
        title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        title.setPosition(window.getSize().x / 2.f, 60.f);
        window.draw(title);

        // Hint
        Text hint("Up / Down to scroll, ESC to go back", theme.font, 20);
        hint.setFillColor(theme.textColor);
        FloatRect hb = hint.getLocalBounds();
        hint.setOrigin(hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
        hint.setPosition(window.getSize().x / 2.f, 110.f);
        window.draw(hint);

        if (totalMatches == 0)
        {
            Text noData("No matches played yet.", theme.font, 24);
            noData.setFillColor(theme.textColor);
            noData.setPosition(80.f, 180.f);
            window.draw(noData);
        }
        else
        {
            float baseY = 150.f;

            // Header row
            Text header;
            header.setFont(theme.font);
            header.setCharacterSize(22);
            header.setFillColor(theme.highlightColor);

            header.setString("Time");
            header.setPosition(60.f, baseY);
            window.draw(header);

            header.setString("Opponent");
            header.setPosition(260.f, baseY);
            window.draw(header);

            header.setString("Result");
            header.setPosition(440.f, baseY);
            window.draw(header);

            header.setString("Score");
            header.setPosition(560.f, baseY);
            window.draw(header);

            // Table rows
            for (int i = 0; i < pageSize; ++i)
            {
                int idx = scroll + i;
                if (idx >= totalMatches)
                    break;

                MatchRecord r = p.history.get(idx);
                float y = baseY + 30.f + i * 28.f;

                Text tTime, tOpp, tRes, tScore;

                tTime.setFont(theme.font);
                tOpp.setFont(theme.font);
                tRes.setFont(theme.font);
                tScore.setFont(theme.font);

                tTime.setCharacterSize(20);
                tOpp.setCharacterSize(20);
                tRes.setCharacterSize(20);
                tScore.setCharacterSize(20);

                tTime.setFillColor(theme.textColor);
                tOpp.setFillColor(theme.textColor);
                tRes.setFillColor(theme.textColor);
                tScore.setFillColor(theme.textColor);

                tTime.setString(r.timestamp);
                tOpp.setString(r.opponent);
                tRes.setString(r.result);
                tScore.setString(std::to_string(r.score));

                tTime.setPosition(60.f,  y);
                tOpp .setPosition(260.f, y);
                tRes .setPosition(440.f, y);
                tScore.setPosition(560.f, y);

                window.draw(tTime);
                window.draw(tOpp);
                window.draw(tRes);
                window.draw(tScore);
            }
        }

        window.display();
    }

    return AppState::EXIT_APP;


    
}

AppState showMultiplayerScreen(RenderWindow& window, const Theme& theme)
{
    // Just call the multiplayer game function and then return to player menu
    runMultiplayerGame(window, theme);
    return AppState::PLAYER_MENU;
}
