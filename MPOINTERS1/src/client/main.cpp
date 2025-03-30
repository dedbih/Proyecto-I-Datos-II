#include "MPointer.h"
#include <iostream>

int main() {
    MPointer<int> p1 = MPointer<int>::New(42);
    std::cout << "Value: " << *p1 << std::endl;

    *p1 = 100;
    std::cout << "Modified value: " << *p1 << std::endl;

    return 0;
}

//si