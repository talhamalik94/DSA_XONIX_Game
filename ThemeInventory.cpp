// ThemeInventory.cpp
#include "ThemeInventory.h"

ThemeInventory g_themeInventory;


int ThemeInventory::nodeHeight(ThemeNode* n)
{
    return n ? n->height : 0;
}

int ThemeInventory::getBalance(ThemeNode* n)
{
    if (!n) return 0;
    return nodeHeight(n->left) - nodeHeight(n->right);
}

ThemeNode* ThemeInventory::rotateRight(ThemeNode* y)
{
    ThemeNode* x  = y->left;
    ThemeNode* T2 = x->right;

    x->right = y;
    y->left  = T2;

    y->height = 1 + (nodeHeight(y->left) > nodeHeight(y->right) ? nodeHeight(y->left) : nodeHeight(y->right));
    x->height = 1 + (nodeHeight(x->left) > nodeHeight(x->right) ? nodeHeight(x->left) : nodeHeight(x->right));

    return x;
}

ThemeNode* ThemeInventory::rotateLeft(ThemeNode* x)
{
    ThemeNode* y  = x->right;
    ThemeNode* T2 = y->left;

    y->left  = x;
    x->right = T2;

    x->height = 1 + (nodeHeight(x->left) > nodeHeight(x->right)
                     ? nodeHeight(x->left) : nodeHeight(x->right));
    y->height = 1 + (nodeHeight(y->left) > nodeHeight(y->right)
                     ? nodeHeight(y->left) : nodeHeight(y->right));

    return y;
}

ThemeNode* ThemeInventory::insertNode(ThemeNode* node, const ThemeInfo& info)
{
    if (!node)
    {
        ThemeNode* n = new ThemeNode;
        n->data   = info;
        n->left   = 0;
        n->right  = 0;
        n->height = 1;
        return n;
    }

    if (info.id < node->data.id)
        node->left  = insertNode(node->left, info);
    else if (info.id > node->data.id)
        node->right = insertNode(node->right, info);
    else
        return node;  // duplicate id ignored

    node->height = 1 + (nodeHeight(node->left) > nodeHeight(node->right)
                        ? nodeHeight(node->left) : nodeHeight(node->right));

    int balance = getBalance(node);

    // Left Left
    if (balance > 1 && info.id < node->left->data.id)
        return rotateRight(node);

    // Right Right
    if (balance < -1 && info.id > node->right->data.id)
        return rotateLeft(node);

    // Left Right
    if (balance > 1 && info.id > node->left->data.id)
    {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left
    if (balance < -1 && info.id < node->right->data.id)
    {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

ThemeNode* ThemeInventory::searchNode(ThemeNode* node, int id)
{
    if (!node) return 0;

    if (id == node->data.id) return node;
    if (id < node->data.id)  return searchNode(node->left, id);
    return searchNode(node->right, id);
}

void ThemeInventory::inorderCollect(ThemeNode* node, int category,
                                    ThemeInfo outArray[], int& count, int maxCount)
{
    if (!node || count >= maxCount) return;

    inorderCollect(node->left, category, outArray, count, maxCount);

    if (node->data.category == category && count < maxCount)
    {
        outArray[count] = node->data;
        count++;
    }

    inorderCollect(node->right, category, outArray, count, maxCount);
}

void ThemeInventory::clearNode(ThemeNode* node)
{
    if (!node) return;
    clearNode(node->left);
    clearNode(node->right);
    delete node;
}

// ------ public interface ------

ThemeInventory::ThemeInventory()
{
    root = 0;
}

ThemeInventory::~ThemeInventory()
{
    clearNode(root);
}

void ThemeInventory::insert(const ThemeInfo& info)
{
    root = insertNode(root, info);
}
int ThemeInventory::getFirstIdInCategory(int category)
{
    ThemeInfo list[MAX_THEMES];
    int count = 0;
    collectByCategory(category, list, count, MAX_THEMES);
    if (count > 0)
        return list[0].id;
    return -1;
}


ThemeInfo* ThemeInventory::searchById(int id)
{
    ThemeNode* n = searchNode(root, id);
    if (!n) return 0;
    return &n->data;
}

void ThemeInventory::collectByCategory(int category,
                                       ThemeInfo outArray[], int& outCount, int maxCount)
{
    outCount = 0;
    inorderCollect(root, category, outArray, outCount, maxCount);
}


void initDefaultThemes()
{
    // Main menu themes (category = 0)
    ThemeInfo t;

    t.id = 101;
    t.category = 0;
    t.name = "Stony Peak";
    t.description = "Classic Masterpiece";
    t.menuImagePath = "images/classic.jpg";
    t.gameImagePath = "";    
    g_themeInventory.insert(t);

    t.id = 102;
    t.category = 0;
    t.name = "Demon";
    t.description = "Dark and Mysterious";
    t.menuImagePath = "images/darkcastle.jpg";
    t.gameImagePath = "";
    g_themeInventory.insert(t);

    t.id = 103;
    t.category = 0;
    t.name = "Hexagon Abstract";
    t.description = "Black Red Hexagons";
    t.menuImagePath = "images/xonix.jpg";
    t.gameImagePath = "";
    g_themeInventory.insert(t);

    t.id = 104;
    t.category = 0;
    t.name = "Game Of Thrones";
    t.description = "Lion Sigil";
    t.menuImagePath = "images/night.jpg";
    t.gameImagePath = "";
    g_themeInventory.insert(t);

    // Game backgrounds (category = 1). Use your real game bg images.
    t.id = 201;
    t.category = 1;
    t.name = "War";
    t.description = "Fiery war zone";
    t.menuImagePath = "";
    t.gameImagePath = "images/war.jpg";
    g_themeInventory.insert(t);

    t.id = 202;
    t.category = 1;
    t.name = "One Man Army";
    t.description = "Soldier in action";
    t.menuImagePath = "";
    t.gameImagePath = "images/demon.jpg";
    g_themeInventory.insert(t);

    t.id = 203;
    t.category = 1;
    t.name = "Chess Rulers";
    t.description = "Majestic chess pieces";
    t.menuImagePath = "";
    t.gameImagePath = "images/chess.jpg";
    g_themeInventory.insert(t);

    t.id = 204;
    t.category = 1;
    t.name = "Retro Plain";
    t.description = "Flat retro landscape";
    t.menuImagePath = "";
    t.gameImagePath = "images/controller.jpg";
    g_themeInventory.insert(t);
}
