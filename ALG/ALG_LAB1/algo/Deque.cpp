#include "Deque.h"

Data::Data(int a, char b) : val_int(a), val_char(b) {}
Node::Node(int a, char b) : data(a, b), next(nullptr), prev(nullptr) {}

template<typename T>
Deque<T>::Deque() : front(nullptr), rear(nullptr) {}

template<typename T>
bool Deque<T>::isEmpty() const
{
    return front == nullptr;
}

template<typename T>
int Deque<T>::countNodes() const
{
    int count = 0;
    Node* current = front;
    while (current != nullptr)
    {
        count++;
        current = current->next;
    }
    return count;
}

template<typename T>
bool Deque<T>::isEqual(const Data& a, const T& value) const // Dodaj 'const' tutaj
{
    if constexpr (std::is_same_v<T, int>)
    {
        return a.val_int == value;
    }
    else if constexpr (std::is_same_v<T, char>)
    {
        return a.val_char == value;
    }
    return false;
}

template<typename T>
bool Deque<T>::comparison(Node* newNode, Node* current) const
{
    if (std::is_same_v<T, int>)
    {
        return newNode->data.val_int <= current->data.val_int;
    }
    else if (std::is_same_v<T, char>)
    {
        return newNode->data.val_char <= current->data.val_char;
    }
    return false;
}

template<typename T>
void Deque<T>::push(int intValue, char charValue)
{
    Node* newNode = new Node(intValue, charValue);
    if (isEmpty())
    {
        front = rear = newNode;
        return;
    }

    Node* current = front;
    if (comparison(newNode, current))
    {
        newNode->next = front;
        front->prev = newNode;
        front = newNode;
        return;
    }

    while (current->next != nullptr)
    {
        if (comparison(newNode, current->next))
        {
            newNode->next = current->next;
            newNode->prev = current;
            current->next->prev = newNode;
            current->next = newNode;
            return;
        }
        current = current->next;
    }

    rear->next = newNode;
    newNode->prev = rear;
    rear = newNode;
}

template<typename T>
void Deque<T>::remove(T targetValue)
{
    if (isEmpty())
    {
        std::cout << "Deque is empty!" << std::endl;
        return;
    }

    Node* current = front;
    while (current != nullptr)
    {
        if (isEqual(current->data, targetValue))
        {
            if (current->prev != nullptr)
            {
                current->prev->next = current->next;
            }
            else
            {
                front = current->next;
            }

            if (current->next != nullptr)
            {
                current->next->prev = current->prev;
            }
            else
            {
                rear = current->prev;
            }

            std::cout << "Removing node with value: "
                << (std::is_same_v<T, int> ? current->data.val_int : current->data.val_char) << std::endl;
            delete current;
            return;
        }
        current = current->next;
    }

    std::cout << "Value not found in deque!" << std::endl;
}

template<typename T>
void Deque<T>::printNodeInfo(Node* node) const
{
    if (node != nullptr)
    {
        std::cout << "Data[ int: " << node->data.val_int << ", char: " << node->data.val_char << " ], "
            << "Next: " << node->next << ", "
            << "Prev: " << node->prev << std::endl;
    }
    else
    {
        std::cout << "Node is nullptr!" << std::endl;
    }
}

template<typename T>
void Deque<T>::printNodeAtIndex(int index, int direction) const
{
    if (index < 0 || index >= countNodes())
    {
        std::cout << "Index out of bounds!" << std::endl;
        return;
    }

    Node* current = (direction == 0) ? front : rear;
    if (direction == 0)
        for (int i = 0; i < index; i++)
            current = current->next;
    else
        for (int i = 0; i < index; i++)
            current = current->prev;

    printNodeInfo(current);
}

template<typename T>
void Deque<T>::printAllNodeInfo(int direction) const
{
    if (isEmpty())
    {
        std::cout << "Deque is empty!" << std::endl;
        return;
    }

    Node* current = (direction == 0) ? front : rear;
    int index = 0;

    while (current != nullptr)
    {
        std::cout << "Node " << index << ": ";
        printNodeInfo(current);
        current = (direction == 0) ? current->next : current->prev;
        index++;
    }
}

template<typename T>
void Deque<T>::clearDeque()
{
    Node* current = front;

    while (current != nullptr)
    {
        Node* next = current->next;
        delete current;
        current = next;
    }

    front = rear = nullptr;

    std::cout << "Deque cleared." << std::endl;
}

template<typename T>
void Deque<T>::updateNodeValue(int index, int direction, int intValue, char charValue)
{
    if (index < 0 || index >= countNodes())
    {
        std::cout << "Index out of bounds!" << std::endl;
        return;
    }

    Node* current = (direction == 0) ? front : rear;

    if (direction == 0)
        for (int i = 0; i < index; i++)
            current = current->next;
    else
        for (int i = 0; i < index; i++)
            current = current->prev;

    current->data.val_int = intValue;
    current->data.val_char = charValue;
}

template<typename T>
void Deque<T>::findByValue(T value, int direction) const
{
    Node* current = (direction == 0) ? front : rear;

    while (current != nullptr)
    {
        if (isEqual(current->data, value))
        {
            printNodeInfo(current);
            return;
        }

        current = (direction == 0) ? current->next : current->prev;
    }

    std::cout << "Value not found!" << std::endl;
}

template<typename T>
Deque<T>::~Deque()
{
    clearDeque();
}

template class Deque<int>;
template class Deque<char>;
