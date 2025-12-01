// SaveGame.cpp
#include "SaveGame.h"

#include <fstream>
#include <filesystem>
#include <ctime>
#include <sstream>

using namespace std;

// ----------------- Timestamp helper -----------------

std::string getCurrentTimestamp()
{
    time_t now = time(nullptr);
    tm* lt = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt);
    return std::string(buf);
}

// ----------------- GameState -----------------

GameState::GameState()
{
    saveId.clear();
    playerId.clear();
    timestamp.clear();
    isMultiplayer = false;
    level = 0;

    playerX = playerY = 0;
    dir = 0;
    isDrawing = false;

    score = 0;
    lives = 3;
    totalLandTiles = 0;
    targetLandTiles = 0;
    currentStrokeTiles = 0;

    p2X = p2Y = 0;
    p2Dir = 0;
    p2Score = 0;
    p2Lives = 0;
    activePlayer = 1;

    powerUpTop = -1;
    p2PowerUpTop = -1;
    for (int i = 0; i < 10; ++i)
    {
        powerUpStack[i] = 0;
        p2PowerUpStack[i] = 0;
    }

    enemyCount = 0;
    for (int i = 0; i < 10; ++i)
    {
        ex[i] = ey[i] = 0;
        evx[i] = evy[i] = 0.f;
    }

    tilesHead = nullptr;
}

GameState::~GameState()
{
    clear();
}

