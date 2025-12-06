// SaveGame.h
#pragma once

#include <string>
#include "Config.h"
#include "Grid.h"

struct TileNode
{
    int row;   
    int col;   
    int value; 
    TileNode *next;
};

struct GameState
{
    std::string saveId;    
    std::string playerId;  
    std::string timestamp; 
    bool isMultiplayer;    
    int level;             
    // Single-player fields (also used for P1 in MP if needed)
    int playerX;
    int playerY;
    int dir;        // 0 right, 1 down, 2 left, 3 up
    bool isDrawing; // for SP; in MP safe resume we can ignore

    int score;
    int lives;
    int totalLandTiles;
    int targetLandTiles;
    int currentStrokeTiles;

    // second player and turn info
    int p2X;
    int p2Y;
    int p2Dir;
    int p2Score;
    int p2Lives;
    int activePlayer; // 1 or 2

    // Power-up stacks
    int powerUpTop;
    int powerUpStack[10];

    int p2PowerUpTop;
    int p2PowerUpStack[10];

    int enemyCount;
    int ex[10];
    int ey[10];
    float evx[10];
    float evy[10];

    TileNode *tilesHead;

    GameState();
    ~GameState();
    void clear();
};

// Manages saving/loading to "saves/<saveId>.txt"
class SaveGameManager
{
public:
    SaveGameManager();

    bool saveGameState(const GameState &state) const;
    bool loadGameState(const std::string &saveId, GameState &outState) const;

    // New: load the most recent save for this player (by timestamp)
    bool loadLastSaveForPlayer(const std::string &playerId,
                               GameState &outState,
                               std::string &outSaveId) const;
};

// Helpers to convert between global grid and linked list representation
void buildGameStateFromGrid(GameState &state, const int grid[M][N]);
void applyGameStateToGrid(const GameState &state, int grid[M][N]);

// Helper to get current timestamp as string
std::string getCurrentTimestamp();
