// MatchHistory.h
#pragma once
#include <string>

struct MatchRecord
{
    std::string opponent;
    std::string result;    // "WIN" or "LOSS"
    int         score;
    std::string timestamp; // human readable time string
};

class MatchHistory
{
private:
    static const int MAX = 200;
    MatchRecord arr[MAX];
    int front, rear;

public:
    MatchHistory();

    bool isEmpty() const;
    bool isFull()  const;

    void enqueue(const MatchRecord &r);
    MatchRecord dequeue();
    int  size() const;

    // access for UI (0 = oldest visible)
    MatchRecord get(int index) const;

    // append the LAST record in the queue for this username
    void saveToFile(const std::string &username) const;
    // load all records for this username
    void loadFromFile(const std::string &username);
};
