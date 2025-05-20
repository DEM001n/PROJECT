#pragma once
#include "LinkedList.h"
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>
#include <chrono>
using namespace std;
#pragma execution_character_set( "utf-8" )

// ===================================================================
// Struktura dla par klucz-wartość
template <typename Kt, typename Vt>
struct Pair {
    Kt key;
    Vt value;

    Pair(Kt key, Vt value)
        : key(key), value(value) {}
};

template <typename Kt, typename Vt>
class HashTable {
    int size, capacity;
    Linked_list<Pair<Kt, Vt>*>** array;

    void overwrite_array() {
        Linked_list<Pair<Kt, Vt>*>** temp_array = new Linked_list<Pair<Kt, Vt>*>*[capacity];
        initialize_lists(temp_array);

        for (int i = 0; i < capacity / 2; i++) {
            if (array[i]->size == 0)
                continue;

            Node<Pair<Kt, Vt>*>* current = array[i]->Head;
            while (current != nullptr) {
                put_helper(current->data->key, current->data->value, temp_array);
                current = current->next;
            }
        }
        clear_array(array, capacity / 2);
        array = temp_array;
    }

    void put_helper(Kt key, Vt value, Linked_list<Pair<Kt, Vt>*>** arr) {
        int hashed_key = hash_key(key);
        Linked_list<Pair<Kt, Vt>*>* found_list = arr[hashed_key];

        Node<Pair<Kt, Vt>*>* current = found_list->Head;
        while (current != nullptr) {
            if (current->data->key == key) {
                current->data->value = value;
                return;
            }
            current = current->next;
        }
        found_list->add_behind(new Pair<Kt, Vt>(key, value));
        size++;
    }

public:
    HashTable() : size(0), capacity(8), array(new Linked_list<Pair<Kt, Vt>*>* [capacity]) {
        initialize_lists(array);
    }

    ~HashTable() {
        clear();
        clear_array(array, capacity);
    }

    void put(Kt key, Vt value) {
        if (size >= round(capacity * 0.75)) {
            capacity *= 2;
            overwrite_array();
        }
        put_helper(key, value, array);
    }

    Pair<Kt, Vt>* get(Kt key) {
        int hashed_key = hash_key(key);
        Linked_list<Pair<Kt, Vt>*>* found_list = array[hashed_key];

        Node<Pair<Kt, Vt>*>* current = found_list->Head;
        while (current != nullptr) {
            if (current->data->key == key)
                return current->data;
            current = current->next;
        }
        return nullptr;
    }

