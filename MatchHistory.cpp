// MatchHistory.cpp
#include "MatchHistory.h"
#include <fstream>
#include <sstream>

MatchHistory::MatchHistory()
{
    front = 0;
    rear  = 0;
}

bool MatchHistory::isEmpty() const
{
    return front == rear;
}

bool MatchHistory::isFull() const
{
    return (rear + 1) % MAX == front;
}

void MatchHistory::enqueue(const MatchRecord &r)
{
    if (!isFull())
    {
        arr[rear] = r;
        rear = (rear + 1) % MAX;
    }
}

MatchRecord MatchHistory::dequeue()
{
    if (isEmpty())
        return MatchRecord{"", "", 0, ""};

    MatchRecord r = arr[front];
    front = (front + 1) % MAX;
    return r;
}

int MatchHistory::size() const
{
    return (rear - front + MAX) % MAX;
}

MatchRecord MatchHistory::get(int index) const
{
    int realIdx = (front + index) % MAX;
    return arr[realIdx];
}

// Now only APPEND the last record for this user
void MatchHistory::saveToFile(const std::string &username) const
{
    if (isEmpty())
        return;

    std::ofstream file("player_history.txt", std::ios::app);
    if (!file.good())
        return;

    // last enqueued record
    int lastIndex = (rear - 1 + MAX) % MAX;
    const MatchRecord &r = arr[lastIndex];

    file << username << "|"
         << r.opponent  << "|"
         << r.result    << "|"
         << r.score     << "|"
         << r.timestamp << "\n";
}

// Read lines like: user|opponent|result|score|timestamp
void MatchHistory::loadFromFile(const std::string &username)
{
    std::ifstream file("player_history.txt");
    if (!file.good())
    {
        front = rear = 0;
        return;
    }

    front = rear = 0;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string user, opp, res, scoreStr, timeStr;

        if (!std::getline(ss, user, '|'))
            continue;

        if (user != username)
            continue;

        if (!std::getline(ss, opp,      '|')) continue;
        if (!std::getline(ss, res,      '|')) continue;
        if (!std::getline(ss, scoreStr, '|')) continue;
        std::getline(ss, timeStr, '|'); // last field; may contain spaces

        int score = 0;
        try { score = std::stoi(scoreStr); } catch (...) { score = 0; }

        MatchRecord r;
        r.opponent  = opp;
        r.result    = res;
        r.score     = score;
        r.timestamp = timeStr;

        enqueue(r);
    }

    file.close();
}
