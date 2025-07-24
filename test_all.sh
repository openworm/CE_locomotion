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

# Compile the main program
make


# Compile the CE_orientation C++ code
cd CE_orientation
make clean
make
cd ..

# Compile the Worm2D C++ code
cd Worm2D
make clean
make
cd ..


if [ "$quick_test" == 0 ]; then


    # Run the main program & generate exampleRun dir
    rm -rf exampleRun
    omv test -V .test.example.omt

    # Regenerate the neuroml & test version of main with that 
    rm -rf exampleRun_nml    
    cd neuromlLocal
    ./regenerate.sh # regenerated NML & runs omv all -V
    cd ..
    
    omv test -V .test.nmlNS.omt
    


    
fi

