//ALGO2 IS1 210B LAB05
//Dmytro Mahaliuk
//md55722@zut.edu.pl

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


void setConsoleTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

template <typename V>
HashTable<V>::HashTable(size_t initialSize, double loadFactor)
    : currentSize(0), maxSize(initialSize), loadFactor(loadFactor) {
    table = new HashNode<V>*[maxSize];
    for (size_t i = 0; i < maxSize; ++i) {
        table[i] = nullptr;
    }
}

template <typename V>
HashTable<V>::~HashTable() {
    clear();
    delete[] table;
}

template <typename V>
size_t HashTable<V>::hashFunction(const std::string& key) const {
    size_t hash = 0;
    size_t power = 1;
    for (char ch : key) {
        hash = (hash + (ch * power) % maxSize) % maxSize;
        power = (power * 31) % maxSize;
    }
    return hash;
}

template <typename V>
void HashTable<V>::rehash() {
    size_t oldMaxSize = maxSize;
    maxSize *= 2;
    HashNode<V>** newTable = new HashNode<V>*[maxSize];
    for (size_t i = 0; i < maxSize; ++i) {
        newTable[i] = nullptr;
    }

    for (size_t i = 0; i < oldMaxSize; ++i) {
        if (table[i] != nullptr) {
            for (size_t j = 0; j < table[i]->keys.size(); ++j) {
                size_t newIndex = hashFunction(table[i]->keys[j]);
                if (newTable[newIndex] == nullptr) {
                    newTable[newIndex] = new HashNode<V>();
                }
                newTable[newIndex]->keys.push_back(table[i]->keys[j]);
                newTable[newIndex]->values.push_back(table[i]->values[j]);
            }
            delete table[i];
        }
    }

    delete[] table;
    table = newTable;
}

template <typename V>
void HashTable<V>::insert(const std::string& key, const V& value) {
    if (static_cast<double>(currentSize + 1) / maxSize > loadFactor) {
        rehash();
    }

    size_t index = hashFunction(key);

    if (table[index] == nullptr) {
        table[index] = new HashNode<V>();
    }

    auto& keys = table[index]->keys;
    auto& values = table[index]->values;

    for (size_t i = 0; i < keys.size(); ++i) {
        if (keys[i] == key) {
            values[i] = value;
            return;
        }
    }

    keys.push_back(key);
    values.push_back(value);
    ++currentSize;
}

template <typename V>
V* HashTable<V>::find(const std::string& key) const {
    size_t index = hashFunction(key);

    if (table[index] == nullptr) return nullptr;

    auto& keys = table[index]->keys;
    auto& values = table[index]->values;

    for (size_t i = 0; i < keys.size(); ++i) {
        if (keys[i] == key) {
            return &values[i];
        }
    }

    return nullptr;
}

template <typename V>
bool HashTable<V>::remove(const std::string& key) {
    size_t index = hashFunction(key);

    if (table[index] == nullptr) return false;

    auto& keys = table[index]->keys;
    auto& values = table[index]->values;

    for (size_t i = 0; i < keys.size(); ++i) {
        if (keys[i] == key) {
            keys.erase(keys.begin() + i);
            values.erase(values.begin() + i);
            --currentSize;

            if (keys.empty()) {
                delete table[index];
                table[index] = nullptr;
            }

            return true;
        }
    }

    return false;
}

template <typename V>
void HashTable<V>::clear() {
    for (size_t i = 0; i < maxSize; ++i) {
        delete table[i];
        table[i] = nullptr;
    }
    currentSize = 0;
}

template <typename V>
std::string HashTable<V>::toString() const {
    std::ostringstream oss;
    oss << "HashTable (maxSize=" << maxSize << "):\n";
    for (size_t i = 0; i < maxSize; ++i) {
        oss << "[" << i << ":";
        if (table[i] != nullptr) {
            for (size_t j = 0; j < table[i]->keys.size(); ++j) {
                oss << "(" << table[i]->keys[j] << "," << table[i]->values[j] << ")";
                if (j != table[i]->keys.size() - 1) oss << ", ";
            }
        }
        else {
            oss << "NULL";
        }
        oss << "]\t";
    }
    return oss.str();
}

template <typename V>
void HashTable<V>::statistics() const {
    size_t nonEmptyBuckets = 0;
    size_t totalElements = 0;

    for (size_t i = 0; i < maxSize; ++i) {
        if (table[i] != nullptr) {
            ++nonEmptyBuckets;
            totalElements += table[i]->keys.size();
        }
    }

    double averageNonEmpty = nonEmptyBuckets > 0 ? static_cast<double>(totalElements) / nonEmptyBuckets : 0.0;

    setConsoleTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "Total buckets: " << maxSize << std::endl;
    std::cout << "Non-empty buckets: " << nonEmptyBuckets << std::endl;
    std::cout << "Total elements: " << totalElements << std::endl;
    std::cout << "Average elements per non-empty bucket: " << std::fixed << std::setprecision(2) << averageNonEmpty << std::endl;
    std::cout << "Load factor: " << static_cast<double>(currentSize) / maxSize << std::endl;
    setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

template class HashTable<int>;
template class HashTable<char>;
template class HashTable<std::string>;


int main() {
    HashTable<int> table;

    measureExecutionTime([&]()
        {
            for (int i = 1; i <= 100000; i+=2)
            {
                int a = i;
                std::string generatedKey;

                while (a > 0) {
                    int asciiValue = (a % 10) + 65; 
                    generatedKey += static_cast<char>(asciiValue);
                    a = static_cast<int>(a/10);
                }
                std::reverse(generatedKey.begin(), generatedKey.end());
                table.insert(generatedKey, i);
            }
        });

    std::cout << table.toString() << std::endl;

    measureExecutionTime([&]()
        {
            for (int i = 1; i <= 100000; i = i + 7)
            {
                int a = i;
                std::string generatedKey;

                while (a > 0) {
                    int asciiValue = (a % 10) + 65;
                    generatedKey += static_cast<char>(asciiValue);
                    a = static_cast<int>(a / 10);
                }
                /*
                std::reverse(generatedKey.begin(), generatedKey.end());
                if (table.find(generatedKey))
                    std::cout << generatedKey << " found: " << table.find(generatedKey)->value << std::endl;
                else
                    std::cout << generatedKey << " not found" << std::endl;
                */
            }
        });

    measureExecutionTime([&]()
        {
            for (int i = 1; i <= 100000; i = i + 7)
            {
                int a = i;
                std::string generatedKey;

                while (a > 0) {
                    int asciiValue = (a % 10) + 65;
                    generatedKey += static_cast<char>(asciiValue);
                    a = static_cast<int>(a / 10);
                }
                /*
                std::reverse(generatedKey.begin(), generatedKey.end());
                if (table.remove(generatedKey))
                    std::cout << generatedKey << " removed" << std::endl;
                else
                    std::cout << generatedKey << " not removed" << std::endl;
                */
            }
        });

    table.statistics();

    measureExecutionTime([&]() {table.clear(); });
    
    return 0;
}
