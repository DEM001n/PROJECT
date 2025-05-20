#include "MaxHeap.h"

void setConsoleTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

template <typename T>
MaxHeap<T>::MaxHeap() : currentSize(0), maxSize(1), isExternalArray(false) {
    heapTable = new T[maxSize];
}

template <typename T>
MaxHeap<T>::MaxHeap(T* externalArray, size_t size, bool topDown)
    : heapTable(externalArray), currentSize(size), maxSize(size), isExternalArray(true) {
    if (topDown) {
        heapifyTopDown();
    }
    else {
        heapifyBottomUp();
    }
}

template <typename T>
MaxHeap<T>::~MaxHeap() {
    if (!isExternalArray) {
        delete[] heapTable;
    }
}

template <typename T>
void MaxHeap<T>::resize() {
    maxSize *= 2;
    T* newHeapTable = new T[maxSize];
    for (size_t i = 0; i < currentSize; ++i) {
        newHeapTable[i] = heapTable[i];
    }
    delete[] heapTable;
    heapTable = newHeapTable;
}

template <typename T>
void MaxHeap<T>::add(const T& value) {
    for (size_t i = 0; i < currentSize; ++i) {
        if (heapTable[i] == value) {
            return;
        }
    }

    if (currentSize == maxSize) {
        resize();
    }

    heapTable[currentSize] = value;
    heapifyUp(currentSize);
    currentSize++;
}


template <typename T>
int MaxHeap<T>::getHeight() const {
    if (currentSize == 0) {
        return -1;
    }
    return static_cast<int>(std::log2(currentSize));
}

template <typename T>
T MaxHeap<T>::extractMax() {
    if (currentSize == 0) {
        throw std::out_of_range("Heap is empty");
    }
    T maxValue = heapTable[0];
    heapTable[0] = heapTable[currentSize - 1];
    currentSize--;
    heapifyDown(0);
    return maxValue;
}

template <typename T>
void MaxHeap<T>::clear() {
    if (!isExternalArray) {
        delete[] heapTable;
    }
    maxSize = 1;
    currentSize = 0;
    heapTable = new T[maxSize];
}

template <typename T>
std::string MaxHeap<T>::to_string() const {
    std::string result;
    result += "maxSize: " + std::to_string(maxSize) + "\n" + "currentSize: " + std::to_string(currentSize) + "\n\n";

    size_t elementsInLevel = 1;
    size_t i = 0;

    while (i < currentSize) {
        for (size_t j = 0; j < elementsInLevel && i < currentSize; ++j, ++i) {
            result += std::to_string(heapTable[i]) + " ";
        }
        result += "\n\n";
        elementsInLevel *= 2;
    }

    return result;
}

template <typename T>
void MaxHeap<T>::heapifyUp(int index) {
    int parentIndex = (index - 1) / 2;
    if (index > 0 && heapTable[index] > heapTable[parentIndex]) {
        std::swap(heapTable[index], heapTable[parentIndex]);
        heapifyUp(parentIndex);
    }
}

template <typename T>
void MaxHeap<T>::heapifyDown(int index) {
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int largest = index;

    if (leftChild < currentSize && heapTable[leftChild] > heapTable[largest]) {
        largest = leftChild;
    }
    if (rightChild < currentSize && heapTable[rightChild] > heapTable[largest]) {
        largest = rightChild;
    }

    if (largest != index) {
        std::swap(heapTable[index], heapTable[largest]);
        heapifyDown(largest);
    }
}

template <typename T>
void MaxHeap<T>::heapifyUpRecursive(int index) {
    if (index == 0) return;
    int parentIndex = (index - 1) / 2;
    if (heapTable[index] > heapTable[parentIndex]) {
        std::swap(heapTable[index], heapTable[parentIndex]);
        heapifyUpRecursive(parentIndex);
    }
}

template <typename T>
void MaxHeap<T>::heapifyDownRecursive(int index) {
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int largest = index;

    if (leftChild < currentSize && heapTable[leftChild] > heapTable[largest]) {
        largest = leftChild;
    }
    if (rightChild < currentSize && heapTable[rightChild] > heapTable[largest]) {
        largest = rightChild;
    }

    if (largest != index) {
        std::swap(heapTable[index], heapTable[largest]);
        heapifyDownRecursive(largest);
    }
}

template <typename T>
void MaxHeap<T>::heapifyTopDown() {
    for (size_t i = 1; i < currentSize; ++i) {
        heapifyUp(i);
    }
}

template <typename T>
void MaxHeap<T>::heapifyBottomUp() {
    for (int i = (currentSize / 2) - 1; i >= 0; --i) {
        heapifyDown(i);
    }
}

template <typename T>
void MaxHeap<T>::sort() {
    size_t originalSize = currentSize;
    for (size_t i = currentSize - 1; i > 0; --i) {
        std::swap(heapTable[0], heapTable[i]);
        currentSize--;
        heapifyDown(0);
    }
    currentSize = originalSize;
}

template <typename T>
size_t MaxHeap<T>::size() const {
    return currentSize;
}

template <typename T>
size_t MaxHeap<T>::capacity() const {
    return maxSize;
}

template class MaxHeap<int>;
template class MaxHeap<char>;
