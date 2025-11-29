#pragma once

#include <string>
#include "PlayerDatabase.h"

using std::string;

// Hash table node for username -> index mapping
struct HashNode
{
    string username;
    int index;
    HashNode* next;
};

class FriendSystem
{
private:
    static const int TABLE_SIZE = 101;   // simple prime size
    HashNode* table[TABLE_SIZE];

    PlayerDatabase* db;                 // pointer to main database

    int hash(const string& key) const;

    // helpers on linked lists
    bool alreadyFriends(Player& a, int bIndex) const;
    void addFriendLink(Player& a, int bIndex);
    bool hasRequestFrom(Player& toPlayer, int fromIndex) const;
    bool removeRequestNode(Player& toPlayer, int fromIndex);

    // persistence helpers (friends only)
    void loadFriendsFromFile();
    void saveFriendsToFile() const;

public:
    FriendSystem(PlayerDatabase* dbPtr);
    ~FriendSystem();

    void buildHashTable();

    int findPlayerIndex(const string& username) const;

    // Send friend request from "fromIndex" to "toUsername"
    // Returns true if request added
    bool sendFriendRequest(int fromIndex, const string& toUsername, string& message);

    // Accept request. "toIndex" is current logged in player
    bool acceptFriendRequest(int toIndex, const string& fromUsername, string& message);

    // Reject request (simply removes it)
    bool rejectFriendRequest(int toIndex, const string& fromUsername, string& message);

    // Get list of friend usernames
    void getFriendUsernames(int playerIndex, string outNames[], int& count) const;

    // Get list of pending request usernames
    void getRequestUsernames(int playerIndex, string outNames[], int& count) const;
};
