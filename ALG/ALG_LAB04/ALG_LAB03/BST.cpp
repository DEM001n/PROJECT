#include "BST.h"

int getRandomNumber(int min, int max) {
    return min + rand() % ((max - min) + 1);
}

template <typename T>
BSTNode<T>::BSTNode(int id, T data, bool color)
    : id(id), key(data), color(color), parent(nullptr), left(nullptr), right(nullptr) {}

template <typename T>
RBTree<T>::RBTree() : root(nullptr), size(0), count(0) {}

template <typename T>
RBTree<T>::~RBTree() {
    clearTree(root);
}

template <typename T>
void RBTree<T>::insert(T data, std::function<bool(T, T)> comp) {
    insertNode(root, nullptr, count, data, comp);
    count++;
}

template <typename T>
void RBTree<T>::preOrderDisplay() const {
    int height = getHeight();
    std::cout << "Pre-order display:\n";
    preOrderDisplay(root, 0, height);
}

template <typename T>
void RBTree<T>::display() const {
    int height = getHeight();
    std::cout << "In-order display of RBTree:\n";
    inOrderDisplay(root, height);
}

template <typename T>
int RBTree<T>::getSize() const {
    return size;
}

template <typename T>
int RBTree<T>::getHeight() const {
    return getHeightHelper(root);
}

// =======================================================

template <typename T>
void RBTree<T>::rotateLeft(BSTNode<T>*& node) {
    BSTNode<T>* rightChild = node->right;
    node->right = rightChild->left;
    if (rightChild->left) {
        rightChild->left->parent = node;
    }
    rightChild->parent = node->parent;
    if (!node->parent) {
        root = rightChild;
    }
    else if (node == node->parent->left) {
        node->parent->left = rightChild;
    }
    else {
        node->parent->right = rightChild;
    }
    rightChild->left = node;
    node->parent = rightChild;
}

template <typename T>
void RBTree<T>::rotateRight(BSTNode<T>*& node) {
    BSTNode<T>* leftChild = node->left;
    node->left = leftChild->right;
    if (leftChild->right) {
        leftChild->right->parent = node;
    }
    leftChild->parent = node->parent;
    if (!node->parent) {
        root = leftChild;
    }
    else if (node == node->parent->left) {
        node->parent->left = leftChild;
    }
    else {
        node->parent->right = leftChild;
    }
    leftChild->right = node;
    node->parent = leftChild;
}

template <typename T>
void RBTree<T>::insertNode(BSTNode<T>*& node, BSTNode<T>* parent, int id, T data, std::function<bool(T, T)> comp) {
    if (!node) {
        node = new BSTNode<T>(id, data, true);  // Новый узел всегда красный
        node->parent = parent;
        ++size;
        balanceAfterInsert(node); // Балансировка после вставки
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
void RBTree<T>::balanceAfterInsert(BSTNode<T>*& node) {
    while (node != root && node->parent->color) {
        BSTNode<T>* parent = node->parent;
        BSTNode<T>* grandparent = parent->parent;

        if (parent == grandparent->left) {
            BSTNode<T>* uncle = grandparent->right;
            if (uncle && uncle->color) {  // Случай 1: Дядя красный
                parent->color = false;
                uncle->color = false;
                grandparent->color = true;
                node = grandparent;
            }
            else {
                if (node == parent->right) {  // Случай 2: Левый поворот
                    node = parent;
                    rotateLeft(node);
                }
                parent->color = false; // Случай 3: Правый поворот
                grandparent->color = true;
                rotateRight(grandparent);
            }
        }
        else {
            BSTNode<T>* uncle = grandparent->left;
            if (uncle && uncle->color) {  // Случай 1: Дядя красный
                parent->color = false;
                uncle->color = false;
                grandparent->color = true;
                node = grandparent;
            }
            else {
                if (node == parent->left) {  // Случай 2: Правый поворот
                    node = parent;
                    rotateRight(node);
                }
                parent->color = false; // Случай 3: Левый поворот
                grandparent->color = true;
                rotateLeft(grandparent);
            }
        }
    }
    root->color = false;  // Корень всегда черный
}

// ===================== Функции отображения и очистки =====================

template <typename T>
void RBTree<T>::preOrderDisplay(BSTNode<T>* node, int depth, int height) const {
    if (node) {
        for (int i = 0; i < height - depth; ++i) {
            std::cout << "-";
        }
        std::cout << "ID:" << node->id << " Key:" << node->key;
        std::cout << " Color:" << (node->color ? "Red" : "Black") << std::endl;

        preOrderDisplay(node->left, depth + 1, height);
        preOrderDisplay(node->right, depth + 1, height);
    }
}

template <typename T>
void RBTree<T>::inOrderDisplay(BSTNode<T>* node, int height, int depth) const {
    if (node) {
        inOrderDisplay(node->left, height, depth + 1);
        for (int i = 0; i < height - depth; ++i) {
            std::cout << "-";
        }
        std::cout << "ID:" << node->id << " Key:" << node->key;
        std::cout << " Color:" << (node->color ? "Red" : "Black") << std::endl;
        inOrderDisplay(node->right, height, depth + 1);
    }
}

template <typename T>
void RBTree<T>::clearTree(BSTNode<T>* node) {
    if (node) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
}

template <typename T>
int RBTree<T>::getHeightHelper(BSTNode<T>* node) const {
    if (!node) {
        return 0;
    }
    int leftHeight = getHeightHelper(node->left);
    int rightHeight = getHeightHelper(node->right);
    return std::max(leftHeight, rightHeight) + 1;
}

// Эксплицитная инстанциация шаблона для конкретных типов
template class RBTree<int>;
template class RBTree<char>;
