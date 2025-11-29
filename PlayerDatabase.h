// PlayerDatabase.h
#pragma once

#include <string>
#include "Player.h"

using std::string;

// Simple dynamic-array based player database
class PlayerDatabase
{
private:
    Player *players;
    int size;
    int capacity;
    string filename;

    void ensureCapacity();
    int findIndexByUsername(const string &username) const;

public:
    PlayerDatabase(const string &file);
    ~PlayerDatabase();

    void loadFromFile();
    void saveToFile() const;

    // Simple getters to read player data (for leaderboard etc.)
    int getSize() const;
    const Player &getPlayer(int index) const;

    // Register new player
    bool registerPlayer(const string &username,
                        const string &password,
                        const string &nickname,
                        const string &email,
                        string &errorMessage);

    // Authenticate login, return index or -1
    int authenticate(const string &username,
                     const string &password,
                     string &errorMessage) const;

    // Change password for existing user
    bool changePassword(const string &username,
                        const string &newPassword,
                        string &errorMessage);

    // Update total score for a player
    void updateScore(int index, int deltaScore);

    Player &getPlayerRef(int index);
};
