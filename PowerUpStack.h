// PowerUpStack.h
#pragma once
#include <string>

class PowerUpStack
{
private:
    static const int MAX = 50;  
    std::string arr[MAX];
    int topIndex;

public:
    PowerUpStack();

    bool isEmpty() const;
    bool isFull() const;

    void push(const std::string &val);
    std::string pop();
    std::string peek() const;

    int size() const;
};
