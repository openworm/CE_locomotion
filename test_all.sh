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


make MAKE_JSON=1

if [ "$quick_test" == 0 ]; then
    rm -rf exampleRun
    #time python run_main.py -R 1233 -p 96 --doEvol --folderName exampleRun

    time omv all -V 
fi

make MAKE_JSON=1 tests2

rm -rf test_output_2/*.dat
./tests2