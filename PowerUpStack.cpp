// PowerUpStack.cpp
#include "PowerUpStack.h"

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