void GameState::clear()
{
    TileNode* cur = tilesHead;
    while (cur)
    {
        TileNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    tilesHead = nullptr;
}

// ----------------- SaveGameManager -----------------

SaveGameManager::SaveGameManager()
{
    // Ensure "saves" directory exists
    std::filesystem::create_directories("saves");
}

bool SaveGameManager::saveGameState(const GameState& state) const
{
    if (state.saveId.empty())
        return false;

    std::string filePath = "saves/" + state.saveId + ".txt";
    ofstream out(filePath.c_str());
    if (!out.is_open())
        return false;

    // 1) Meta
    out << state.saveId << "\n";
    out << state.playerId << "\n";
    out << state.timestamp << "\n";
    out << (state.isMultiplayer ? 1 : 0) << "\n";
    out << state.level << "\n";

    // 2) Primary player info
    out << state.playerX << " " << state.playerY << " "
        << state.dir << " " << (state.isDrawing ? 1 : 0) << "\n";

    out << state.score << " " << state.lives << " "
        << state.totalLandTiles << " " << state.targetLandTiles << " "
        << state.currentStrokeTiles << "\n";

    // 3) Second player info (multiplayer safe-state)
    out << state.p2X << " " << state.p2Y << " "
        << state.p2Dir << " " << state.p2Score << " "
        << state.p2Lives << " " << state.activePlayer << "\n";

    // 4) Power-up stacks
    out << state.powerUpTop << "\n";
    for (int i = 0; i < 10; ++i)
        out << state.powerUpStack[i] << (i == 9 ? '\n' : ' ');

    out << state.p2PowerUpTop << "\n";
    for (int i = 0; i < 10; ++i)
        out << state.p2PowerUpStack[i] << (i == 9 ? '\n' : ' ');

    // 5) Enemies
    out << state.enemyCount << "\n";
    for (int i = 0; i < state.enemyCount; ++i)
        out << state.ex[i] << (i + 1 == state.enemyCount ? '\n' : ' ');
    for (int i = 0; i < state.enemyCount; ++i)
        out << state.ey[i] << (i + 1 == state.enemyCount ? '\n' : ' ');
    for (int i = 0; i < state.enemyCount; ++i)
        out << state.evx[i] << (i + 1 == state.enemyCount ? '\n' : ' ');
    for (int i = 0; i < state.enemyCount; ++i)
        out << state.evy[i] << (i + 1 == state.enemyCount ? '\n' : ' ');

    // 6) Tiles linked list (non-water cells)
    int tileCount = 0;
    for (TileNode* n = state.tilesHead; n != nullptr; n = n->next)
        tileCount++;

    out << tileCount << "\n";
    for (TileNode* n = state.tilesHead; n != nullptr; n = n->next)
        out << n->row << " " << n->col << " " << n->value << "\n";

    return true;
}

bool SaveGameManager::loadGameState(const std::string& saveId, GameState& outState) const
{
    outState.clear();

    std::string filePath = "saves/" + saveId + ".txt";
    ifstream in(filePath.c_str());
    if (!in.is_open())
        return false;

    // 1) Meta
    std::getline(in, outState.saveId);
    if (!in.good()) return false;
    std::getline(in, outState.playerId);
    std::getline(in, outState.timestamp);

    int multiFlag = 0;
    in >> multiFlag;
    outState.isMultiplayer = (multiFlag != 0);

    in >> outState.level;

    // 2) Primary player
    int drawingFlag = 0;
    in >> outState.playerX
       >> outState.playerY
       >> outState.dir
       >> drawingFlag;
    outState.isDrawing = (drawingFlag != 0);

    in >> outState.score
       >> outState.lives
       >> outState.totalLandTiles
       >> outState.targetLandTiles
       >> outState.currentStrokeTiles;

    // 3) Second player
    in >> outState.p2X
       >> outState.p2Y
       >> outState.p2Dir
       >> outState.p2Score
       >> outState.p2Lives
       >> outState.activePlayer;

    // 4) Power-ups
    in >> outState.powerUpTop;
    for (int i = 0; i < 10; ++i)
        in >> outState.powerUpStack[i];

    in >> outState.p2PowerUpTop;
    for (int i = 0; i < 10; ++i)
        in >> outState.p2PowerUpStack[i];

    // 5) Enemies
    in >> outState.enemyCount;
    if (outState.enemyCount < 0) outState.enemyCount = 0;
    if (outState.enemyCount > 10) outState.enemyCount = 10;

    for (int i = 0; i < outState.enemyCount; ++i)
        in >> outState.ex[i];
    for (int i = 0; i < outState.enemyCount; ++i)
        in >> outState.ey[i];
    for (int i = 0; i < outState.enemyCount; ++i)
        in >> outState.evx[i];
    for (int i = 0; i < outState.enemyCount; ++i)
        in >> outState.evy[i];

    // 6) Tiles list
    int tileCount = 0;
    in >> tileCount;

    TileNode* head = nullptr;
    TileNode* tail = nullptr;

    for (int k = 0; k < tileCount; ++k)
    {
        int r, c, v;
        in >> r >> c >> v;

        TileNode* node = new TileNode;
        node->row = r;
        node->col = c;
        node->value = v;
        node->next = nullptr;

        if (!head)
        {
            head = tail = node;
        }
        else
        {
            tail->next = node;
            tail = node;
        }
    }

    outState.tilesHead = head;
    return true;
}

// ----------------- Grid helpers -----------------

void buildGameStateFromGrid(GameState& state, const int gridArr[M][N])
{
    // Clear existing list
    state.clear();

    TileNode* head = nullptr;
    TileNode* tail = nullptr;

    for (int r = 0; r < M; ++r)
    {
        for (int c = 0; c < N; ++c)
        {
            int v = gridArr[r][c];
            if (v != 0)
            {
                TileNode* node = new TileNode;
                node->row = r;
                node->col = c;
                node->value = v;
                node->next = nullptr;

                if (!head)
                {
                    head = tail = node;
                }
                else
                {
                    tail->next = node;
                    tail = node;
                }
            }
        }
    }

    state.tilesHead = head;
}

void applyGameStateToGrid(const GameState& state, int gridArr[M][N])
{
    // Reset grid to water
    for (int r = 0; r < M; ++r)
        for (int c = 0; c < N; ++c)
            gridArr[r][c] = 0;

    // Apply tiles: we also "clean up" any mid-trails
    for (TileNode* node = state.tilesHead; node != nullptr; node = node->next)
    {
        int r = node->row;
        int c = node->col;
        int v = node->value;

        if (r < 0 || r >= M || c < 0 || c >= N)
            continue;

        // Safe-state policy:
        // 2 (trail) becomes 1 (solid ground),
        // 4 (temp fill marker) is discarded,
        // others stay as they are.
        if (v == 2)
            v = 1;
        else if (v == 4)
            continue;

        gridArr[r][c] = v;
    }

    // Ensure borders remain solid ground
    for (int c = 0; c < N; ++c)
    {
        gridArr[0][c] = 1;
        gridArr[M - 1][c] = 1;
    }
    for (int r = 0; r < M; ++r)
    {
        gridArr[r][0] = 1;
        gridArr[r][N - 1] = 1;
    }

}
