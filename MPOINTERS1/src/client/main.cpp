#include "MPointer.h"
#include <iostream>


int main() {
    MPointer<int> p1 = MPointer<int>::New();  // Use MPointer to create a new integer
    p1.set(42);  // Set value to 42
    std::cout << *p1 << std::endl;  // Output value (should print 42)
    return 0;
}

//si