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

cd RoyalSociety2018
make clean
make
cd ..

cd network2021
make clean
make
cd ..

cd Worm2D
make clean
make
cd ..

ruff format *.py */*.py
ruff check *.py */*.py

if [ "$quick_test" == 0 ]; then

    rm -rf exampleRun
    rm -rf exampleRun_nml
    rm -rf exampleRunRS18
    rm -rf exampleRunNet21
    rm -rf exampleRunRS18W2D
    rm -rf exampleRunCEW2D
    rm -rf exampleRun21W2D
    rm -rf exampleRunCEW2D_nml
    rm -rf exampleRun21W2D_nml

    if [[  `uname -o` == "GNU/Linux" ]]; then 
        echo "Running 2018 tests which only pass on Linux..."
        omv test -V .test.2018.omt
        omv test -V .test.2018W2D.omt
    fi;

    omv test -V .test.example.omt
    omv test -V .test.CEW2D.omt
    omv test -V .test.2021.omt
    omv test -V .test.2021W2D.omt
    

    cd neuromlLocal
    ./regenerate.sh # regenerated NML & runs omv all -V
    cd ..
    
    omv test -V .test.nmlNS.omt
    omv test -V .test.CEW2D_nml.omt

    cd neuromlLocal
    ./regenerate_21.sh # regenerated NML & runs omv all -V
    cd ..

    python test2021W2D_nml.py

fi

make tests2

rm -rf test_output_2/*.dat
./tests2