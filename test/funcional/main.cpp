#include <iostream>
#include "funcional_tests.h"

using namespace std;

int main() {
    int falhas = runFunctionalTests();
    cout << (falhas == 0 ? "\nFUNCIONAL: OK\n" : "\nFUNCIONAL: FALHOU\n");
    return falhas == 0 ? 0 : 1;
}
