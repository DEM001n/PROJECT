//ALGO2 IS1 210B LAB02
//Dmytro Mahaliuk
//md55722@zut.edu.pl

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

int main() {

    DynamicArray<std::string> dynamicArray;

    int n = 1000;
    int m = 100000;

    double tmp_time = measureExecutionTime([&]() {
        for (size_t i = 0; i <= n; ++i) {
            dynamicArray.add("Element " + std::to_string(i));
        }
        });

    double step_time = (double)(tmp_time / n);
    double predict_time = step_time * m;

    std::cout << "Step time: " << step_time << " milliseconds" << std::endl;
    std::cout << "Predict time: " << predict_time << " milliseconds" << std::endl;

    dynamicArray.clear();

    tmp_time = measureExecutionTime([&]() {
        for (size_t i = 0; i < m; ++i) {
            dynamicArray.add("Element " + std::to_string(i));
        }
        });

    std::cout << "Execution time: " << tmp_time << " milliseconds" << std::endl;

    std::cout << dynamicArray.to_string();

    return 0;
}