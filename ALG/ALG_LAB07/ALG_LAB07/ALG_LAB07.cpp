#include "MaxHeap.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <vector>
#include <cmath> 
#include <algorithm>

template <typename T>
T adjustForNegativeValues(T* arr, size_t size)
{
	T minNegative = *std::min_element(arr, arr + size);

	if (minNegative < 0)
	{
		T offset = -minNegative;
		for (size_t i = 0; i < size; ++i)
		{
			arr[i] += offset; 
		}
		return offset;
	}
	return 0;
}

template <typename T>
T normalizeToUnitRange(T* arr, size_t size)
{
	T maxElement = *std::max_element(arr, arr + size);

	if (maxElement > 0)
	{
		int exponent = static_cast<int>(std::log10(maxElement));
		T divisor = std::pow(10, exponent);

		for (size_t i = 0; i < size; ++i)
		{
			arr[i] /= divisor; 
		}
		return divisor;
	}
	return 1; 
}

template<typename T>
T getRandomValue(T min, T max) {
	if (min > max) {
		std::swap(min, max);
	}

	if constexpr (std::is_integral<T>::value) {
		return min + rand() % (static_cast<int>(max - min + 1));
	}
	else if constexpr (std::is_floating_point<T>::value) {
		double scale = (rand() / static_cast<double>(RAND_MAX));
		return min + scale * (max - min);
	}
	else {
		static_assert(std::is_arithmetic<T>::value, "Unsupported type");
	}
}

template <typename T>
bool areArraysEqual(const T* arr1, const T* arr2, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		if (arr1[i] != arr2[i]) {
			return false;
		}
	}
	return true;
}

template <typename T>
void arrayToString(const T* arr, size_t size)
{
	std::cout << "\n[";
	for (size_t i = 0; i < size; ++i)
	{
		std::cout << arr[i];
		if (i < size - 1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl;
}

void countingSort(int* arr, size_t size, int m) {
	std::vector<int> count(m, 0);

	for (size_t i = 0; i < size; ++i) {
		count[arr[i]]++;
	}

	size_t index = 0;
	for (int i = 0; i < m; ++i) {
		while (count[i] > 0) {
			arr[index++] = i;
			count[i]--;
		}
	}
}

void bucketSort(int* array, size_t n, int m) {
	std::vector<std::vector<int>> buckets(m);

	for (size_t i = 0; i < n; ++i) {
		int bucketIndex = min(array[i] * m / (m + 1), m - 1);
		buckets[bucketIndex].push_back(array[i]);
	}

	for (int i = 0; i < m; ++i) {
		std::sort(buckets[i].begin(), buckets[i].end());
	}

	size_t index = 0;
	for (int i = 0; i < m; ++i) {
		for (const int value : buckets[i]) {
			array[index++] = value;
		}
	}
}

template <typename T, typename KeyGetter, typename Comparator>
void bucketSort(T* array, size_t n, int m, KeyGetter getKey, Comparator comparator) {
	std::vector<std::vector<T>> buckets(m);

	for (size_t i = 0; i < n; ++i) {
		double key = getKey(array[i]);
		int bucketIndex = min(static_cast<int>(key * m), m - 1);
		buckets[bucketIndex].push_back(array[i]);
	}

	for (int i = 0; i < m; ++i) {
		std::sort(buckets[i].begin(), buckets[i].end(), comparator);
	}

	size_t index = 0;
	for (int i = 0; i < m; ++i) {
		for (const T& value : buckets[i]) {
			array[index++] = value;
		}
	}
}

struct Point {
	double x, y;
};

int main() {

	srand(static_cast<unsigned>(time(nullptr)));

	size_t size = 10000000;
	int m = 100;

	int* array = new int[size];

	for (size_t i = 0; i < size; ++i) {
		array[i] = getRandomValue(0, (int)size);
	}

	MaxHeap<int> heap(array, size, true);

	//std::cout << "Heap after construction:\n" << heap.to_string();

	measureExecutionTime([&](){heap.sort();});

	//arrayToString(array, size);

	delete[] array;

	std::cout << "\n===================================================================================\n" << std::endl;

	int* array1 = new int[size];
	for (size_t i = 0; i < size; ++i) {
		array1[i] = getRandomValue(0, (int)size);
	}

	int* arrCopy = new int[size];
	memcpy(arrCopy, array1, size * sizeof(int));

	measureExecutionTime([&]() {countingSort(array1, size, (int)size); });

	//arrayToString(array1, 100);
	std::cout << "Original vs Sorted: " << (areArraysEqual(arrCopy, array1, size) ? "Equal" : "Not Equal") << std::endl;
	delete[] arrCopy;
	delete[] array1;

	std::cout << "\n===================================================================================\n" << std::endl; //optymalna liczba kubelkow [sqrt(n) ; n/2]

	int* array2 = new int[size];
	for (size_t i = 0; i < size; ++i) {
		array2[i] = getRandomValue(0, 30);
	}

	arrCopy = new int[size];
	memcpy(arrCopy, array2, size * sizeof(int));

	measureExecutionTime([&]() {bucketSort(array2, size, (int)(size/m)); });

	//arrayToString(array2, size);
	std::cout << "Original vs Sorted: " << (areArraysEqual(arrCopy, array2, size) ? "Equal" : "Not Equal") << std::endl;
	delete[] arrCopy;
	delete[] array2;

	std::cout << "\n===================================================================================\n" << std::endl;

	Point* points = new Point[size];
	for (size_t i = 0; i < size; ++i) {
		points[i].x = getRandomValue<double>(0, 1);//liczba kombinacj (teoretyczne) 205 891 132 094 649
		points[i].y = getRandomValue<double>(0, 100);
	}

	auto getKey = [](const Point& p) { return p.x; };
	auto comparator = [](const Point& p1, const Point& p2) { return p1.x < p2.x; };

	measureExecutionTime([&]() {bucketSort(points, size, (int)(size / m), getKey, comparator); });

	//for (size_t i = 0; i < 1000; ++i) {	std::cout << "(" << points[i].x << ", " << points[i].y << ") ";}

	delete[] points;

	return 0;
}
