// SaveGame.h
#pragma once

#include <string>
#include "Config.h"
#include "Grid.h"

// Linked list node required by assignment
// Represents one non-water tile in the grid
struct TileNode
{
    int row;        // 0..M-1
    int col;        // 0..N-1
    int value;      // original grid value (1, 2, 3, ...)
    TileNode* next;
};

// Unified GameState for both Single-player and Multiplayer
struct GameState
{
    // Meta
    std::string saveId;      // unique save identifier
    std::string playerId;    // username (or "Guest")
    std::string timestamp;   // human readable
    bool isMultiplayer;      // false = SP, true = MP
    int level;               // 0 easy, 1 medium, 2 hard

    // Single-player fields (also used for P1 in MP if needed)
    int playerX;
    int playerY;
    int dir;                 // 0 right, 1 down, 2 left, 3 up
    bool isDrawing;          // for SP; in MP safe resume we can ignore

    int score;
    int lives;
    int totalLandTiles;
    int targetLandTiles;
    int currentStrokeTiles;

    // Multiplayer: second player and turn info (for safe-state resume)
    int p2X;
    int p2Y;
    int p2Dir;
    int p2Score;
    int p2Lives;
    int activePlayer;        // 1 or 2

    // Power-up stacks
    int powerUpTop;
    int powerUpStack[10];

    int p2PowerUpTop;
    int p2PowerUpStack[10];

    // Enemies (used in both SP and MP as hazards)
    int enemyCount;
    int ex[10];
    int ey[10];
    float evx[10];
    float evy[10];

    // Linked list of all non-water tiles
    TileNode* tilesHead;

    GameState();
    ~GameState();
    void clear();
};

// Manages saving/loading to "saves/<saveId>.txt"
class SaveGameManager
{
public:
    SaveGameManager();

    bool saveGameState(const GameState& state) const;
    bool loadGameState(const std::string& saveId, GameState& outState) const;
};

// Helpers to convert between global grid and linked list representation
void buildGameStateFromGrid(GameState& state, const int grid[M][N]);
void applyGameStateToGrid(const GameState& state, int grid[M][N]);

// Helper to get current timestamp as string
std::string getCurrentTimestamp();
