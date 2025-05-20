#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <functional>
#include <vector>
#include <ctime>


//=============================================================================================================================================
template <typename Func>
void measureExecutionTime(Func func)
{
    std::clock_t start = std::clock();
    func();
    std::clock_t end = std::clock();
    double duration = double(static_cast<int64_t>(end) - static_cast<int64_t>(start)) / (CLOCKS_PER_SEC / 1000);
    std::cout << "\nExecution time: " << duration << " milliseconds\n" << std::endl;
};

// Структура для хранения пары "ключ-значение".
template <typename Value>
struct KeyValuePair
{
    std::string key;
    Value value;
};

// Класс двусвязного списка, используемый в качестве хранилища для каждого ведра.
template<typename T>
class lista_dwukierunkowa
{
public:
    struct wezel_listy
    {
        T dane;
        wezel_listy* nast;
        wezel_listy* poprz;

        wezel_listy(const T& dane, wezel_listy* nast = nullptr, wezel_listy* poprz = nullptr)
            : dane(dane), nast(nast), poprz(poprz)
        {
        }
    };

    wezel_listy* glowa;
    wezel_listy* ogon;
    unsigned dlugosc;

    lista_dwukierunkowa() noexcept : glowa(nullptr), ogon(nullptr), dlugosc(0) {}

    ~lista_dwukierunkowa()
    {
        czyszczenie();
    }

    unsigned pobierz_dlugosc() const noexcept
    {
        return dlugosc;
    }

    void wstaw_na_koniec(const T& dane)
    {
        wezel_listy* nowy_wezel = new wezel_listy(dane, nullptr, ogon);
        if (ogon)
            ogon->nast = nowy_wezel;
        else
            glowa = nowy_wezel;
        ogon = nowy_wezel;
        ++dlugosc;
    }

    wezel_listy* znajdz_element(const std::string& key) const
    {
        for (auto p = glowa; p; p = p->nast)
            if (p->dane.key == key)
                return p;
        return nullptr;
    }

    bool usun_element(const std::string& key)
    {
        wezel_listy* do_usuniecia = znajdz_element(key);
        if (!do_usuniecia) return false;

        if (do_usuniecia == glowa) 
        {
            glowa = glowa->nast;
            if (glowa)
                glowa->poprz = nullptr;
        }
        else if (do_usuniecia == ogon)
        {
            ogon = ogon->poprz;
            if (ogon)
                ogon->nast = nullptr;
        }
        else
        {
            do_usuniecia->poprz->nast = do_usuniecia->nast;
            do_usuniecia->nast->poprz = do_usuniecia->poprz;
        }

        delete do_usuniecia;
        --dlugosc;
        return true;
    }

    void czyszczenie()
    {
        while (glowa)
        {
            wezel_listy* temp = glowa;
            glowa = glowa->nast;
            delete temp;
        }
        ogon = nullptr;
        dlugosc = 0;
    }
};

// Класс хеш-таблицы, использующий список в каждой ячейке.
template <typename Value>
class HashTable
{
private:
    size_t capacity;
    size_t size;
    lista_dwukierunkowa<KeyValuePair<Value>>* table; // Таблица с двусвязными списками.

    size_t hashFunction(const std::string& key) const
    {
        size_t hash = 0;
        size_t base = 31;
        for (char ch : key)
        {
            hash = (hash * base + ch) % capacity;
        }
        return hash;
    }

    double loadFactor() const
    {
        return static_cast<double>(size) / capacity;
    }

    void resize(size_t new_capacity)
{
    measureExecutionTime([&](){
        int previous_capacity = capacity;
        capacity = new_capacity;

        auto* new_table = new lista_dwukierunkowa<KeyValuePair<Value>>[new_capacity];


        for (size_t i = 0; i < previous_capacity; ++i)
        {
            lista_dwukierunkowa<KeyValuePair<Value>>& current_list = table[i];  // Явное указание типа

            for (auto* p = current_list.glowa; p; p = p->nast)
            {
                size_t new_index = hashFunction(p->dane.key);
                new_table[new_index].wstaw_na_koniec(p->dane);
            }
        }

        delete[] table;
        table = new_table;
    });
    
}


public:
    HashTable(size_t initial_capacity = 10): capacity(initial_capacity), size(0)
    {
        table = new lista_dwukierunkowa<KeyValuePair<Value>>[capacity];
    }

