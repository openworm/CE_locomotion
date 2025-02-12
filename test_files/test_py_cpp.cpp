// =============================================================
// Tests for C++ code
// =============================================================

#include <Python.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>


void test()
{
    std::cout << "Created a test..." << std::endl;

    PyObject *pName, *pModule, *pDict, *pFun, *pValue;
    Py_Initialize();
    PyRun_SimpleString("print('Printing a statement from Python...')");
    Py_Finalize();
}
int main(int argc, const char *argv[])
{

    //Py_Initialize();

    std::cout << "Running a number of tests..." << std::endl;

    test();

    std::cout << "Done!" << std::endl;

    return 0;
}
