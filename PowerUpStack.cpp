// PowerUpStack.cpp
#include "PowerUpStack.h"

namespace
{
    // Map string -> int for saving
    int encodePowerUp(const std::string &val)
    {
        if (val == "FREEZE") return 1;
        return 0;
    }

    // Map int -> string for loading
    std::string decodePowerUp(int code)
    {
        if (code == 1) return "FREEZE";
        // Unknown or empty slot
        return "";
    }
}

PowerUpStack::PowerUpStack()
{
    topIndex = -1;
}

bool PowerUpStack::isEmpty() const
{
    return topIndex == -1;
}

bool PowerUpStack::isFull() const
{
    return topIndex == MAX - 1;
}

void PowerUpStack::push(const std::string &val)
{
    if (!isFull())
        arr[++topIndex] = val;
}

std::string PowerUpStack::pop()
{
    if (isEmpty())
        return "";
    return arr[topIndex--];
}

std::string PowerUpStack::peek() const
{
    if (isEmpty())
        return "";
    return arr[topIndex];
}

int PowerUpStack::size() const
{
    return topIndex + 1;
}

void PowerUpStack::clear()
{
    topIndex = -1;
}

void PowerUpStack::toEncodedArray(int outArr[], int &outTop, int maxOut) const
{
    // If stack is empty
    if (topIndex < 0)
    {
        outTop = -1;
        for (int i = 0; i < maxOut; ++i)
            outArr[i] = 0;
        return;
    }

    // We only store up to maxOut items
    int limit = topIndex;
    if (limit >= maxOut)
        limit = maxOut - 1;

    outTop = limit;

    for (int i = 0; i <= limit; ++i)
    {
        outArr[i] = encodePowerUp(arr[i]);
    }

    for (int i = limit + 1; i < maxOut; ++i)
    {
        outArr[i] = 0;
    }
}

void PowerUpStack::fromEncodedArray(const int inArr[], int inTop)
{
    clear();

    if (inTop < 0)
        return;

    int limit = inTop;
    if (limit >= MAX)
        limit = MAX - 1;

    for (int i = 0; i <= limit; ++i)
    {
        std::string val = decodePowerUp(inArr[i]);
        if (!val.empty())
        {
            push(val);
        }
    }
}