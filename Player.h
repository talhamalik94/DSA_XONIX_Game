// Player.h
#pragma once

#include <string>
#include "MatchHistory.h"
using std::string;

// Forward declarations for friend lists
struct FriendNode;
struct RequestNode;

// Simple player record
struct Player
{
    string username;
    string password;
    string nickname;
    string email;
    int totalScore;

    MatchHistory history;

    int score;
    int wins;
    int losses;

    FriendNode *friendsHead;
    RequestNode *requestsHead;

    Player();
};

struct FriendNode
{
    int friendIndex;
    FriendNode *next;
};

struct RequestNode
{
    int fromIndex;
    RequestNode *next;
};
