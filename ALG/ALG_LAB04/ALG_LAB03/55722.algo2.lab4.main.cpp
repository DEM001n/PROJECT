//ALG02 IS1 210B LAB04
//Dmytro Mahaliuk
//md55722@zut.edu.pl

#include <iostream>
#include <algorithm>
#include <functional> 
#include <sstream>   
#include <ctime>
#include <windows.h>

void setConsoleTextColor(WORD color);

int getRandomNumber(int min, int max);

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

template <typename T>
struct BSTNode {
    int id;
    bool color;       // false = black, true = red
    T key;
    BSTNode<T>* left;
    BSTNode<T>* right;
    BSTNode<T>* parent;

    BSTNode(int id, T data, bool color);
};

template <typename T>
class RBTree {
private:
    BSTNode<T>* root;
    int size;

    void insertNode(BSTNode<T>*& node, BSTNode<T>* parent, int id, T data, std::function<bool(T, T)> comp);

    void reIndexNodes(BSTNode<T>* node, int id);


    void rotateRight(BSTNode<T>*& root, BSTNode<T>* node);
    void rotateLeft(BSTNode<T>*& root, BSTNode<T>* node);

    void balanceAfterInsert(BSTNode<T>* node);

    void preOrderDisplay(BSTNode<T>* node, int depth, int height) const;
    void inOrderDisplay(BSTNode<T>* node, int height, int depth = 0) const;
    void clearTree(BSTNode<T>* node);
    int getHeightHelper(BSTNode<T>* node) const;
    BSTNode<T>* searchNode(BSTNode<T>* node, T data, std::function<bool(T, T)> comp) const;
       
public:
    RBTree();
    ~RBTree();

    void insert(T data, std::function<bool(T, T)> comp);
    void preOrderDisplay() const;
    void inOrderDisplay() const;
    void display() const;
    int getSize() const;
    int getHeight() const;
    void clear();
    BSTNode<T>* search(T data, std::function<bool(T, T)> comp) const;

};

void setConsoleTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

int getRandomNumber(int min, int max) {
    return min + rand() % ((max - min) + 1);
}

template <typename T>
BSTNode<T>::BSTNode(int id, T data, bool color)
    : id(id), key(data), color(color), parent(nullptr), left(nullptr), right(nullptr) {}

template <typename T>
RBTree<T>::RBTree() : root(nullptr), size(0){}

template <typename T>
RBTree<T>::~RBTree() {
    clearTree(root);
}

template <typename T>
void RBTree<T>::insert(T data, std::function<bool(T, T)> comp) {
    insertNode(root, nullptr, 0, data, comp);
}

template <typename T>
void RBTree<T>::preOrderDisplay() const {
    int height = getHeight();
    std::cout << "Pre-order display:\n";
    preOrderDisplay(root, 0, height);
}

template <typename T>
void RBTree<T> ::inOrderDisplay() const {
    int height = getHeight();
    std::cout << "In-order display:\n";
    inOrderDisplay(root, height, 0);
}

template <typename T>
void RBTree<T>::display() const {
    std::cout << "height:"<<getHeight()<<" size:" << getSize() << std::endl;
}

template <typename T>
int RBTree<T>::getSize() const {
    return size;
}

template <typename T>
int RBTree<T>::getHeight() const {
    return getHeightHelper(root)-1;
}

template <typename T>
void RBTree<T>::clear() {
    clearTree(root);
    size = 0;
}

template <typename T>
BSTNode<T>* RBTree<T>::search(T data, std::function<bool(T, T)> comp) const {
    return searchNode(root, data, comp);
}

// =================================================================

template <typename T>
void RBTree<T>::rotateLeft(BSTNode<T>*& root, BSTNode<T>* node) {
    if (node == nullptr || node->right == nullptr)
        return;

    BSTNode<T>* temp = node->right;
    node->right = temp->left;

    if (temp->left != nullptr) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node->parent == nullptr) {
        root = temp;
    }
    else if (node == node->parent->left) {
        node->parent->left = temp;
    }
    else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}

template <typename T>
void RBTree<T>::rotateRight(BSTNode<T>*& root, BSTNode<T>* node) {
    if (node == nullptr || node->left == nullptr)
        return;

    BSTNode<T>* temp = node->left;
    node->left = temp->right;

    if (temp->right != nullptr) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node->parent == nullptr) {
        root = temp;
    }
    else if (node == node->parent->right) {
        node->parent->right = temp;
    }
    else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}

template <typename T>
void RBTree<T>::reIndexNodes(BSTNode<T>* node, int id) {
    if (node == nullptr) return;

    node->id = id;
    if (node->left) {
        reIndexNodes(node->left, (id * 2) + 1);
    }
    if (node->right) {
        reIndexNodes(node->right, (id * 2) + 2);
    }
}

