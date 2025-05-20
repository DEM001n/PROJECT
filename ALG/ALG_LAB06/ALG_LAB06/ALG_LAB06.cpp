#include "MaxHeap.h"
#include <vector>  

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
