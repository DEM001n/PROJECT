#pragma once
#include <iostream>
#include <stdexcept>
using namespace std;

// Struktura w�z�a listy wi�zanej
template <typename T>
struct Node {
    T data{};          // Dane w�z�a
    Node* next{};      // Wska�nik na kolejny w�ze�
    Node* previous{};  // Wska�nik na poprzedni w�ze�

    ~Node() {
        delete data;    // Usuwamy dane w�z�a (je�li s� wska�nikiem)
        data = nullptr; // Zapobiega niebezpiecznym wska�nikom
    }
};

// Klasa listy wi�zanej
template <typename T>
class Linked_list {
public:
    Node<T>* Head{};  // Wska�nik na pocz�tek listy
    Node<T>* Tail{};  // Wska�nik na koniec listy
    size_t size{};    // Rozmiar listy

    // Destruktor
    ~Linked_list() {
        clear();
    }

    // Dodanie elementu na koniec
    void add_behind(T data) {
        Node<T>* node = new Node<T>();
        node->data = data;

        if (size == 0) {
            Head = node;
            Tail = node;
        }
        else {
            Tail->next = node;
            node->previous = Tail;
            Tail = node;
        }
        size++;
    }

    // Dodanie elementu na pocz�tek
    void add_ahead(T data) {
        Node<T>* node = new Node<T>();
        node->data = data;

        if (size == 0) {
            Head = node;
            Tail = node;
        }
        else {
            node->next = Head;
            Head->previous = node;
            Head = node;
        }
        size++;
    }

    // Usuni�cie elementu z ko�ca
    void remove_behind() {
        if (size == 1) {
            delete Tail;
            Tail = nullptr;
            Head = nullptr;
        }
        else {
            Tail = Tail->previous;
            delete Tail->next;
            Tail->next = nullptr;
        }
        size--;
    }

    // Usuni�cie elementu z pocz�tku
    void remove_ahead() {
        if (size == 1) {
            delete Head;
            Head = nullptr;
            Tail = nullptr;
        }
        else {
            Head = Head->next;
            delete Head->previous;
            Head->previous = nullptr;
        }
        size--;
    }

    // Dost�p do elementu przez indeks
    T operator[](size_t index) {
        if (index >= size) {
            throw out_of_range("Index out of range");
        }

        Node<T>* current;
        if (index < size / 2) {
            current = Head;
            for (size_t i = 0; i < index; i++) {
                current = current->next;
            }
        }
        else {
            current = Tail;
            for (size_t i = size - 1; i > index; i--) {
                current = current->previous;
            }
        }
        return current->data;
    }

    // Czyszczenie listy
    void clear() {
        Node<T>* current = Head;
        while (current != nullptr) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        Head = nullptr;
        Tail = nullptr;
        size = 0;
    }
};
