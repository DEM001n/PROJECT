#pragma once
#include <string>
#include <stdexcept>
#include <iostream>
#include <ctime>

template <typename Func>
double measureExecutionTime(Func func) {
    std::clock_t start = std::clock();
    func();
    std::clock_t end = std::clock();
    double duration = double(static_cast<int64_t>(end) - static_cast<int64_t>(start)) / (CLOCKS_PER_SEC / 1000);
    return duration;
}

template <typename T>
class DynamicArray {
private:
    T* array;
    size_t currentSize;
    size_t maxSize;

    void resize();

public:
    DynamicArray();
    ~DynamicArray();

    void add(const T& element);
    T& get(size_t index) const;
    void set(size_t index, const T& value);
    size_t size() const;
    size_t capacity() const;
    std::string to_string() const;
    void bubbleSort(bool (*comp)(const T&, const T&) = nullptr);
    void clear();

    T& operator[](size_t index) const;
};
