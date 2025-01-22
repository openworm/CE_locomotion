// =============================================================
// Tests for C++ code
// =============================================================

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <Python.h>

void test()
{
    std::cout << "Created aa test..." << std::endl;

    PyObject *pName, *pModule, *pDict, *pFun, *pValue;
    //Py_Initialize();
}
int main(int argc, const char *argv[])
{

   // Py_Initialize();

    std::cout << "Running a number of tests..." << std::endl;

    test();

    std::cout << "Done!" << std::endl;

    return 0;
}
