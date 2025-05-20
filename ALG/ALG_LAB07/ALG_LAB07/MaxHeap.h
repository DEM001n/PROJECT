#pragma once
#include <iostream>
#include <stdexcept>
#include <string>
#include <ctime>
#include <windows.h>
#include <stdlib.h>

void setConsoleTextColor(WORD color);

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
class MaxHeap {
private:
    T* heapTable; // Массив типа T напрямую
    size_t currentSize;
    size_t maxSize;
    bool isExternalArray;

    void resize();
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    MaxHeap();
    MaxHeap(T* externalArray, size_t size, bool topDown);
    ~MaxHeap();

    int getHeight() const;
    void add(const T& value);
    T extractMax();
    void clear();
    std::string to_string() const;

    void heapifyUpRecursive(int index);
    void heapifyDownRecursive(int index);

    void heapifyTopDown();
    void heapifyBottomUp();

    void sort();
    size_t size() const;
    size_t capacity() const;
};
