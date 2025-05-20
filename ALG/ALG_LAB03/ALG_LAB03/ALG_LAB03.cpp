#include <iostream>
#include <algorithm>
#include <functional> 
#include <sstream>   
#include <time.h>
#include <cstdlib>  // rand() srand()
#include <ctime>    // time()

int getRandomNumber(int min, int max);

template <typename Func>
void measureExecutionTime(Func func)
{
    std::clock_t start = std::clock();
    func();
    std::clock_t end = std::clock();
    double duration = double(static_cast<int64_t>(end) - static_cast<int64_t>(start)) / (CLOCKS_PER_SEC / 1000);
    std::cout << "Execution time: " << duration << " milliseconds" << std::endl;
};

template <typename T>
struct BSTNode {
    int id;
    T key;
    BSTNode<T>* left;
    BSTNode<T>* right;
    BSTNode<T>* parent;

    BSTNode(int id, T data);
};

template <typename T>
class BST {
private:
    BSTNode<T>* root;
    int size;
    int count;

    void insertNode(BSTNode<T>*& node, BSTNode<T>* parent, int id, T data, std::function<bool(T, T)> comp);
    BSTNode<T>* searchNode(BSTNode<T>* node, T data, std::function<bool(T, T)> comp) const;
    void deleteNode(BSTNode<T>*& node, T data, std::function<bool(T, T)> comp);
    BSTNode<T>* getMin(BSTNode<T>* node) const;
    void preOrderDisplay(BSTNode<T>* node, int depth, int height) const;
    void inOrderDisplay(BSTNode<T>* node, int height, int depth = 0) const;
    void clearTree(BSTNode<T>* node);
    int getHeightHelper(BSTNode<T>* node) const;

public:
    BST();
    ~BST();

    void insert(T data, std::function<bool(T, T)> comp);
    BSTNode<T>* search(T data, std::function<bool(T, T)> comp) const;
    void deleteNode(T data, std::function<bool(T, T)> comp);
    void preOrderDisplay() const;
    void display() const;
    int getSize() const;
    int getHeight() const;
};


int getRandomNumber(int min, int max) {
    return min + rand() % ((max - min) + 1);
}

template <typename T>
BSTNode<T>::BSTNode(int id, T data): id(id), key(data), parent(nullptr), left(nullptr), right(nullptr) {}

template <typename T>
BST<T>::BST() : root(nullptr), size(0), count(0) {}

template <typename T>
BST<T>::~BST() {
    clearTree(root);
}

template <typename T>
void BST<T>::insert(T data, std::function<bool(T, T)> comp) {
    insertNode(root, nullptr, count, data, comp);
    count++;
}

template <typename T>
BSTNode<T>* BST<T>::search(T data, std::function<bool(T, T)> comp) const {
    return searchNode(root, data, comp);
}

template <typename T>
void BST<T>::deleteNode(T data, std::function<bool(T, T)> comp) {
    deleteNode(root, data, comp);
}

template <typename T>
void BST<T>::preOrderDisplay() const {
    int height = getHeight(); 
    std::cout << "Pre-order display:\n";
    preOrderDisplay(root, 0, height); 
}

template <typename T>
void BST<T>::display() const {
    int height = getHeight();
    std::cout << "In-order display of BST:\n";
    inOrderDisplay(root, height);
}

template <typename T>
int BST<T>::getSize() const {
    return size;
}

template <typename T>
int BST<T>::getHeight() const {
    return getHeightHelper(root);
}

//==============================================================================================

template <typename T>
void BST<T>::insertNode(BSTNode<T>*& node, BSTNode<T>* parent, int id, T data, std::function<bool(T, T)> comp) {
    if (!node) {
        node = new BSTNode<T>(id, data);
        node->parent = parent;
        ++size;
    }
    else if (comp(data, node->key)) {
        insertNode(node->left, node, id, data, comp);
    }
    else if (comp(node->key, data)) {
        insertNode(node->right, node, id, data, comp);
    }
    else {
        std::cout << data << " already exists.\n";
    }
}

