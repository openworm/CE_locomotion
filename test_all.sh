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

cd CE_orientation
make clean
make
cd ..

cd Worm2D
make clean
make
cd ..


if [ "$quick_test" == 0 ]; then

    
    rm -rf exampleRunCOW2D
    rm -rf exampleRunCO

   

    omv test -V .test.CO.omt
    omv test -V .test.COW2D.omt
    
fi

