// Matchmaking.cpp
#include "MatchMaking.h"

// -------------- MatchPriorityQueue (max heap) ------------------

MatchPriorityQueue::MatchPriorityQueue()
{
    size = 0;
}

void MatchPriorityQueue::clear()
{
    size = 0;
}

bool MatchPriorityQueue::isEmpty() const
{
    return size == 0;
}

int MatchPriorityQueue::getSize() const
{
    return size;
}

void MatchPriorityQueue::heapifyUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;
        if (priorities[index] > priorities[parent])
        {
            int tempIndex = heap[index];
            heap[index] = heap[parent];
            heap[parent] = tempIndex;

            int tempPr = priorities[index];
            priorities[index] = priorities[parent];
            priorities[parent] = tempPr;

            index = parent;
        }
        else
        {
            break;
        }
    }
}

void MatchPriorityQueue::heapifyDown(int index)
{
    while (true)
    {
        int left  = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < size && priorities[left] > priorities[largest])
            largest = left;

        if (right < size && priorities[right] > priorities[largest])
            largest = right;

        if (largest != index)
        {
            int tempIndex = heap[index];
            heap[index] = heap[largest];
            heap[largest] = tempIndex;

            int tempPr = priorities[index];
            priorities[index] = priorities[largest];
            priorities[largest] = tempPr;

            index = largest;
        }
        else
        {
            break;
        }
    }
}

bool MatchPriorityQueue::contains(int playerIndex) const
{
    for (int i = 0; i < size; i++)
    {
        if (heap[i] == playerIndex)
            return true;
    }
    return false;
}

void MatchPriorityQueue::insert(int playerIndex, int score)
{
    if (size >= 100)
        return; // full, ignore

    // do not insert duplicates
    if (contains(playerIndex))
        return;

    heap[size] = playerIndex;
    priorities[size] = score;
    heapifyUp(size);
    size++;
}

int MatchPriorityQueue::extractMax()
{
    if (size == 0)
        return -1;

    int maxIndex = heap[0];

    heap[0] = heap[size - 1];
    priorities[0] = priorities[size - 1];
    size--;

    if (size > 0)
        heapifyDown(0);

    return maxIndex;
}

// -------------- GameRoomQueue (FIFO queue) ------------------

GameRoomQueue::GameRoomQueue()
{
    front = 0;
    rear  = 0;
    count = 0;
}

void GameRoomQueue::clear()
{
    front = 0;
    rear  = 0;
    count = 0;
}

bool GameRoomQueue::isEmpty() const
{
    return count == 0;
}

bool GameRoomQueue::isFull() const
{
    return count == 100;
}

bool GameRoomQueue::enqueue(const Match& m)
{
    if (isFull())
        return false;

    rooms[rear] = m;
    rear = (rear + 1) % 100;
    count++;
    return true;
}

bool GameRoomQueue::dequeue(Match& m)
{
    if (isEmpty())
        return false;

    m = rooms[front];
    front = (front + 1) % 100;
    count--;
    return true;
}

// -------------- MatchmakingSystem ------------------

MatchmakingSystem::MatchmakingSystem()
{
    // nothing else
}

void MatchmakingSystem::clear()
{
    pq.clear();
    roomQueue.clear();
}

bool MatchmakingSystem::addPlayer(int playerIndex, int score)
{
    // if already in priority queue, do nothing
    if (pq.contains(playerIndex))
        return false;

    pq.insert(playerIndex, score);
    return true;
}

void MatchmakingSystem::createMatches()
{
    // While we have at least two players waiting, create pairs.
    while (pq.getSize() >= 2)
    {
        int p1 = pq.extractMax();
        int p2 = pq.extractMax();

        if (p1 == -1 || p2 == -1)
            break;

        Match m;
        m.player1 = p1;
        m.player2 = p2;

        roomQueue.enqueue(m);
    }
}

bool MatchmakingSystem::getMatchForPlayer(int playerIndex, int& opponentIndex)
{
    // linear scan of game rooms to check if this player has a match
    // simple and good enough for small data.

    if (roomQueue.isEmpty())
        return false;

    // Since GameRoomQueue is a circular queue, we cannot directly access its internals here
    // but for simplicity, we will dequeue all, search, and re-enqueue them back.

    GameRoomQueue temp;
    bool found = false;
    opponentIndex = -1;

    Match current;
    while (roomQueue.dequeue(current))
    {
        if (!found)
        {
            if (current.player1 == playerIndex)
            {
                found = true;
                opponentIndex = current.player2;
                // do not re-enqueue this match, it is consumed
            }
            else if (current.player2 == playerIndex)
            {
                found = true;
                opponentIndex = current.player1;
                // do not re-enqueue this match, it is consumed
            }
            else
            {
                // player not in this room, keep it
                temp.enqueue(current);
            }
        }
        else
        {
            // already found his match, just keep the rest
            temp.enqueue(current);
        }
    }

    // restore roomQueue from temp
    Match m2;
    while (temp.dequeue(m2))
    {
        roomQueue.enqueue(m2);
    }

    return found;
}