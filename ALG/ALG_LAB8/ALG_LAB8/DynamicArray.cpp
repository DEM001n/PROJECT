#include "DynamicArray.h"

template <typename T>
DynamicArray<T>::DynamicArray() : currentSize(0), maxSize(1) {
    array = new T[maxSize];
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
    delete[] array;
}

template <typename T>
void DynamicArray<T>::resize() {
    maxSize = static_cast<size_t>(maxSize * 2);
    T* newArray = new T[maxSize];
    for (size_t i = 0; i < currentSize; ++i) {
        newArray[i] = array[i];
    }
    delete[] array;
    array = newArray;
}

template <typename T>
void DynamicArray<T>::add(const T& element) {
    if (currentSize == maxSize) {
        resize();
    }
    array[currentSize++] = element;
}

template <typename T>
T& DynamicArray<T>::get(size_t index) const {
    if (index >= currentSize) {
        throw std::out_of_range("Index out of range");
    }
    return array[index];
}

template <typename T>
void DynamicArray<T>::set(size_t index, const T& value) {
    if (index >= currentSize) {
        throw std::out_of_range("Index out of range");
    }
    array[index] = value;
}

template <typename T>
size_t DynamicArray<T>::size() const {
    return currentSize;
}

template <typename T>
size_t DynamicArray<T>::capacity() const {
    return maxSize;
}

template <typename T>
std::string DynamicArray<T>::to_string() const {
    std::string result = "Array address: " + std::to_string(reinterpret_cast<uintptr_t>(array)) + "\n";
    result += "Current size: " + std::to_string(currentSize) + "\n";
    result += "Max capacity: " + std::to_string(maxSize) + "\n";

    return result;
}

template <typename T>
void DynamicArray<T>::bubbleSort(bool (*comp)(const T&, const T&)) {
    if (comp == nullptr) {
        comp = [](const T& a, const T& b) { return a < b; };
    }

    for (size_t i = 0; i < currentSize - 1; ++i) {
        for (size_t j = 0; j < currentSize - 1 - i; ++j) {
            if (comp(array[j + 1], array[j])) {
                std::swap(array[j], array[j + 1]);
            }
        }
    }
}

template <typename T>
void DynamicArray<T>::clear() {
    delete[] array;
    maxSize = 1;
    currentSize = 0;
    array = new T[maxSize];
}



template <typename T>
T& DynamicArray<T>::operator[](size_t index) const {
    return get(index);
}

template class DynamicArray<int>;
template class DynamicArray<char>;
template class DynamicArray<std::string>;
