#pragma once
#include <string>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>

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

template <typename V>
struct HashNode {
    std::vector<std::string> keys;
    std::vector<V> values;
};

template <typename V>
class HashTable {
private:
    HashNode<V>** table;
    size_t currentSize;
    size_t maxSize;
    double loadFactor;

    size_t hashFunction(const std::string& key) const;
    void rehash();

public:
    HashTable(size_t initialSize = 8, double loadFactor = 0.75);
    ~HashTable();

    void insert(const std::string& key, const V& value);
    V* find(const std::string& key) const;
    bool remove(const std::string& key);
    void clear();
    std::string toString() const;
    void statistics() const;
};

