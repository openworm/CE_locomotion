#!/bin/bash
set -ex

make clean all

./test_py_cpp 
