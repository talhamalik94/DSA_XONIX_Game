// Leaderboard.h
#pragma once

#include "PlayerDatabase.h"

// One leaderboard entry = reference to a player and their score
struct LeaderboardEntry
{
    int playerIndex; // index inside PlayerDatabase
    int score;       // cached totalScore
};

class Leaderboard
{
private:
    LeaderboardEntry heap[10]; // min-heap by score
    int heapSize;

    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    Leaderboard();

    void clear();

    // Build heap from all players in database
    void buildFromDatabase(const PlayerDatabase& db);

    // Insert a player following "top 10 using min-heap" rule
    void insert(int playerIndex, int score);

    // Copy heap into out[] sorted from highest to lowest score
    void toSortedArray(LeaderboardEntry out[], int& outSize) const;
};
