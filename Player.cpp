// Player.cpp
#include "Player.h"

Player::Player()
{
    username = "";
    password = "";
    nickname = "";
    email    = "";
    totalScore = 0;

    score  = 0;
    wins   = 0;
    losses = 0;

    friendsHead  = nullptr;
    requestsHead = nullptr;
}
