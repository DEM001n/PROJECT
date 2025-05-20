#include "RBTree.h"

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
