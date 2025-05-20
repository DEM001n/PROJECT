//ALGO2 IS1 210B LAB06
//Dmytro Mahaliuk
//md55722@zut.edu.pl

#include <iostream>
#include <stdexcept>
#include <string>
#include <ctime>
#include <windows.h>
#include <stdlib.h>
#include <vector>  

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
struct heapNode {
    T value;
};

template <typename T>
class MaxHeap {
private:
    heapNode<T>* heapTable;   
    size_t currentSize;       
    size_t maxSize;        

    void resize();
    void heapifyUp(int index);
    void heapifyDown(int index);
    //void strIn-Order(std::string& result, int index, int depth) const;

public:
    MaxHeap();
    ~MaxHeap();

    int getHeight() const;
    void add(const T& value);
    T extractMax();
    void clear();
    std::string to_string() const;
    void heapifyUpRecursive(int index);
    void heapifyDownRecursive(int index);
    size_t size() const;
    size_t capacity() const;
};

void setConsoleTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

template <typename T>
MaxHeap<T>::MaxHeap() : currentSize(0), maxSize(1) {
    heapTable = new heapNode<T>[maxSize];
}

template <typename T>
MaxHeap<T>::~MaxHeap() {
    delete[] heapTable;
}

template <typename T>
void MaxHeap<T>::resize() {
    maxSize *= 2;
    heapNode<T>* newHeapTable = new heapNode<T>[maxSize];
    for (size_t i = 0; i < currentSize; ++i) {
        newHeapTable[i] = heapTable[i];
    }
    delete[] heapTable;
    heapTable = newHeapTable;
}

template <typename T>
void MaxHeap<T>::add(const T& value) {
    for (size_t i = 0; i < currentSize; ++i) {
        if (heapTable[i].value == value) {
            //std::cout << value << " already exists" << std::endl;
            return;
        }
    }

    if (currentSize == maxSize) {
        resize(); 
    }

    heapTable[currentSize].value = value; 
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
    T maxValue = heapTable[0].value;
    heapTable[0] = heapTable[currentSize - 1];
    currentSize--;
    heapifyDown(0);
    return maxValue;
}

template <typename T>
void MaxHeap<T>::clear() {
    delete[] heapTable;
    maxSize = 1;
    currentSize = 0;
    heapTable = new heapNode<T>[maxSize];
}

template <typename T>
std::string MaxHeap<T>::to_string() const {
    std::string result;  
    result += "maxSize: " + std::to_string(maxSize) +"\n" + "currentSize: " + std::to_string(currentSize) + "\n\n";
    //strIn-Order(result, 0, 0); 

    size_t elementsInLevel = 1;  
    size_t i = 0;

    while (i < currentSize) {
        for (size_t j = 0; j < elementsInLevel && i < currentSize; ++j, ++i) {
            result += std::to_string(heapTable[i].value) + " ";
        }
        result += "\n\n";
        elementsInLevel *= 2;
    }

    return result;
}

/*
template <typename T>
void MaxHeap<T>::strIn-Order(std::string& result, int index, int depth) const {
    if (index >= currentSize) return;  

    strIn-Order(result, index * 2 + 1, depth + 1);

    for (int i = 0; i < getHeight() - depth; ++i) {
        result += "|\t";
    }

    result += std::to_string(heapTable[index].value) + "\n";

    strIn-Order(result, index * 2 + 2, depth + 1);
}
*/

template <typename T>
void MaxHeap<T>::heapifyUp(int index) {
    int parentIndex = (index - 1) / 2;
    if (index > 0 && heapTable[index].value > heapTable[parentIndex].value) {
        std::swap(heapTable[index], heapTable[parentIndex]);
        heapifyUp(parentIndex);
    }
}

template <typename T>
void MaxHeap<T>::heapifyDown(int index) {
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int largest = index;

    if (leftChild < currentSize && heapTable[leftChild].value > heapTable[largest].value) {
        largest = leftChild;
    }
    if (rightChild < currentSize && heapTable[rightChild].value > heapTable[largest].value) {
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
    if (heapTable[index].value > heapTable[parentIndex].value) {
        std::swap(heapTable[index], heapTable[parentIndex]);
        heapifyUpRecursive(parentIndex);
    }
}

template <typename T>
void MaxHeap<T>::heapifyDownRecursive(int index) {
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int largest = index;

    if (leftChild < currentSize && heapTable[leftChild].value > heapTable[largest].value) {
        largest = leftChild;
    }
    if (rightChild < currentSize && heapTable[rightChild].value > heapTable[largest].value) {
        largest = rightChild;
    }

    if (largest != index) {
        std::swap(heapTable[index], heapTable[largest]);
        heapifyDownRecursive(largest);
    }
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

int getRandomNumber(int min, int max) {
    return min + rand() % ((max - min) + 1);
}

int main()
{
    MaxHeap<int> maxHeap;

    std::vector<int> test_data(10000);  

    for (int i = 0; i < 10000; i += 1) 
        test_data[i] = getRandomNumber(-10000, 10000);

    measureExecutionTime([&]() {
            for (int i = 0; i < 10000; i += 1)  
            {
                maxHeap.add(test_data[i]);
            }
        });

    std::cout << maxHeap.to_string();

    return 0;
}
