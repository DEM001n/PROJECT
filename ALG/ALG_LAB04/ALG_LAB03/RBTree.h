#pragma once
#include <iostream>
#include <algorithm>
#include <functional> 
#include <sstream>   
#include <ctime>
#include <windows.h>

void setConsoleTextColor(WORD color);

int getRandomNumber(int min, int max);

template <typename Func>
void measureExecutionTime(Func func)
{
    std::clock_t start = std::clock();
    func();
    std::clock_t end = std::clock();
    double duration = double(static_cast<int64_t>(end) - static_cast<int64_t>(start)) / (CLOCKS_PER_SEC / 1000);
    setConsoleTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "Execution time: " << duration << " milliseconds" << std::endl;
    setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
};

template <typename T>
struct BSTNode {
    int id;
    bool color;       // false = black, true = red
    T key;
    BSTNode<T>* left;
    BSTNode<T>* right;
    BSTNode<T>* parent;

    BSTNode(int id, T data, bool color);
};

template <typename T>
class RBTree {
private:
    BSTNode<T>* root;
    int size;

    void insertNode(BSTNode<T>*& node, BSTNode<T>* parent, int id, T data, std::function<bool(T, T)> comp);

    void reIndexNodes(BSTNode<T>* node, int id);


    void rotateRight(BSTNode<T>*& root, BSTNode<T>* node);
    void rotateLeft(BSTNode<T>*& root, BSTNode<T>* node);

    void balanceAfterInsert(BSTNode<T>* node);

    void preOrderDisplay(BSTNode<T>* node, int depth, int height) const;
    void inOrderDisplay(BSTNode<T>* node, int height, int depth = 0) const;
    void clearTree(BSTNode<T>* node);
    int getHeightHelper(BSTNode<T>* node) const;
    BSTNode<T>* searchNode(BSTNode<T>* node, T data, std::function<bool(T, T)> comp) const;
       
public:
    RBTree();
    ~RBTree();

    void insert(T data, std::function<bool(T, T)> comp);
    void preOrderDisplay() const;
    void inOrderDisplay() const;
    void display() const;
    int getSize() const;
    int getHeight() const;
    void clear();
    BSTNode<T>* search(T data, std::function<bool(T, T)> comp) const;

};
