// Matchmaking.h
#pragma once

#include "PlayerDatabase.h"

// A pair of players that will play a match
struct Match
{
    int player1;   // index in PlayerDatabase
    int player2;   // index in PlayerDatabase
};

// Simple max heap based priority queue.
// Priority is player's total score. Higher score = higher priority.
class MatchPriorityQueue
{
private:
    int heap[100];       // store player indices from PlayerDatabase
    int priorities[100]; // parallel array storing score of each player
    int size;

    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    MatchPriorityQueue();

    void clear();
    bool isEmpty() const;
    int getSize() const;

    // Insert player index with priority "score"
    void insert(int playerIndex, int score);

    // Remove and return index of player with highest score
    int extractMax();

    // Check if a player is already in the heap (simple linear search)
    bool contains(int playerIndex) const;
};

// Simple circular queue for game rooms
class GameRoomQueue
{
private:
    Match rooms[100];
    int front;
    int rear;
    int count;

public:
    GameRoomQueue();

    void clear();
    bool isEmpty() const;
    bool isFull() const;

    bool enqueue(const Match& m);
    bool dequeue(Match& m);
};

// Overall matchmaking system
class MatchmakingSystem
{
private:
    MatchPriorityQueue pq;
    GameRoomQueue roomQueue;

public:
    MatchmakingSystem();

    void clear();

    // Add a player to the matchmaking queue based on their total score.
    // Returns false if already in the queue.
    bool addPlayer(int playerIndex, int score);

    // Take players from priority queue and form game rooms in FIFO order.
    void createMatches();

    // Check if there is a match where this player is present.
    // If found, returns true and writes opponent index into opponentIndex.
    bool getMatchForPlayer(int playerIndex, int& opponentIndex);

    // For debugging you can later print queue, not needed for now.
};
