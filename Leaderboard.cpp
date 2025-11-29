// Leaderboard.cpp
#include "Leaderboard.h"

Leaderboard::Leaderboard()
{
    heapSize = 0;
}

void Leaderboard::clear()
{
    heapSize = 0;
}

void Leaderboard::heapifyUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;
        if (heap[index].score < heap[parent].score)
        {
            LeaderboardEntry temp = heap[index];
            heap[index] = heap[parent];
            heap[parent] = temp;
            index = parent;
        }
        else
        {
            break;
        }
    }
}

void Leaderboard::heapifyDown(int index)
{
    while (true)
    {
        int left  = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < heapSize && heap[left].score < heap[smallest].score)
            smallest = left;

        if (right < heapSize && heap[right].score < heap[smallest].score)
            smallest = right;

        if (smallest != index)
        {
            LeaderboardEntry temp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = temp;
            index = smallest;
        }
        else
        {
            break;
        }
    }
}

void Leaderboard::insert(int playerIndex, int score)
{
    // ignore negative scores
    if (score < 0)
        return;

    if (heapSize < 10)
    {
        heap[heapSize].playerIndex = playerIndex;
        heap[heapSize].score       = score;
        heapifyUp(heapSize);
        heapSize++;
    }
    else
    {
        // heap full, check against smallest (root)
        if (score <= heap[0].score)
        {
            // does not enter top 10
            return;
        }

        // replace root and restore heap
        heap[0].playerIndex = playerIndex;
        heap[0].score       = score;
        heapifyDown(0);
    }
}

void Leaderboard::buildFromDatabase(const PlayerDatabase& db)
{
    clear();

    int count = db.getSize();
    for (int i = 0; i < count; i++)
    {
        const Player& p = db.getPlayer(i);
        insert(i, p.totalScore);
    }
}

void Leaderboard::toSortedArray(LeaderboardEntry out[], int& outSize) const
{
    outSize = heapSize;

    // copy heap
    for (int i = 0; i < heapSize; i++)
    {
        out[i] = heap[i];
    }

    // selection sort in descending order (highest score first)
    for (int i = 0; i < outSize - 1; i++)
    {
        int maxIndex = i;
        for (int j = i + 1; j < outSize; j++)
        {
            if (out[j].score > out[maxIndex].score)
            {
                maxIndex = j;
            }
        }

        if (maxIndex != i)
        {
            LeaderboardEntry temp = out[i];
            out[i] = out[maxIndex];
            out[maxIndex] = temp;
        }
    }
}