template <typename T>
BSTNode<T>* BST<T>::searchNode(BSTNode<T>* node, T data, std::function<bool(T, T)> comp) const {
    if (!node) return nullptr;
    if (node->key == data) return node;
    if (comp(data, node->key)) return searchNode(node->left, data, comp);
    return searchNode(node->right, data, comp);
}

template <typename T>
void BST<T>::deleteNode(BSTNode<T>*& node, T data, std::function<bool(T, T)> comp) {
    if (!node) return;

    if (comp(data, node->key)) {
        deleteNode(node->left, data, comp);
    }
    else if (comp(node->key, data)) {
        deleteNode(node->right, data, comp);
    }
    else {
        if (!node->left && !node->right) {
            delete node;
            node = nullptr;
        }
        else if (!node->left) {
            BSTNode<T>* temp = node;
            node = node->right;
            delete temp;
        }
        else if (!node->right) {
            BSTNode<T>* temp = node;
            node = node->left;
            delete temp;
        }
        else {
            BSTNode<T>* minNode = getMin(node->right);
            node->key = minNode->key;
            node->id = minNode->id;
            deleteNode(node->right, minNode->key, comp);
        }
    }
}

template <typename T>
BSTNode<T>* BST<T>::getMin(BSTNode<T>* node) const {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

template <typename T>
void BST<T>::preOrderDisplay(BSTNode<T>* node, int depth, int height) const {
    if (node) {
        for (int i = 0; i < height - depth; ++i) {
            std::cout << "-"; //\t
        }
        std::cout << "ID:" << node->id << " Key:" << node->key;
        if (node->parent) {
            std::cout << " P:" << node->parent->id;
        }
        if (node->left) {
            std::cout << " L:" << node->left->id;
        }
        if (node->right) {
            std::cout << " R:" << node->right->id;
        }

        std::cout << std::endl;

        preOrderDisplay(node->left, depth + 1, height);
        preOrderDisplay(node->right, depth + 1, height);
    }
}


template <typename T>
void BST<T>::inOrderDisplay(BSTNode<T>* node, int height, int depth) const {
    if (node) {
        inOrderDisplay(node->left, height, depth + 1);
        for (int i = 0; i < height - depth; ++i) {
            std::cout << "-";
        }
        std::cout << "ID:" << node->id << " Key:" << node->key;
        if (node->parent) {
            std::cout << " P:" << node->parent->id;
        }
        if (node->left) {
            std::cout << " L:" << node->left->id;
        }
        if (node->right) {
            std::cout << " R:" << node->right->id;
        }
        std::cout << std::endl;
        inOrderDisplay(node->right, height, depth + 1);
    }
}

template <typename T>
void BST<T>::clearTree(BSTNode<T>* node) {
    if (node) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
}

template <typename T>
int BST<T>::getHeightHelper(BSTNode<T>* node) const {
    if (!node) {
        return 0;
    }
    int leftHeight = getHeightHelper(node->left);
    int rightHeight = getHeightHelper(node->right);
    return std::max(leftHeight, rightHeight) + 1;
}


template class BST<int>;
template class BST<char>;


int main() {
    BST<int> tree;

    auto comp = [](int a, int b) { return a < b; };

    measureExecutionTime([&]() {
        for (size_t i = 0; i < 1000; i++)
        {
            tree.insert(getRandomNumber(-500, 500), comp);
        }
        });
 

    measureExecutionTime([&]() {tree.display(); });

    std::cout << "Tree size: " << tree.getSize() << std::endl;
    std::cout << "Tree height: " << tree.getHeight() << std::endl;

    measureExecutionTime([&]() {tree.preOrderDisplay(); });

    measureExecutionTime([&]() {
        for (size_t i = 0; i < 500; i++)
        {


            BSTNode<int>* found = tree.search(getRandomNumber(-500, 500), comp);
            if (found) {
                std::cout << "Found node: ID:" << found->id << " Key:" << found->key << std::endl;
            }
            else {
                std::cout << "Node not found.\n";
            }
        }
        });
    

    measureExecutionTime([&]() {
        for (size_t i = 0; i < 1000; i++)
        {
            tree.deleteNode(getRandomNumber(-500, 500), comp);
        }
        });

    return 0;
}
