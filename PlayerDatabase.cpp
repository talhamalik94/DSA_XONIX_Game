// PlayerDatabase.cpp
#include <iostream>
#include <fstream>
#include "PlayerDatabase.h"

using namespace std;

PlayerDatabase::PlayerDatabase(const string& file)
{
    players  = nullptr;
    size     = 0;
    capacity = 0;
    filename = file;
    loadFromFile();
}

PlayerDatabase::~PlayerDatabase()
{
    delete[] players;
}

void PlayerDatabase::ensureCapacity()
{
    if (capacity == 0)
    {
        capacity = 10;
        players = new Player[capacity];
    }
    else if (size >= capacity)
    {
        int newCapacity = capacity * 2;
        Player* temp = new Player[newCapacity];
        for (int i = 0; i < size; i++)
            temp[i] = players[i];
        delete[] players;
        players = temp;
        capacity = newCapacity;
    }
}

int PlayerDatabase::findIndexByUsername(const string& username) const
{
    for (int i = 0; i < size; i++)
    {
        if (players[i].username == username)
            return i;
    }
    return -1;
}

void PlayerDatabase::loadFromFile()
{
    delete[] players;
    players  = nullptr;
    size     = 0;
    capacity = 0;

    ifstream in(filename.c_str());
    if (!in)
    {
        // no file yet, start empty
        return;
    }

    Player temp;
    while (in >> temp.username >> temp.password >> temp.nickname >> temp.email >> temp.totalScore)
    {
        ensureCapacity();
        players[size] = temp;
        size++;
    }

    in.close();
}

void PlayerDatabase::saveToFile() const
{
    ofstream out(filename.c_str());
    if (!out)
    {
        cout << "Could not open player file for writing: " << filename << "\n";
        return;
    }

    for (int i = 0; i < size; i++)
    {
        out << players[i].username << " "
            << players[i].password << " "
            << players[i].nickname << " "
            << players[i].email    << " "
            << players[i].totalScore << "\n";
    }

    out.close();
}

int PlayerDatabase::getSize() const
{
    return size;
}

const Player& PlayerDatabase::getPlayer(int index) const
{
    // assume index valid (Leaderboard only calls with correct index)
    return players[index];
}





bool PlayerDatabase::registerPlayer(const string& username,
                                    const string& password,
                                    const string& nickname,
                                    const string& email,
                                    string& errorMessage)
{
    // simple checks
    if (username == "" || password == "")
    {
        errorMessage = "Username and password cannot be empty.";
        return false;
    }

    if (findIndexByUsername(username) != -1)
    {
        errorMessage = "Username already exists.";
        return false;
    }

    ensureCapacity();
    players[size].username   = username;
    players[size].password   = password;
    players[size].nickname   = nickname;
    players[size].email      = email;
    players[size].totalScore = 0;
    size++;

    saveToFile();
    errorMessage = "";
    return true;
}

int PlayerDatabase::authenticate(const string& username,
                                 const string& password,
                                 string& errorMessage) const
{
    int idx = findIndexByUsername(username);
    if (idx == -1)
    {
        errorMessage = "User not found.";
        return -1;
    }

    if (players[idx].password != password)
    {
        errorMessage = "Incorrect password.";
        return -1;
    }

    errorMessage = "";
    return idx;
}

bool PlayerDatabase::changePassword(const string& username,
                                    const string& newPassword,
                                    string& errorMessage)
{
    int idx = findIndexByUsername(username);
    if (idx == -1)
    {
        errorMessage = "User not found.";
        return false;
    }

    if (newPassword == "")
    {
        errorMessage = "Password cannot be empty.";
        return false;
    }

    players[idx].password = newPassword;
    saveToFile();
    errorMessage = "";
    return true;
}

void PlayerDatabase::updateScore(int index, int deltaScore)
{
    if (index < 0 || index >= size)
        return;

    players[index].totalScore += deltaScore;
    if (players[index].totalScore < 0)
        players[index].totalScore = 0;

    saveToFile();
}

Player& PlayerDatabase::getPlayerRef(int index)
{
    return players[index];
}

