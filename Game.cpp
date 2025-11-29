// Game.cpp
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

#include "sfml.h"
#include "Game.h"
#include "Starfield.h"
#include "Enemy.h"
#include "Config.h"
#include "Grid.h"
#include "Theme.h"
#include "PowerUpStack.h"
#include "PlayerDatabase.h"
#include "MatchHistory.h"

// Main grid definition
int grid[M][N] = {0};

// Level selection from Screens.cpp
extern int g_selectedLevel;

// Flood fill for captured area logic
static void floodFill(int gy, int gx)
{
    if (gy < 0 || gy >= M || gx < 0 || gx >= N)
        return;

    if (grid[gy][gx] != 0)
        return;

    grid[gy][gx] = -1;

    floodFill(gy - 1, gx);
    floodFill(gy + 1, gx);
    floodFill(gy, gx - 1);
    floodFill(gy, gx + 1);
}

// -------------------------------------------
//              MAIN GAME FUNCTION
// -------------------------------------------
int runSinglePlayerGame(RenderWindow &window, const Theme &theme)
{
    srand(static_cast<unsigned int>(time(nullptr)));

    Texture tilesTex, gameOverTex, enemyTex;
    tilesTex.loadFromFile("images/tiles.png");
    gameOverTex.loadFromFile("images/gameover.png");
    enemyTex.loadFromFile("images/enemy.png");

    Sprite tile(tilesTex);
    Sprite gameOverSprite(gameOverTex);
    Sprite enemySprite(enemyTex);
    enemySprite.setOrigin(20.f, 20.f);

    gameOverSprite.setPosition(100.f, 100.f);

    Starfield starfield(N * ts, M * ts);

    // -------------------------------------------
    //           DIFFICULTY SETTINGS
    // -------------------------------------------
    float delay = 0.07f; // constant player speed
    int enemyCount;

    if (g_selectedLevel == 0)
        enemyCount = 2; // Easy
    else if (g_selectedLevel == 1)
        enemyCount = 4; // Medium
    else
        enemyCount = 6; // Hard

    Enemy enemies[10]; // use only enemyCount

    // -------------------------------------------
    //         BUILD INITIAL PLAYFIELD
    // -------------------------------------------
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;

    // Player starts on top border, safe
    int playerX = N / 2;
    int playerY = 0;
    int dirX = 0, dirY = 0;

    bool gameRunning = true;
    bool exitGame = false;

    int finalScore = 0;

    Clock clock;
    float timer = 0;

    // -------------------------------------------
    //           POWER UP STACK
    // -------------------------------------------
    PowerUpStack powerUps;
    int nextPowerUpAt = 50; // first power up at 50, then every +30

    // -------------------------------------------
    //      MULTIPLIER SYSTEM VARIABLES
    // -------------------------------------------
    int tilesCapturedThisStroke = 0;
    int captureStreak = 0;
    int multiplier = 1;

    // -------------------------------------------
    //           SCORE TEXT
    // -------------------------------------------
    Text scoreText;
    scoreText.setFont(theme.font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(theme.textColor);
    scoreText.setPosition(10.f, 5.f);

    Text powerUpText;
    powerUpText.setFont(theme.font);
    powerUpText.setCharacterSize(22);
    powerUpText.setFillColor(theme.highlightColor);
    powerUpText.setPosition(10.f, 35.f);

    // Freeze control
    bool enemiesFrozen = false;
    float freezeTimer = 0.f;

    while (window.isOpen())
    {
        float dt = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += dt;

        // -------------------------------------------
        //               EVENT HANDLING
        // -------------------------------------------
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                // Close window and leave game loop
                window.close();
                exitGame = true;
            }

            if (event.type == Event::KeyPressed)
            {
                // Exit game with Esc or Enter
                if (event.key.code == Keyboard::Escape ||
                    event.key.code == Keyboard::Enter)
                {
                    exitGame = true;
                }

                // Space activates power up
                if (event.key.code == Keyboard::Space && !powerUps.isEmpty())
                {
                    string p = powerUps.pop();
                    if (p == "FREEZE")
                    {
                        enemiesFrozen = true;
                        freezeTimer = 3.f; // freeze for 3 seconds
                    }
                }
            }
        }

        if (exitGame)
            break;

        // -------------------------------------------
        //          MOVEMENT INPUT
        // -------------------------------------------
        bool anyKey = false;

        bool onLand = (grid[playerY][playerX] == 1); // captured land / border

        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            dirX = -1;
            dirY = 0;
            anyKey = true;
        }
        else if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            dirX = 1;
            dirY = 0;
            anyKey = true;
        }
        else if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            dirX = 0;
            dirY = -1;
            anyKey = true;
        }
        else if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            dirX = 0;
            dirY = 1;
            anyKey = true;
        }

        if (onLand)
        {
            // On land: if no key held, stop player
            if (!anyKey)
            {
                dirX = 0;
                dirY = 0;
            }
        }
        else
        {
            // In water / trail: player keeps going until direction is changed
        }

        // -------------------------------------------
        //            ENEMY FREEZE TIMER
        // -------------------------------------------
        if (enemiesFrozen)
        {
            freezeTimer -= dt;
            if (freezeTimer <= 0)
            {
                enemiesFrozen = false;
            }
        }

        // -------------------------------------------
        //             STARFIELD UPDATE
        // -------------------------------------------
        starfield.update(dt);

        // -------------------------------------------
        //             MOVE PLAYER
        // -------------------------------------------
        if (gameRunning && timer > delay && (dirX != 0 || dirY != 0))
        {
            playerX += dirX;
            playerY += dirY;

            if (playerX < 0)
                playerX = 0;
            if (playerX > N - 1)
                playerX = N - 1;
            if (playerY < 0)
                playerY = 0;
            if (playerY > M - 1)
                playerY = M - 1;

            if (grid[playerY][playerX] == 2)
                gameRunning = false;

            if (grid[playerY][playerX] == 0)
            {
                grid[playerY][playerX] = 2;
                tilesCapturedThisStroke++;
            }

            timer = 0.f;
        }

        // -------------------------------------------
        //           MOVE ENEMIES
        // -------------------------------------------
        if (!enemiesFrozen)
        {
            for (int i = 0; i < enemyCount; i++)
                enemies[i].move();
        }

        // -------------------------------------------
        //       PLAYER RETURNS TO LAND = CAPTURE
        // -------------------------------------------
        if (gameRunning && grid[playerY][playerX] == 1)
        {
            if (tilesCapturedThisStroke > 0)
            {
                if (tilesCapturedThisStroke > 10)
                {
                    captureStreak++;

                    if (captureStreak >= 5)
                        multiplier = 4;
                    else if (captureStreak >= 3)
                        multiplier = 2;
                    else
                        multiplier = 1;
                }

                // Flood fill using enemies as origins
                for (int i = 0; i < enemyCount; i++)
                {
                    int gy = enemies[i].y / ts;
                    int gx = enemies[i].x / ts;
                    floodFill(gy, gx);
                }

                // Capturing tiles
                for (int i = 0; i < M; i++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        if (grid[i][j] == -1)
                        {
                            grid[i][j] = 0;
                        }
                        else
                        {
                            if (grid[i][j] == 0)
                            {
                                finalScore += (1 * multiplier);
                            }
                            grid[i][j] = 1;
                        }
                    }
                }

                tilesCapturedThisStroke = 0;

                if (finalScore >= nextPowerUpAt)
                {
                    powerUps.push("FREEZE");
                    nextPowerUpAt += 30;
                }
            }

            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++)
                    if (grid[i][j] == 2)
                        grid[i][j] = 1;
        }

        // -------------------------------------------
        //        ENEMY TOUCHES TRAIL
        // -------------------------------------------
        for (int i = 0; i < enemyCount; i++)
        {
            int gy = enemies[i].y / ts;
            int gx = enemies[i].x / ts;
            if (grid[gy][gx] == 2)
                gameRunning = false;
        }

        // -------------------------------------------
        //                DRAWING
        // -------------------------------------------
        window.clear(theme.backgroundColor);
        starfield.draw(window, theme.starColor);

        // Tiles
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0)
                    continue;
                if (grid[i][j] == 1)
                    tile.setTextureRect(IntRect(0, 0, ts, ts));
                if (grid[i][j] == 2)
                    tile.setTextureRect(IntRect(54, 0, ts, ts));

                tile.setPosition(j * ts, i * ts);
                window.draw(tile);
            }
        }

        // Player
        tile.setTextureRect(IntRect(36, 0, ts, ts));
        tile.setPosition(playerX * ts, playerY * ts);
        window.draw(tile);

        // Enemies
        enemySprite.rotate(10.f);
        for (int i = 0; i < enemyCount; i++)
        {
            enemySprite.setPosition(enemies[i].x, enemies[i].y);
            window.draw(enemySprite);
        }

        // HUD
        Rectangle hud;
        hud.setSize(Vec2(200.f, 60.f));
        hud.setFillColor(theme.hudBackgroundColor);
        hud.setPosition(5.f, 0.f);
        hud.setOutlineThickness(1.5f);
        hud.setOutlineColor(theme.highlightColor);
        window.draw(hud);

        scoreText.setString("Score: " + to_string(finalScore));
        window.draw(scoreText);

        powerUpText.setString("Power Ups: " + to_string(powerUps.size()));
        window.draw(powerUpText);

        if (enemiesFrozen)
        {
            Text frz;
            frz.setFont(theme.font);
            frz.setCharacterSize(30);
            frz.setFillColor(Color::Cyan);
            frz.setString("FREEZE ACTIVE");
            frz.setPosition(350, 10);
            window.draw(frz);
        }

        if (!gameRunning)
        {
            Rectangle overlay;
            overlay.setSize(Vec2(window.getSize().x, window.getSize().y));
            overlay.setFillColor(Color(0, 0, 0, 150));
            window.draw(overlay);

            window.draw(gameOverSprite);

            Text msg;
            msg.setFont(theme.font);
            msg.setCharacterSize(24);
            msg.setFillColor(theme.textColor);
            msg.setString("Press Enter or Esc to return");

            FloatRect mb = msg.getLocalBounds();
            msg.setOrigin(mb.left + mb.width / 2, mb.top + mb.height / 2);
            msg.setPosition(window.getSize().x / 2, window.getSize().y - 60);

            window.draw(msg);
        }

        window.display();
    }

    // -------------------------------------------
    //       RECORD MATCH INTO HISTORY
    // -------------------------------------------
    extern PlayerDatabase g_playerDb;
    extern int g_currentPlayer;

    if (g_currentPlayer >= 0)
    {
        Player &p = g_playerDb.getPlayerRef(g_currentPlayer);

        MatchRecord r;
        r.opponent = "AI";

        // Simple rule: any positive score = WIN, 0 / negative = LOSS
        r.result = (finalScore > 0) ? "WIN" : "LOSS";
        r.score = finalScore;

        // Pretty timestamp like "2025-11-29 13:45"
        time_t now = time(nullptr);
        tm *lt = localtime(&now);
        char buf[20];
        if (lt && strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", lt))
            r.timestamp = buf;
        else
            r.timestamp = "unknown";

        p.history.enqueue(r);
        p.history.saveToFile(p.username);
    }

    return finalScore;
}

