#include "Deque.h"

int main()
{
    Deque<char> deque; 

    measureExecutionTime([&]()
        {
            for (int i = 0; i < 1000; i++)
            {
                deque.push(i , (char)(i%200));
            }
        });

    measureExecutionTime([&]()
        {
            deque.printAllNodeInfo(1);
        });

    measureExecutionTime([&]()
        {
            for (int i = 0; i < 500; i++)
                deque.remove((char)(i%100)); 
        });

    measureExecutionTime([&]()
        {
            deque.printAllNodeInfo(0);
        });

    deque.findByValue((char)200, 1);
    deque.findByValue((char)99, 0);

    deque.printNodeAtIndex(15, 0);
    deque.printNodeAtIndex(3, 1);

    return 0;
}
