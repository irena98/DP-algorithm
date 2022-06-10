#include <iostream>
#include "solver.h"

int main() {
    Solver sat(std::cin);

    if(sat.dp_sat())
        std::cout << "SAT" << std::endl;
    else
        std::cout << "UNSAT" << std::endl;

    return 0;
}