// ----------------- MULTIPLAYER (TURN BY TURN) -----------------
int runMultiplayerGame(RenderWindow &window, const Theme &theme)
{
    // We reuse the global grid[M][N]
    extern int grid[M][N];

    // Difficulty: same rules as single-player (enemy count from level)
    extern int g_selectedLevel;

    int enemyCount;
    if (g_selectedLevel == 0)
        enemyCount = 2; // Easy
    else if (g_selectedLevel == 1)
        enemyCount = 4; // Medium
    else
        enemyCount = 6; // Hard

    // Get player name for P1 from database
    extern PlayerDatabase g_playerDb;
    extern int g_currentPlayer;

    string p1Name = "P1";
    if (g_currentPlayer >= 0)
    {
        const Player &p = g_playerDb.getPlayer(g_currentPlayer);
        p1Name = p.nickname.empty() ? p.username : p.nickname;
    }
    string p2Name = "P2";

    // Intro screen with explanation
    auto showIntro = [&](const string &title) -> bool
    {
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    window.close();
                    return false;
                }
                if (event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Escape)
                        return false;
                    if (event.key.code == Keyboard::Enter ||
                        event.key.code == Keyboard::Space)
                        return true;
                }
            }

            window.clear(theme.backgroundColor);

            Text tTitle(title, theme.font, 32);
            tTitle.setFillColor(theme.highlightColor);
            FloatRect tb = tTitle.getLocalBounds();
            tTitle.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
            tTitle.setPosition(N * ts / 2.f, 70.f);
            window.draw(tTitle);

            Text l1("Two players share one board.", theme.font, 22);
            l1.setFillColor(theme.textColor);
            l1.setPosition(40.f, 130.f);
            window.draw(l1);

            Text l2("P1: Arrows   P2: Q/A/X/G  (Q-Up, A-Left, X-Down, G-Right)", theme.font, 20);
            l2.setFillColor(theme.textColor);
            l2.setPosition(40.f, 170.f);
            window.draw(l2);

            Text l3("If one hits the other's trail, the one who hits dies.", theme.font, 20);
            l3.setFillColor(theme.textColor);
            l3.setPosition(40.f, 210.f);
            window.draw(l3);

            Text l4("If they collide while both drawing, both die.", theme.font, 20);
            l4.setFillColor(theme.textColor);
            l4.setPosition(40.f, 240.f);
            window.draw(l4);

            Text l5("Power-up: FREEZE stops enemies + opponent for 3 seconds.", theme.font, 20);
            l5.setFillColor(theme.textColor);
            l5.setPosition(40.f, 270.f);
            window.draw(l5);

            Text l6("Press Enter to start, Esc to go back.", theme.font, 20);
            l6.setFillColor(theme.textColor);
            l6.setPosition(40.f, 320.f);
            window.draw(l6);

            window.display();
        }
        return false;
    };

    if (!showIntro("Multiplayer Mode"))
        return 0;

    // ================== INITIALIZE BOARD ==================
    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                grid[i][j] = 1;
            else
                grid[i][j] = 0;
        }
    }

    // trailOwner: 0 = none, 1 = P1, 2 = P2
    int trailOwner[M][N];
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            trailOwner[i][j] = 0;

    // ================== ENEMIES ==================
    Enemy enemies[10];
    Starfield starfield(N * ts, M * ts);
    Texture tilesTex, enemyTex, gameOverTex;
    tilesTex.loadFromFile("images/tiles.png");
    enemyTex.loadFromFile("images/enemy.png");
    gameOverTex.loadFromFile("images/gameover.png");

    Sprite tile(tilesTex);
    Sprite enemySprite(enemyTex);
    enemySprite.setOrigin(20.f, 20.f);

    Sprite gameOverSprite(gameOverTex);
    gameOverSprite.setPosition(100.f, 100.f);

    // ================== PLAYER STATE ==================
    struct PlayerState
    {
        int x, y;
        int dirX, dirY;
        bool alive;
        bool isDrawing;
        int score;
        PowerUpStack powerUps;
    };

    PlayerState p1, p2;
    p1.x = N / 3;
    p1.y = 0;
    p2.x = 2 * N / 3;
    p2.y = 0;

    p1.dirX = p1.dirY = 0;
    p2.dirX = p2.dirY = 0;

    p1.alive = p2.alive = true;
    p1.isDrawing = p2.isDrawing = false;
    p1.score = p2.score = 0;

    int p1NextPU = 50, p2NextPU = 50;

    // ================== FREEZE LOGIC ==================
    bool freezeActive = false;
    int frozenBy = 0; // 1 = P1, 2 = P2
    float freezeTimer = 0.f;

    // Shared match timer
    float matchTime = 0.f;

    // HUD texts
    Text p1Text;
    Text p2Text;
    Text timerText;

    p1Text.setFont(theme.font);
    p2Text.setFont(theme.font);
    timerText.setFont(theme.font);

    p1Text.setCharacterSize(20);
    p2Text.setCharacterSize(20);
    timerText.setCharacterSize(20);

    p1Text.setFillColor(Color::Green);
    p2Text.setFillColor(Color::Red);
    timerText.setFillColor(theme.textColor);

    p1Text.setFillColor(Color::Green);
    p2Text.setFillColor(Color::Red);
    timerText.setFillColor(theme.textColor);

    // Game loop
    bool running = true;
    Clock clock;
    float tick = 0.f;
    const float delay = 0.07f;

    // helper to check "constructing" state
    auto isConstructing = [&](const PlayerState &p) -> bool
    {
        int gy = p.y;
        int gx = p.x;
        if (gy < 0 || gy >= M || gx < 0 || gx >= N)
            return false;
        // in water or on its own trail
        return (grid[gy][gx] == 0 || (grid[gy][gx] == 2));
    };

    // floodFill helper (uses global grid)
    auto ff = [&](int sy, int sx)
    {
        floodFill(sy, sx);
    };

    // area capture for a given player
    auto captureAreaFor = [&](int playerId, PlayerState &player)
    {
        // Snapshot old grid
        int oldGrid[M][N];
        for (int i = 0; i < M; ++i)
            for (int j = 0; j < N; ++j)
                oldGrid[i][j] = grid[i][j];

        // Flood-fill from enemies
        for (int i = 0; i < enemyCount; ++i)
        {
            int gy = enemies[i].y / ts;
            int gx = enemies[i].x / ts;
            ff(gy, gx);
        }

        int gained = 0;

        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (grid[i][j] == -1)
                {
                    grid[i][j] = 0; // reachable water stays water
                }
                else
                {
                    // 0 = enclosed water, 1 = land, 2 = trails
                    if (grid[i][j] == 0)
                    {
                        grid[i][j] = 1;
                        if (oldGrid[i][j] != 1)
                            gained++;
                    }
                }
            }
        }

        // convert all trails to land (both players)
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (grid[i][j] == 2)
                {
                    grid[i][j] = 1;
                    if (oldGrid[i][j] != 1)
                        gained++;
                    trailOwner[i][j] = 0;
                }
            }
        }

        if (gained > 0)
            player.score += gained;
    };

    while (running && window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        tick += dt;
        matchTime += dt;

        if (freezeActive)
        {
            freezeTimer -= dt;
            if (freezeTimer <= 0.f)
            {
                freezeActive = false;
                frozenBy = 0;
            }
        }

        // ------------------ events ------------------
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return 0;
            }
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                {
                    running = false;
                }

                // Power-up keys
                if (event.key.code == Keyboard::Space && p1.alive)
                {
                    if (!p1.powerUps.isEmpty())
                    {
                        string pu = p1.powerUps.pop();
                        if (pu == "FREEZE")
                        {
                            freezeActive = true;
                            frozenBy = 1;
                            freezeTimer = 3.f;
                        }
                    }
                }
                if (event.key.code == Keyboard::R && p2.alive)
                {
                    if (!p2.powerUps.isEmpty())
                    {
                        string pu = p2.powerUps.pop();
                        if (pu == "FREEZE")
                        {
                            freezeActive = true;
                            frozenBy = 2;
                            freezeTimer = 3.f;
                        }
                    }
                }
            }
        }

        if (!running)
            break;

        // ------------------ INPUT: DIRECTIONS ------------------
        bool p1CanMove = (!freezeActive || frozenBy == 1);
        bool p2CanMove = (!freezeActive || frozenBy == 2);

        // P1: arrows
        if (p1.alive && p1CanMove)
        {
            bool any = false;
            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                p1.dirX = -1;
                p1.dirY = 0;
                any = true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                p1.dirX = 1;
                p1.dirY = 0;
                any = true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                p1.dirX = 0;
                p1.dirY = -1;
                any = true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                p1.dirX = 0;
                p1.dirY = 1;
                any = true;
            }

            bool onLand = (grid[p1.y][p1.x] == 1);
            if (onLand && !any)
            {
                p1.dirX = 0;
                p1.dirY = 0;
            }
        }

        // P2: Q/A/X/G  (Q-Up, A-Left, X-Down, G-Right)
        if (p2.alive && p2CanMove)
        {
            bool any = false;
            if (Keyboard::isKeyPressed(Keyboard::A))
            {
                p2.dirX = -1;
                p2.dirY = 0;
                any = true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::G))
            {
                p2.dirX = 1;
                p2.dirY = 0;
                any = true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Q))
            {
                p2.dirX = 0;
                p2.dirY = -1;
                any = true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::X))
            {
                p2.dirX = 0;
                p2.dirY = 1;
                any = true;
            }

            bool onLand2 = (grid[p2.y][p2.x] == 1);
            if (onLand2 && !any)
            {
                p2.dirX = 0;
                p2.dirY = 0;
            }
        }

        // ------------------ MOVE PLAYERS ------------------
        if (tick > delay)
        {
            auto stepPlayer = [&](int id, PlayerState &p)
            {
                if (!p.alive)
                    return;

                if (p.dirX == 0 && p.dirY == 0)
                    return;

                int nx = p.x + p.dirX;
                int ny = p.y + p.dirY;

                if (nx < 0)
                    nx = 0;
                if (nx > N - 1)
                    nx = N - 1;
                if (ny < 0)
                    ny = 0;
                if (ny > M - 1)
                    ny = M - 1;

                // Start drawing if leaving land into water
                if (grid[p.y][p.x] == 1 && grid[ny][nx] == 0)
                    p.isDrawing = true;

                p.x = nx;
                p.y = ny;

                if (p.isDrawing && grid[p.y][p.x] == 0)
                {
                    grid[p.y][p.x] = 2;
                    trailOwner[p.y][p.x] = id;
                }

                // If returns to land while drawing -> capture
                if (p.isDrawing && grid[p.y][p.x] == 1)
                {
                    captureAreaFor(id, p);
                    p.isDrawing = false;

                    // Power-up when score crosses thresholds
                    if (id == 1 && p.score >= p1NextPU)
                    {
                        p.powerUps.push("FREEZE");
                        p1NextPU += 40;
                    }
                    if (id == 2 && p.score >= p2NextPU)
                    {
                        p.powerUps.push("FREEZE");
                        p2NextPU += 40;
                    }
                }
            };

            stepPlayer(1, p1);
            stepPlayer(2, p2);

            tick = 0.f;
        }

        // ------------------ MOVE ENEMIES ------------------
        if (!freezeActive)
        {
            for (int i = 0; i < enemyCount; ++i)
                enemies[i].move();
        }

        // ------------------ COLLISIONS ------------------

        // Enemy vs players and trails
        for (int i = 0; i < enemyCount; ++i)
        {
            int gy = enemies[i].y / ts;
            int gx = enemies[i].x / ts;
            if (gy < 0 || gy >= M || gx < 0 || gx >= N)
                continue;

            // Enemy hits P1 trail
            if (grid[gy][gx] == 2 && trailOwner[gy][gx] == 1 && p1.alive)
                p1.alive = false;

            // Enemy hits P2 trail
            if (grid[gy][gx] == 2 && trailOwner[gy][gx] == 2 && p2.alive)
                p2.alive = false;

            // Enemy hits players while they are constructing
            if (p1.alive && gy == p1.y && gx == p1.x && isConstructing(p1))
                p1.alive = false;
            if (p2.alive && gy == p2.y && gx == p2.x && isConstructing(p2))
                p2.alive = false;
        }

        // Player vs player collisions
        bool p1Cons = p1.alive && isConstructing(p1);
        bool p2Cons = p2.alive && isConstructing(p2);

        if (p1.alive && p2.alive)
        {
            // Both constructing and collide
            if (p1Cons && p2Cons && p1.x == p2.x && p1.y == p2.y)
            {
                p1.alive = p2.alive = false;
            }

            // One constructing, other not, same cell
            else if (p1Cons && !p2Cons && p1.x == p2.x && p1.y == p2.y)
            {
                p1.alive = false;
            }
            else if (p2Cons && !p1Cons && p1.x == p2.x && p1.y == p2.y)
            {
                p2.alive = false;
            }
        }

        // Touching other player's trail
        if (p1.alive)
        {
            int gy = p1.y, gx = p1.x;
            if (gy >= 0 && gy < M && gx >= 0 && gx < N)
            {
                if (grid[gy][gx] == 2 && trailOwner[gy][gx] == 2)
                    p1.alive = false;
            }
        }
        if (p2.alive)
        {
            int gy = p2.y, gx = p2.x;
            if (gy >= 0 && gy < M && gx >= 0 && gx < N)
            {
                if (grid[gy][gx] == 2 && trailOwner[gy][gx] == 1)
                    p2.alive = false;
            }
        }

        if (!p1.alive && p1.isDrawing)
            p1.isDrawing = false;
        if (!p2.alive && p2.isDrawing)
            p2.isDrawing = false;

        // end condition
        if (!p1.alive && !p2.alive)
            running = false;

        // ------------------ DRAW ------------------
        starfield.update(dt);
        window.clear(theme.backgroundColor);
        starfield.draw(window, theme.starColor);

        // draw grid
        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (grid[i][j] == 0)
                    continue;

                if (grid[i][j] == 1)
                    tile.setTextureRect(IntRect(0, 0, ts, ts));
                else if (grid[i][j] == 2)
                    tile.setTextureRect(IntRect(54, 0, ts, ts));

                // Color trails differently
                if (grid[i][j] == 2)
                {
                    if (trailOwner[i][j] == 1)
                        tile.setColor(Color(255, 255, 0)); // yellow trail for P1
                    else if (trailOwner[i][j] == 2)
                        tile.setColor(Color(255, 100, 100)); // reddish trail for P2
                }
                else
                {
                    tile.setColor(Color::White);
                }

                tile.setPosition(j * ts, i * ts);
                window.draw(tile);
            }
        }

        // Draw players as tiles (same sprite, different colors)
        tile.setTextureRect(IntRect(36, 0, ts, ts));

        if (p1.alive)
        {
            tile.setColor(Color::Green);
            tile.setPosition(p1.x * ts, p1.y * ts);
            window.draw(tile);
        }

        if (p2.alive)
        {
            tile.setColor(Color::Red);
            tile.setPosition(p2.x * ts, p2.y * ts);
            window.draw(tile);
        }

        // Draw enemies
        enemySprite.rotate(8.f);
        for (int i = 0; i < enemyCount; ++i)
        {
            enemySprite.setPosition(enemies[i].x, enemies[i].y);
            window.draw(enemySprite);
        }

        // HUD bar across top
        // HUD bar across top
        Rectangle hudRect;
        hudRect.setSize(Vec2(static_cast<float>(N * ts), 60.f));
        hudRect.setPosition(0.f, 0.f);
        hudRect.setFillColor(theme.hudBackgroundColor);
        window.draw(hudRect);

        p1Text.setString("P1 " + p1Name +
                         " | Score: " + to_string(p1.score) +
                         " | PU: " + to_string(p1.powerUps.size()) +
                         (p1.alive ? "" : " [DEAD]"));

        p2Text.setString("P2 " + p2Name +
                         " | Score: " + to_string(p2.score) +
                         " | PU: " + to_string(p2.powerUps.size()) +
                         (p2.alive ? "" : " [DEAD]"));

        int sec = static_cast<int>(matchTime);
        int mm = sec / 60;
        int ss = sec % 60;
        char buf[16];
        sprintf(buf, "%02d:%02d", mm, ss);
        timerText.setString(buf);

        p1Text.setPosition(10.f, 5.f);
        p2Text.setPosition(window.getSize().x - 370.f, 5.f);
        timerText.setPosition(window.getSize().x / 2.f - 30.f, 35.f);

        window.draw(p1Text);
        window.draw(p2Text);
        window.draw(timerText);

        if (!running)
        {
            Rectangle overlay;
            overlay.setSize(Vec2(window.getSize().x, window.getSize().y));
            overlay.setPosition(0.f, 0.f);
            overlay.setFillColor(Color(0, 0, 0, 150));
            window.draw(overlay);
        }

        window.display();
    }

    // ============= RESULT SCREEN =============
    string result;
    if (p1.score > p2.score)
        result = p1Name + " wins!";
    else if (p2.score > p1.score)
        result = p2Name + " wins!";
    else
        result = "Draw!";

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return 0;
            }
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape ||
                    event.key.code == Keyboard::Enter ||
                    event.key.code == Keyboard::Space)
                    return 0;
            }
        }

        window.clear(theme.backgroundColor);

        Text t1("Multiplayer Result", theme.font, 32);
        t1.setFillColor(theme.highlightColor);
        FloatRect tb = t1.getLocalBounds();
        t1.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        t1.setPosition(N * ts / 2.f, 80.f);
        window.draw(t1);

        Text s1("P1 " + p1Name + " : " + to_string(p1.score), theme.font, 24);
        Text s2("P2 " + p2Name + " : " + to_string(p2.score), theme.font, 24);
        s1.setFillColor(Color::Green);
        s2.setFillColor(Color::Red);
        s1.setPosition(100.f, 150.f);
        s2.setPosition(100.f, 190.f);
        window.draw(s1);
        window.draw(s2);

        Text s3(result, theme.font, 26);
        s3.setFillColor(theme.textColor);
        s3.setPosition(100.f, 240.f);
        window.draw(s3);

        Text hint("Press Enter / Esc to return to menu", theme.font, 20);
        hint.setFillColor(theme.textColor);
        hint.setPosition(100.f, 290.f);
        window.draw(hint);

        window.display();
    }

    return 0;
}
