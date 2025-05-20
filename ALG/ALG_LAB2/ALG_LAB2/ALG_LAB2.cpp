#include "DynamicArray.h"

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