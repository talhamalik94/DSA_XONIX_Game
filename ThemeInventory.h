// ThemeInventory.h
#pragma once

#include <string>

// Theme category: 0 = main-menu background, 1 = in-game background
struct ThemeInfo
{
    int id;
    int category;            // 0 menu, 1 game
    std::string name;
    std::string description;
    std::string menuImagePath;
    std::string gameImagePath;
};

struct ThemeNode
{
    ThemeInfo data;
    ThemeNode* left;
    ThemeNode* right;
    int height;
};

class ThemeInventory
{
private:
    ThemeNode* root;

    int nodeHeight(ThemeNode* n);
    int getBalance(ThemeNode* n);
    ThemeNode* rotateRight(ThemeNode* y);
    ThemeNode* rotateLeft(ThemeNode* x);
    ThemeNode* insertNode(ThemeNode* node, const ThemeInfo& info);
    ThemeNode* searchNode(ThemeNode* node, int id);
    void inorderCollect(ThemeNode* node, int category,
                        ThemeInfo outArray[], int& count, int maxCount);
    void clearNode(ThemeNode* node);

public:
    ThemeInventory();
    ~ThemeInventory();

    void insert(const ThemeInfo& info);
    ThemeInfo* searchById(int id);
    void collectByCategory(int category,
                           ThemeInfo outArray[], int& outCount, int maxCount);
};

extern ThemeInventory g_themeInventory;

// Fill AVL tree with predefined themes (call from main once)
void initDefaultThemes();
