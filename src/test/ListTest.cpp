#include <iostream>
#include "MPointer.h"

struct Node {
    MPointer<int> data;
    MPointer<Node> next;

    Node(int value = 0) {  // Combined constructor
        data = MPointer<int>::New(value);
        next = MPointer<Node>::New();  // Initialize with null pointer
    }
};

void printList(MPointer<Node> head) {
    MPointer<Node> current = head;
    while (current != nullptr) {  // Now works with the new operator
        if (current->data != nullptr) {  // Check data pointer validity
            std::cout << *current->data << " -> ";
        }
        current = current->next;
    }
    std::cout << "NULL" << std::endl;
}

int main() {
    // Create nodes with proper initialization
    MPointer<Node> head = MPointer<Node>::New(10);  // Pass constructor argument
    head->next = MPointer<Node>::New(20);  // Initialize next node with value

    printList(head);
    return 0;
}