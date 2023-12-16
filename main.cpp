#include <iostream>
#include "vector.h"


int main() {
    std::cout << "Hello, World!" << std::endl;

    Vector<int> v;

    Vector<int> a{2, 5, 4, 6};
    Vector<int> b{5, 1};
    std::cout << a.kmp(b);
    return 0;
}
