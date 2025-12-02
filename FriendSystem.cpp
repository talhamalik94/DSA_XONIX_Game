#include "FriendSystem.h"
#include <iostream>
#include <fstream>
using namespace std;

FriendSystem::FriendSystem(PlayerDatabase* dbPtr)
{
    db = dbPtr;
    for (int i = 0; i < TABLE_SIZE; i++)
        table[i] = nullptr;

    // build hash and load friends from file
    buildHashTable();
    loadFriendsFromFile();
}

FriendSystem::~FriendSystem()
{
    // save friends to file on exit
    saveFriendsToFile();

    // free hash table nodes
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashNode* cur = table[i];
        while (cur != nullptr)
        {
            HashNode* next = cur->next;
            delete cur;
            cur = next;
        }
        table[i] = nullptr;
    }
}

int FriendSystem::hash(const string& key) const
{
    unsigned int h = 0;
    for (size_t i = 0; i < key.length(); i++)
    {
        h = h * 31 + (unsigned char)key[i];
    }
    return (int)(h % TABLE_SIZE);
}

void FriendSystem::buildHashTable()
{
    // clear old table
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashNode* cur = table[i];
        while (cur != nullptr)
        {
            HashNode* next = cur->next;
            delete cur;
            cur = next;
        }
        table[i] = nullptr;
    }

    int n = db->getSize();
    for (int i = 0; i < n; i++)
    {
        const Player& p = db->getPlayer(i);
        int h = hash(p.username);
        HashNode* node = new HashNode;
        node->username = p.username;
        node->index = i;
        node->next = table[h];
        table[h] = node;
    }
}

int FriendSystem::findPlayerIndex(const string& username) const
{
    int h = hash(username);
    HashNode* cur = table[h];
    while (cur != nullptr)
    {
        if (cur->username == username)
            return cur->index;
        cur = cur->next;
    }
    return -1;
}

// Linked list helpers

bool FriendSystem::alreadyFriends(Player& a, int bIndex) const
{
    FriendNode* cur = a.friendsHead;
    while (cur != nullptr)
    {
        if (cur->friendIndex == bIndex)
            return true;
        cur = cur->next;
    }
    return false;
}

void FriendSystem::addFriendLink(Player& a, int bIndex)
{
    FriendNode* node = new FriendNode;
    node->friendIndex = bIndex;
    node->next = a.friendsHead;
    a.friendsHead = node;
}

bool FriendSystem::hasRequestFrom(Player& toPlayer, int fromIndex) const
{
    RequestNode* cur = toPlayer.requestsHead;
    while (cur != nullptr)
    {
        if (cur->fromIndex == fromIndex)
            return true;
        cur = cur->next;
    }
    return false;
}

bool FriendSystem::removeRequestNode(Player& toPlayer, int fromIndex)
{
    RequestNode* cur = toPlayer.requestsHead;
    RequestNode* prev = nullptr;

    while (cur != nullptr)
    {
        if (cur->fromIndex == fromIndex)
        {
            if (prev == nullptr)
                toPlayer.requestsHead = cur->next;
            else
                prev->next = cur->next;

            delete cur;
            return true;
        }

        prev = cur;
        cur = cur->next;
    }
    return false;
}


void FriendSystem::saveFriendsToFile() const
{
    ofstream out("friends.txt");
    if (!out)
        return;

    int n = db->getSize();
    for (int i = 0; i < n; i++)
    {
        const Player& p = db->getPlayer(i);
        FriendNode* cur = p.friendsHead;
        while (cur != nullptr)
        {
            int j = cur->friendIndex;
            if (i < j) // write each undirected pair once
            {
                const Player& fp = db->getPlayer(j);
                out << p.username << " " << fp.username << "\n";
            }
            cur = cur->next;
        }
    }
}

