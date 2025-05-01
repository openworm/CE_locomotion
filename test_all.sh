#!/bin/bash
set -ex

quick_test=0

if [[ ($# -eq 1) && ($1 == '-q') ]]; then
    quick_test=1
fi

make clean
make tests

rm -rf test_output/*.dat
./tests

make

ruff format *.py */*.py
ruff check *.py */*.py

if [ "$quick_test" == 0 ]; then

    rm -rf exampleRun
    rm -rf exampleRun_nml
    
    omv test -V .test.example.omt
    
    cd neuromlLocal
    ./regenerate.sh # regenerated NML & runs omv all -V
    cd ..
    
    omv test -V .test.nmlNS.omt
    
fi

make tests2

rm -rf test_output_2/*.dat
./tests2