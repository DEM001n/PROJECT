#pragma once
#include <iostream>
#include <algorithm>
#include <functional> 
#include <sstream>   
#include <time.h>
#include <cstdlib>  // rand() srand()
#include <ctime>    // time()

int getRandomNumber(int min, int max);

template <typename Func>
void measureExecutionTime(Func func)
{
    std::clock_t start = std::clock();
    func();
    std::clock_t end = std::clock();
    double duration = double(static_cast<int64_t>(end) - static_cast<int64_t>(start)) / (CLOCKS_PER_SEC / 1000);
    std::cout << "Execution time: " << duration << " milliseconds" << std::endl;
};

template <typename T>
struct BSTNode {
    int id;
    T key;
    BSTNode<T>* left;
    BSTNode<T>* right;
    BSTNode<T>* parent;

    BSTNode(int id, T data);
};

template <typename T>
class BST {
private:
    BSTNode<T>* root;
    int size;
    int count;

    void insertNode(BSTNode<T>*& node, BSTNode<T>* parent, int id, T data, std::function<bool(T, T)> comp);
    BSTNode<T>* searchNode(BSTNode<T>* node, T data, std::function<bool(T, T)> comp) const;
    void deleteNode(BSTNode<T>*& node, T data, std::function<bool(T, T)> comp);
    BSTNode<T>* getMin(BSTNode<T>* node) const;
    void preOrderDisplay(BSTNode<T>* node, int depth, int height) const;
    void inOrderDisplay(BSTNode<T>* node, int height, int depth = 0) const;
    void clearTree(BSTNode<T>* node);
    int getHeightHelper(BSTNode<T>* node) const;

public:
    BST();
    ~BST();

    void insert(T data, std::function<bool(T, T)> comp);
    BSTNode<T>* search(T data, std::function<bool(T, T)> comp) const;
    void deleteNode(T data, std::function<bool(T, T)> comp);
    void preOrderDisplay() const;
    void display() const;
    int getSize() const;
    int getHeight() const;
};
