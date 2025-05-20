#include<iostream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <functional>
#include <vector>
#include <ctime>
#include <sstream>  // Dodanie biblioteki do stringstream

// Implementacja struktury mieszajacej.
template <typename Key, typename Value>
struct KeyValuePair {
    Key key;
    Value value;
};

// Implementacja tablicy mieszajacej.
template <typename Key, typename Value>
class HashTable {
public:
    HashTable(size_t initial_capacity = 10)
        : capacity(initial_capacity), size(0)
    {
        table = new KeyValuePair<Key, Value>*[capacity]();
    }

    ~HashTable() {
        delete[] table;
    }

    // Zwraca ilosc elementow w tablicy.
    size_t get_size() const
    {
        return size;
    }

    // Dodaje do tablicy pary klucz-wartosc.
    void add(const Key& key, const Value& value)
    {
        size_t index = hashFunction(key);
        if (table[index] == nullptr) {
            table[index] = new KeyValuePair<Key, Value>{ key, value };
            ++size;
        }
        else if (table[index]->key == key) {
            table[index]->value = value;
            return;
        }

        // Zwiekszenia rozmiaru w przypadku przekroczenia rozmiaru.
        if (loadFactor() > 0.75) {
            resize(capacity * 2);
        }
    }

    // Wyszukiwanie elementu.
    Value get(const Key& key) const
    {
        size_t index = hashFunction(key);
        for (size_t i = 0; i < capacity; ++i) {
            if (table[index] != nullptr && table[index]->key == key) {
                return table[index]->value;
            }
            index = (index + 1) % capacity;
        }
        throw std::out_of_range("Klucz nie znaleziony");
    }

    // Nowa funkcja do wyszukiwania elementu.
    bool search(const Key& key, Value& value) const
    {
        size_t index = hashFunction(key);
        for (size_t i = 0; i < capacity; ++i) {
            if (table[index] != nullptr && table[index]->key == key) {
                value = table[index]->value;
                return true;  // Element znaleziony
            }
            index = (index + 1) % capacity;
        }
        return false;  // Element nie znaleziony
    }

    // Usuwa pary klucz-wartosc z tablicy.
    void remove(const Key& key)
    {
        size_t index = hashFunction(key);
        for (size_t i = 0; i < capacity; ++i) {
            if (table[index] != nullptr && table[index]->key == key) {
                delete table[index];
                table[index] = nullptr;
                --size;
                return;
            }
            index = (index + 1) % capacity;
        }
        throw std::out_of_range("Klucz nie znaleziony");
    }

    // Usuwa wszystkie elementy z tablicy.
    void clear()
    {
        for (size_t i = 0; i < capacity; ++i) {
            if (table[i] != nullptr) {
                delete table[i];
                table[i] = nullptr;
            }
        }
        size = 0;
    }

    // Wyswietla elementy z tablicy (do 10 pierwszych).
    void print() const
    {
        size_t print_limit = 10;
        for (size_t i = 0; i < capacity && i < print_limit; ++i) {
            if (table[i] != nullptr) {
                std::cout << i << ": " << table[i]->key << " -> " << table[i]->value << ";\n";
            }
        }
    }

    // Wyswietla statystyki tablicy.
    void print_stats() const
    {
        size_t non_empty_buckets = 0;
        size_t min_size = SIZE_MAX;
        size_t max_size = 0;
        size_t total_size = 0;

        for (size_t i = 0; i < capacity; ++i) {
            if (table[i] != nullptr) {
                ++non_empty_buckets;
                size_t bucket_size = 1;  // Każdy kubełek ma jedną parę klucz-wartość
                total_size += bucket_size;
                min_size = std::min(min_size, bucket_size);
                max_size = std::max(max_size, bucket_size);
            }
        }

        double avg_size = non_empty_buckets > 0 ? static_cast<double>(total_size) / non_empty_buckets : 0.0;

        std::cout << "Statystyka tablicy:\n";
        std::cout << "Niepuste kubelki: " << non_empty_buckets << "\n";
        std::cout << "Minimalny rozmiar kubelku: " << (non_empty_buckets > 0 ? min_size : 0) << "\n";
        std::cout << "Maksymalny rozmiar kubelku: " << max_size << "\n";
        std::cout << "Sredni rozmiar kubelku: " << avg_size << "\n";
    }

    // Zwraca napisową reprezentację tablicy.
    std::string to_string() const
    {
        std::ostringstream oss;
        for (size_t i = 0; i < capacity; ++i) {
            if (table[i] != nullptr) {
                oss << i << ": " << table[i]->key << " -> " << table[i]->value << "\n";
            }
        }
        return oss.str();
    }

private:
    size_t capacity;
    size_t size;
    KeyValuePair<Key, Value>** table;

    size_t hashFunction(const Key& key) const
    {
        size_t hash = 0;
        size_t base = 31;
        for (char ch : key) {
            hash = (hash * base + ch) % capacity;
        }
        return hash;
    }

    // Funkcja odpowiedzialna za obmierzanie napelnienia tablicy.
    double loadFactor() const
    {
        return static_cast<double>(size) / capacity;
    }

    // Funkcja zarzadzająca rozmiarem tablicy.
    void resize(size_t new_capacity)
    {
        auto* new_table = new KeyValuePair<Key, Value>*[new_capacity]();
        for (size_t i = 0; i < capacity; ++i) {
            if (table[i] != nullptr) {
                size_t index = hashFunction(table[i]->key) % new_capacity;
                if (new_table[index] == nullptr) {
                    new_table[index] = table[i];
                    continue;
                }
                else if (new_table[index]->key == table[i]->key) {
                    new_table[index]->value = table[i]->value;
                    continue;
                }
                new_table[index] = table[i];
            }
        }
        delete[] table;
        table = new_table;
        capacity = new_capacity;
    }
};

// Generacja klucza wypadkowego dla podanej dlugosci.
std::string random_key(size_t length)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    result.resize(length);

    for (size_t i = 0; i < length; ++i) {
        result[i] = charset[rand() % (sizeof(charset) - 1)];
    }

    return result;
}

int main() {
    const int stopien_maks = 7;
    HashTable<std::string, int> hash_table;

    for (int o = 1; o <= stopien_maks; ++o) {
        const int n = std::pow(10, o);

        // Dodanie elementow do hash tablicy.
        std::vector<std::string> keys(n);  // Korektowanie typu do std::string.
        for (int i = 0; i < n; ++i) {
            keys[i] = random_key(6);
        }

        // Obmierzania czasu wstawiania elementow.
        clock_t t1 = clock();
        for (int i = 0; i < n; ++i) {
            hash_table.add(keys[i], i);
        }
        clock_t t2 = clock();

        std::cout << "Stopien: " << o << "\n";
        std::cout << "Czas wstawiania " << n << " elementow: " << double(t2 - t1) / CLOCKS_PER_SEC << " seconds\n";

        // Wyszukiwanie elementu.
        std::string search_key = keys[n / 2];  // Przykład wyszukiwania środkowego klucza
        int search_value;
        if (hash_table.search(search_key, search_value)) {
            std::cout << "Znaleziono klucz: " << search_key << " z wartoscia: " << search_value << "\n";
        }
        else {
            std::cout << "Klucz " << search_key << " nie został znaleziony.\n";
        }

        // Wyświetlanie statystyk i zawartości tablicy.
        hash_table.print_stats();
        //std::cout << "Reprezentacja to_string tablicy:\n" << hash_table.to_string() << "\n";
        
        hash_table.clear();
    }

    return 0;
}
