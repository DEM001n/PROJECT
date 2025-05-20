#pragma once
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <type_traits>

struct Data
{
    int val_int;
    char val_char;

    Data(int a, char b); 
};

struct Node
{
    Data data;
    Node* next;
    Node* prev;

    Node(int a, char b);
};

template<typename T>
class Deque
{
private:
    Node* front;
    Node* rear;

    bool isEmpty() const;
    void printNodeInfo(Node* node) const;
    bool isEqual(const Data& a, const T& value) const;
    bool comparison(Node* newNode, Node* current) const;
    int countNodes() const;

public:
    Deque();
    void push(int intValue, char charValue);
    void remove(T targetValue);
    void printAllNodeInfo(int direction) const;
    void printNodeAtIndex(int index, int direction) const;
    void clearDeque();
    void updateNodeValue(int index, int direction, int intValue, char charValue);
    void findByValue(T value, int direction) const;

    ~Deque();
};

template <typename Func>
void measureExecutionTime(Func func)
{
    std::clock_t start = std::clock();
    func();
    std::clock_t end = std::clock();
    double duration = double(static_cast<int64_t>(end) - static_cast<int64_t>(start)) / (CLOCKS_PER_SEC / 1000);
    std::cout << "Execution time: " << duration << " milliseconds" << std::endl;
};