template <typename T>
void RBTree<T>::balanceAfterInsert(BSTNode<T>* node) {
    BSTNode<T>* parent = nullptr;
    BSTNode<T>* grandparent = nullptr;

    while (node != root && node->color == true && node->parent->color == true) {
        parent = node->parent;
        grandparent = parent->parent;

        if (parent == grandparent->left) {
            BSTNode<T>* uncle = grandparent->right;

            if (uncle != nullptr && uncle->color == true) {
                grandparent->color = true;
                parent->color = false;
                uncle->color = false;
                node = grandparent;
            }
            else {
                if (node == parent->right) {
                    rotateLeft(root, parent);
                    node = parent;
                    parent = node->parent;
                }

                rotateRight(root, grandparent);
                std::swap(parent->color, grandparent->color);
                node = parent;
            }
        }
        else {
            BSTNode<T>* uncle = grandparent->left;

            if (uncle != nullptr && uncle->color == true) {
                grandparent->color = true;
                parent->color = false;
                uncle->color = false;
                node = grandparent;
            }
            else {
                if (node == parent->left) {
                    rotateRight(root, parent);
                    node = parent;
                    parent = node->parent;
                }

                rotateLeft(root, grandparent);
                std::swap(parent->color, grandparent->color);
                node = parent;
            }
        }
    }

    root->color = false;
    reIndexNodes(root, 0);
}

template <typename T>
void RBTree<T>::insertNode(BSTNode<T>*& node, BSTNode<T>* parent,int id, T data, std::function<bool(T, T)> comp) {
    if (!node) {
        node = new BSTNode<T>(id, data, true);
        node->parent = parent;
        ++size;

        balanceAfterInsert(node);
    }
    else if (comp(data, node->key)) {
        id = (id * 2) + 1;
        insertNode(node->left, node, id, data, comp);
    }
    else if (comp(node->key, data)) {
        id = (id * 2) + 2;
        insertNode(node->right, node, id, data, comp);
    }
    else {
        std::cout << data << " already exists.\n";
    }
}

template <typename T>
void RBTree<T>::preOrderDisplay(BSTNode<T>* node, int depth, int height) const {
    if (node) {
        for (int i = 0; i < height - depth; ++i) {
            std::cout << i << "\t";
        }

        if (node->color)
            setConsoleTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        else
            setConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        std::cout << "ID:" << node->id << " Key:" << node->key << " COL:" << (node->color ? "R" : "B");
        if (node->left) {
            std::cout << " L:" << node->left->id;
        }
        if (node->right) {
            std::cout << " R:" << node->right->id;
        }
        if (node->parent) {
            std::cout << " PID:" << node->parent->id;
        }

        setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        std::cout << std::endl;
        preOrderDisplay(node->left, depth + 1, height);
        preOrderDisplay(node->right, depth + 1, height);
    }
}

template <typename T>
void RBTree<T>::inOrderDisplay(BSTNode<T>* node, int height, int depth) const {
    if (node) {
        inOrderDisplay(node->left, height, depth + 1);
        for (int i = 0; i < height - depth; ++i) {
            std::cout <<i <<"\t";
        }

        if (node->color)
            setConsoleTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        else
            setConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        std::cout << "ID:" << node->id << " Key:" << node->key << " COL:" << (node->color ? "R" : "B");
        if (node->left) {
            std::cout << " L:" << node->left->id;
        }
        if (node->right) {
            std::cout << " R:" << node->right->id;
        }
        if (node->parent) {
            std::cout << " PID:" << node->parent->id;
        }

        setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 

        std::cout << std::endl;
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
    return max(leftHeight, rightHeight) + 1;
}

template <typename T>
BSTNode<T>* RBTree<T>::searchNode(BSTNode<T>* node, T data, std::function<bool(T, T)> comp) const {
    if (!node) return nullptr;
    if (node->key == data) return node;
    if (comp(data, node->key)) return searchNode(node->left, data, comp);
    return searchNode(node->right, data, comp);
}

template class RBTree<int>;
template class RBTree<char>;


int main() {


    RBTree<int> tree;

    auto comp = [](int a, int b) { return a < b; };

    measureExecutionTime([&]() {
        for (size_t i = 0; i < 1000; i++)
        {
            tree.insert(getRandomNumber(-500, 500), comp);
        }
        });

    tree.display();
 
    measureExecutionTime([&]() {tree.inOrderDisplay(); });

    std::cout << "Tree size: " << tree.getSize() << std::endl;
    std::cout << "Tree height: " << tree.getHeight() << std::endl;

    measureExecutionTime([&]() {tree.preOrderDisplay(); });

    measureExecutionTime([&]() {
        for (size_t i = 0; i < 500; i++)
        {


            BSTNode<int>* found = tree.search(getRandomNumber(-500, 500), comp);
            if (found) {
                std::cout << "ID:" << found->id << " Key:" << found->key << " COL:" << (found->color ? "R" : "B");
                if (found->left) {
                    std::cout << " L:" << found->left->id;
                }
                if (found->right) {
                    std::cout << " R:" << found->right->id;
                }
                if (found->parent) {
                    std::cout << " PID:" << found->parent->id;
                }

                std::cout << std::endl;
            }
            else {
                std::cout << "Node not found.\n";
            }
        }
        });

    return 0;
}