void FriendSystem::loadFriendsFromFile()
{
    ifstream in("friends.txt");
    if (!in)
        return;

    string u1, u2;
    while (in >> u1 >> u2)
    {
        int i = findPlayerIndex(u1);
        int j = findPlayerIndex(u2);
        if (i == -1 || j == -1)
            continue;

        Player& p1 = db->getPlayerRef(i);
        Player& p2 = db->getPlayerRef(j);

        if (!alreadyFriends(p1, j))
            addFriendLink(p1, j);
        if (!alreadyFriends(p2, i))
            addFriendLink(p2, i);
    }
}


bool FriendSystem::sendFriendRequest(int fromIndex, const string& toUsername, string& message)
{
    if (fromIndex < 0)
    {
        message = "You must be logged in.";
        return false;
    }

    int toIndex = findPlayerIndex(toUsername);
    if (toIndex == -1)
    {
        message = "User not found.";
        return false;
    }

    if (toIndex == fromIndex)
    {
        message = "You cannot add yourself.";
        return false;
    }

    Player& fromPlayer = db->getPlayerRef(fromIndex);
    Player& toPlayer   = db->getPlayerRef(toIndex);

    if (alreadyFriends(fromPlayer, toIndex))
    {
        message = "Already friends.";
        return false;
    }

    if (hasRequestFrom(toPlayer, fromIndex))
    {
        message = "Request already sent and pending.";
        return false;
    }

    // add request node to "toPlayer"
    RequestNode* node = new RequestNode;
    node->fromIndex = fromIndex;
    node->next = toPlayer.requestsHead;
    toPlayer.requestsHead = node;

    message = "Friend request sent.";
    return true;
}

bool FriendSystem::acceptFriendRequest(int toIndex, const string& fromUsername, string& message)
{
    if (toIndex < 0)
    {
        message = "You must be logged in.";
        return false;
    }

    int fromIndex = findPlayerIndex(fromUsername);
    if (fromIndex == -1)
    {
        message = "User not found.";
        return false;
    }

    Player& toPlayer   = db->getPlayerRef(toIndex);
    Player& fromPlayer = db->getPlayerRef(fromIndex);

    if (!removeRequestNode(toPlayer, fromIndex))
    {
        message = "No such pending request.";
        return false;
    }

    // add both ways in friend lists
    if (!alreadyFriends(toPlayer, fromIndex))
        addFriendLink(toPlayer, fromIndex);
    if (!alreadyFriends(fromPlayer, toIndex))
        addFriendLink(fromPlayer, toIndex);

    // persist friends
    saveFriendsToFile();

    message = "Friend request accepted.";
    return true;
}

bool FriendSystem::rejectFriendRequest(int toIndex, const string& fromUsername, string& message)
{
    if (toIndex < 0)
    {
        message = "You must be logged in.";
        return false;
    }

    int fromIndex = findPlayerIndex(fromUsername);
    if (fromIndex == -1)
    {
        message = "User not found.";
        return false;
    }

    Player& toPlayer = db->getPlayerRef(toIndex);

    if (!removeRequestNode(toPlayer, fromIndex))
    {
        message = "No such pending request.";
        return false;
    }

    message = "Friend request rejected.";
    return true;
}

void FriendSystem::getFriendUsernames(int playerIndex, string outNames[], int& count) const
{
    count = 0;
    if (playerIndex < 0) return;

    const Player& p = db->getPlayer(playerIndex);
    FriendNode* cur = p.friendsHead;

    while (cur != nullptr && count < 50)
    {
        const Player& fp = db->getPlayer(cur->friendIndex);
        outNames[count] = fp.username;
        count++;
        cur = cur->next;
    }
}

void FriendSystem::getRequestUsernames(int playerIndex, string outNames[], int& count) const
{
    count = 0;
    if (playerIndex < 0) return;

    const Player& p = db->getPlayer(playerIndex);
    RequestNode* cur = p.requestsHead;

    while (cur != nullptr && count < 50)
    {
        const Player& fromP = db->getPlayer(cur->fromIndex);
        outNames[count] = fromP.username;
        count++;
        cur = cur->next;
    }
}
