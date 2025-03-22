#include "MPointer.h"
#include <iostream>

struct Node {
    MPointer<int> data;
    MPointer<Node> next;

    Node(int value) : data(MPointer<int>::New()) {
        *data = value;
        next = nullptr;
    }
};

void printList(MPointer<Node> head) {
    MPointer<Node> current = head;
    while (current) {
        std::cout << *current->data << " -> ";
        current = current->next;
    }
    std::cout << "NULL" << std::endl;
}

int main() {
    MPointer<Node> head = MPointer<Node>::New();
    *head = Node(10);
    head->next = MPointer<Node>::New();
    *(head->next) = Node(20);

    printList(head);
    return 0;
}