    bool remove(Kt key) {
        int hashed_key = hash_key(key);
        Linked_list<Pair<Kt, Vt>*>* found_list = array[hashed_key];

        Node<Pair<Kt, Vt>*>* current = found_list->Head;
        while (current != nullptr) {
            if (current->data->key == key) {
                if (current->previous != nullptr) current->previous->next = current->next;
                if (current->next != nullptr) current->next->previous = current->previous;
                if (current == found_list->Head) found_list->Head = current->next;
                if (current == found_list->Tail) found_list->Tail = current->previous;

                delete current->data;
                delete current;
                size--;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void clear() {
        for (int i = 0; i < capacity; i++) {
            array[i]->clear();
        }
        size = 0;
    }

    string to_string(string(*get_elem_data)(Linked_list<Pair<Kt, Vt>*>*)) {
        ostringstream array_string;
        array_string << left << setw(25)
            << "┃Table size" << "┃" << setw(22) << size << "┃\n" << setw(25)
            << "┃Table capacity" << "┃" << setw(22) << capacity << "┃\n"
            << "┗━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━┛\n";

        if (size == 0)
            return array_string.str();

        array_string << "{";
        for (int i = 0; i < min(capacity, 10); i++)
            array_string << "\n\t" << i << ": " << get_elem_data(array[i]);

        array_string << "\n}";
        return array_string.str();
    }

    void print_stats() {
        cout << "\nstats:\n"
            << "\tlist min size: " << get_min_size() << "\n"
            << "\tlist max size: " << get_max_size() << "\n"
            << "\tnot-null lists: " << get_not_null_lists() << "\n"
            << "\tlist avg size: " << get_list_avg_size() << endl;
    }

private:
    int hash_key(const string& key) {
        const int length = static_cast<int>(key.length());
        int pre_hashing_sum = 0;

        for (int i = 0; i < length; i++) {
            pre_hashing_sum += static_cast<int>(key[i]) * pow(31, length - i - 1);
        }

        return mod(pre_hashing_sum, capacity);
    }

    int mod(const int x, const int m) {
        return (x % m + m) % m;
    }

    void initialize_lists(Linked_list<Pair<Kt, Vt>*>** arr) {
        for (int i = 0; i < capacity; i++) {
            arr[i] = new Linked_list<Pair<Kt, Vt>*>();
        }
    }

    void clear_array(Linked_list<Pair<Kt, Vt>*>** arr, int cap) {
        for (int i = 0; i < cap; i++) {
            delete arr[i];
        }
        delete[] arr;
    }

    size_t get_min_size() {
        size_t min_size = get_max_size();
        size_t temp_size = 0;

        for (int i = 0; i < capacity; i++) {
            temp_size = array[i]->size;
            if (temp_size < min_size && temp_size > 0)
                min_size = temp_size;
        }
        return min_size;
    }

    size_t get_max_size() {
        size_t max_size = 0;
        size_t temp_size = 0;

        for (int i = 0; i < capacity; i++) {
            temp_size = array[i]->size;
            if (temp_size > max_size)
                max_size = temp_size;
        }
        return max_size;
    }

    int get_not_null_lists() {
        int not_null_lists = 0;

        for (int i = 0; i < capacity; i++) {
            if (array[i]->size > 0)
                not_null_lists++;
        }
        return not_null_lists;
    }

    double get_list_avg_size() {
        size_t list_avg_size = 0;
        double non_empty_lists = 0;

        for (int i = 0; i < capacity; i++) {
            if (array[i]->size > 0) {
                list_avg_size += array[i]->size;
                non_empty_lists++;
            }
        }
        if (non_empty_lists > 0)
            return list_avg_size / non_empty_lists;

        return 0;
    }
};

string linked_list_to_str(Linked_list<Pair<string, int>*>* list) {
    string string_repr = "";

    Node<Pair<string, int>*>* current = list->Head;
    while (current) {
        string_repr += current->data->key + " -> " + std::to_string(current->data->value);

        if (current->next != nullptr) string_repr += ",";
        else string_repr += ";";

        current = current->next;
    }
    return string_repr;
}

string random_key(const int length) {
    static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    string key;
    key.reserve(length);

    for (int i = 0; i < length; i++)
        key += alphabet[rand() % (sizeof(alphabet) - 1)];

    return key;
}

void print_execution_time(const chrono::high_resolution_clock::time_point& t1, const chrono::high_resolution_clock::time_point& t2) {
    const double exec_time = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
    cout << "┏━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━┓\n" << setw(25) << left
        << "┃Execution time" << "┃" << setw(19) << left << exec_time << " ms┃\n";
}

int main() {
    SetConsoleOutputCP(65001);
    srand(static_cast<unsigned>(time(nullptr)));

    constexpr int MAX_ORDER = 7;
    HashTable<string, int> ht;

    for (int o = 1; o <= MAX_ORDER; o++) {
        const int n = static_cast<int>(pow(10, o));

        auto t1 = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++)
            ht.put(random_key(6), i); // dodawanie elementu
        auto t2 = chrono::high_resolution_clock::now();

        print_execution_time(t1, t2);
        cout << ht.to_string(linked_list_to_str) << endl;

        const int m = static_cast<int>(pow(10, 4));
        int hits = 0;

        t1 = chrono::high_resolution_clock::now();
        for (int i = 0; i < m; i++) {
            Pair<string, int>* entry = ht.get(random_key(6));
            if (entry != nullptr)
                hits++;
        }
        t2 = chrono::high_resolution_clock::now();

        cout << "Searching time: " << chrono::duration_cast<chrono::milliseconds>(t2 - t1).count() << " ms\n";
        ht.print_stats();

        cout << "Hits: " << hits << endl;
        ht.clear();
    }
    return 0;
}
