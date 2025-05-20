#include "Kopiec.h"

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
