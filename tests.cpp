// =============================================================
// Tests for C++ code
// =============================================================

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include "TSearch.h"
#include <cassert>



// ------------------------------------
// The main program
// ------------------------------------
int main (int argc, const char* argv[])
{

    std::cout << "Running a number of tests..."  << std::endl;

    std::cout << clip(10, -10, 20)  << std::endl;

    assert(clip(10, -10, 20) == 10.0);
    assert(clip(1, -1, 1) == 1);
    assert(clip(-1.1, -1, 1) == -1);
    assert(clip(0, -1, 1) == 0);
    assert(clip(0, 1, 1) == 1);

    std::cout << "Done!"  << std::endl;

    return 0;
}
