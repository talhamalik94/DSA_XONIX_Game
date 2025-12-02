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

     // NEW helper functions for save/load
    void clear();

    // Encode current stack into integer array [0..maxOut-1]
    // outTop will be the last valid index in outArr, or -1 if empty
    void toEncodedArray(int outArr[], int &outTop, int maxOut) const;

    // Rebuild stack from encoded array and top index
    void fromEncodedArray(const int inArr[], int inTop);
};
