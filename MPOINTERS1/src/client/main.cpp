#include "MPointer.h"
#include <iostream>

int main() {
    MPointer p1(1);
    p1.set(42);

    std::cout << "Valor almacenado : " << p1.get() << std::endl;
    return 0;
}

//si