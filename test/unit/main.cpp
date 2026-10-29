#include <iostream>
#include "unit_tests.h"

using namespace std;

int main() {
    int falhas = runUnitTests();
    cout << (falhas == 0 ? "\nUNITARIO: OK\n" : "\nUNITARIO: FALHOU\n");
    return falhas == 0 ? 0 : 1;
}
