#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

// Вычисление количества контрольных битов
int calcRedundantBits(int m) {

    return 0;
}

// Вставка контрольных битов (нулей) на позиции степеней двойки
std::vector<int> posRedundantBits(const std::vector<int>& data, int r) {
    int j = 0, k = 1;
    int m = data.size();
    std::vector<int> res;

    for (int i = 1; i <= m + r; ++i) {
        if (i == (1 << j)) {
            res.push_back(0);
            j++;
        }
        else {
            res.push_back(data[m - k]); // вставляем биты с конца
            k++;
        }
    }

    std::reverse(res.begin(), res.end()); // приведение к правильному порядку
    return res;
}

// Вычисление значений контрольных битов
std::vector<int> calcParityBits(std::vector<int> arr, int r) {
    int n = arr.size();

    for (int i = 0; i < r; ++i) {
        int val = 0;
        for (int j = 1; j <= n; ++j) {
            if ((j & (1 << i)) == (1 << i)) {
                val ^= arr[n - j];
            }
        }
        arr[n - (1 << i)] = val;
    }

    return arr;
}

// Выявление позиции ошибки
int detectError(const std::vector<int>& arr, int r) {
    int n = arr.size();
    int res = 0;

    for (int i = 0; i < r; ++i) {
        int val = 0;
        for (int j = 1; j <= n; ++j) {
            if ((j & (1 << i)) == (1 << i)) {
                val ^= arr[n - j];
            }
        }
        res += val * pow(10, i);
    }

    // Преобразование бинарного числа в десятичное
    int decimal = 0, base = 1, temp = res;
    while (temp > 0) {
        int last_digit = temp % 10;
        temp /= 10;
        decimal += last_digit * base;
        base *= 2;
    }

    return decimal;
}

std::vector<int> hammingEncode(const std::vector<int>& data) {
    int m = data.size();
    int r = calcRedundantBits(m);

    std::vector<int> arranged = posRedundantBits(data, r);
    std::vector<int> encoded = calcParityBits(arranged, r);

    return encoded;
}

std::vector<int> hammingDecode(const std::vector<int>& encoded) {
    int r = calcRedundantBits(encoded.size() - calcRedundantBits(encoded.size())); // можно упростить, если знаем r заранее

    int error_pos = detectError(encoded, r);
    std::vector<int> corrected = encoded;

    if (error_pos > 0 && error_pos <= encoded.size()) {
        corrected[corrected.size() - error_pos] ^= 1; // исправляем бит
    }

    // Удаляем контрольные биты (позиции 1, 2, 4, 8 и т.д.)
    std::vector<int> decoded;
    for (int i = 1; i <= corrected.size(); ++i) {
        if ((i & (i - 1)) != 0) { // если не степень двойки
            decoded.push_back(corrected[corrected.size() - i]);
        }
    }

    std::reverse(decoded.begin(), decoded.end()); // возвращаем в прямой порядок
    return decoded;
}

int main() {
    std::vector<int> original = { 1,0,1,1,0,1,0,1,1,1,0,0,1,0,1,0,0,1,1,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,0,1,1,0,0,1,0,1,0,0,1,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,0,1,1,0,1,0,1,1,0,1,0,1,0,0,1,1,1,0,1,0,0,1,0,1,0,1,0,1 };
    std::vector<int> encoded = hammingEncode(original);

    for (int b : original) std::cout << b;
    std::cout << "\n";

    std::vector<int> decoded = hammingDecode(encoded);

    for (int b : decoded) std::cout << b;

    return 0;
}
