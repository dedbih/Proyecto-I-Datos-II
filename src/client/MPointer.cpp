#include "MPointer.h"

int main() {
    MPointer<int>::Init("127.0.0.1", 12345);

    MPointer<int> ptr = MPointer<int>::New();
    *ptr = 42; // Ahora funciona correctamente
    int valor = *ptr;
    std::cout << "Valor: " << valor << std::endl;

    MPointer<int> ptr2 = ptr; // Copia segura
    return 0;
}