    ~HashTable()
    {
        delete[] table;
    }

    size_t get_size() const
    {
        return size;
    }

    void add(const std::string& key, const Value& value)
    {
        size_t index = hashFunction(key);

        // Ищем, есть ли уже такой ключ в списке.
        auto* list = &table[index];
        auto* found = list->znajdz_element(key);

        if (found)
        {
            found->dane.value = value; // Если нашли, обновляем значение.
        }
        else
        {
            list->wstaw_na_koniec(KeyValuePair<Value>{key, value});
            ++size;

            if (loadFactor() > 0.75)
            {
                resize(capacity * 2);
            }
        }
    }

    KeyValuePair<Value>* get(const std::string& key) const
    {
        size_t index = hashFunction(key);
        auto* list = &table[index];
        auto* found = list->znajdz_element(key);

        return found ? &found->dane : nullptr;
    }

    bool remove(const std::string& key)
    {
        size_t index = hashFunction(key);
        auto* list = &table[index];

        bool removed = list->usun_element(key);
        if (removed)
        {
            --size;
        }
        return removed;
    }

    void clear()
    {
        delete[] table;
        table = new lista_dwukierunkowa<KeyValuePair<Value>>[capacity];
        size = 0;
    }

    void print() const
    {
        for (size_t i = 0; i < capacity; ++i)
        {
            const auto& list = table[i];
            if (list.pobierz_dlugosc() > 0)
            {
                std::cout << "Bucket " << i << ": ";
                for (auto* p = list.glowa; p; p = p->nast)
                {
                    std::cout << "(" << p->dane.key << ", " << p->dane.value << ") ";
                }
                std::cout << "\n";
            }
        }
    }

    void print_stats() const
    {
        size_t non_empty_buckets = 0;
        size_t min_size = SIZE_MAX;
        size_t max_size = 0;
        size_t total_size = 0;

        for (size_t i = 0; i < capacity; ++i)
        {
            size_t bucket_size = table[i].pobierz_dlugosc();
            if (bucket_size > 0)
            {
                ++non_empty_buckets;
                min_size = std::min(min_size, bucket_size);
                max_size = std::max(max_size, bucket_size);
                total_size += bucket_size;
            }
        }

        double avg_size = non_empty_buckets > 0 ? static_cast<double>(total_size) / non_empty_buckets : 0.0;

        std::cout << "Statistics:\n";
        std::cout << "Non-empty buckets: " << non_empty_buckets << "\n";
        std::cout << "Min bucket size: " << (non_empty_buckets > 0 ? min_size : 0) << "\n";
        std::cout << "Max bucket size: " << max_size << "\n";
        std::cout << "Avg bucket size: " << avg_size << "\n";
    }
};


// Generacja klucza wypadkowego dla podanej dlugosci.
std::string random_key(size_t length)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    result.resize(length);

    for (size_t i = 0; i < length; ++i)
    {
        result[i] = charset[rand() % (sizeof(charset) - 1)];
    }

    return result;
}

int main()
{
    const int stopien_maks = 7;
    HashTable<int> hash_table;

    for (int o = 1; o <= stopien_maks; ++o)
    {
        const int n = std::pow(10, o);

        std::vector<std::string> keys(n);
        for (int i = 0; i < n; ++i)
        {
            keys[i] = random_key(6);
        }

        clock_t t1 = clock();
        for (int i = 0; i < n; ++i)
        {
            hash_table.add(keys[i], i);
        }
        clock_t t2 = clock();

        std::cout << "Stopien: " << o << "\n";
        std::cout << "Czas wstawiania " << n << " elementow: " << double(t2 - t1) / CLOCKS_PER_SEC << " seconds\n";

        hash_table.print_stats();
        hash_table.clear();
    }

    return 0;
}
