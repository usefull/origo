#include <iostream>

int main() {

#ifdef NDEBUG
    std::cout << "Origo is running in RELEASE mode..." << std::endl;
#else
    std::cout << "Origo is running in DEBUG mode..." << std::endl;
#endif

    return 0;
}