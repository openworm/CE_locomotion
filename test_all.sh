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

cd RoyalSociety2018
make clean
make
cd ..

cd network2021
make clean
make
cd ..

# Compile the Worm2D C++ code
cd Worm2D
make clean
make
make main_osc
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
    rm -rf exampleRunCOW2D
    rm -rf exampleRunCO
    rm -rf exampleRunW2DCE
    rm -rf experiments/izq_runs_nets/103 experiments/izq_runs_nets_nml/103 experiments/izq_runs_nets_nml_musc/103
    rm -rf experiments/izq_runs_nets/23 experiments/izq_runs_nets_nml/23 experiments/izq_runs_nets_nml_musc/23
    rm -rf testruns/exW2DSR testruns/exW2DSR_nml testruns/exW2DSR_nml_musc
    rm -rf testruns/exW2D18
    rm -rf testruns/exW2DCO
    rm -rf testruns/exW2D18gen
    rm -rf testruns/exW2DSR18 testruns/exW2DSR18_nml testruns/exW2DSR18_nml_musc
    rm -rf experiments/osc_sim experiments/osc_sim_nml experiments/osc_sim_nml_musc
    rm -rf experiments/osc_sim_21 experiments/osc_sim_21_nml experiments/osc_sim_21_nml_musc
    rm -rf experiments/osc_sim_21all experiments/osc_sim_21all_nml experiments/osc_sim_21all_nml_musc
    rm -rf testruns/exW2DSR21 testruns/exW2DSR21_nml testruns/exW2DSR21_nml_musc
    rm -rf testruns/exW2DSRE21
    rm -rf testruns/COW2DSREgen testruns/COW2DSREgen_out
    rm -rf testruns/COW2DSRE_test_out
    rm -rf testruns/exW2DSR18E

    if [[  `uname -o` == "GNU/Linux" ]]; then 
        echo "Running 2018 tests which only pass on Linux..."
    
        omv test -V .test.2018.omt
        omv test -V .test.2018W2D.omt
        omv test -V .test.W2D18.omt

    else
        python test2018.py
        python test2018W2D.py
        python testW2D18.py
    fi;

    omv test -V .test.izq_sim.omt

    omv test -V .test.CO.omt
    omv test -V .test.example.omt
    omv test -V .test.CEW2D.omt
    omv test -V .test.2021.omt
    omv test -V .test.2021W2D.omt
    omv test -V .test.W2DCE.omt

    omv test -V .test.osc_sim.omt
    omv test -V .test.osc_sim_21.omt
    omv test -V .test.osc_sim_21all.omt
    omv test -V .test.W2D18gen.omt
    omv test -V .test.W2DSR18.omt
    omv test -V .test.W2DSR18E.omt
    omv test -V .test.COW2D.omt
    omv test -V .test.W2DCO.omt
    


    omv test -V .test.W2DSR.omt
    omv test -V .test.W2DSR21.omt
    omv test -V .test.W2DSRE21.omt
    omv test -V .test.COW2DSR.omt
    omv test -V .test.COW2DSR2.omt


    cd neuromlLocal
    set -ex
    ./clean.sh 
    ruff format *py
    cd ..
    python regenerate_folder.py --folder exampleRunCEW2D
    cd neuromlLocal
    omv test -V .test.w2d.nrn.omt
    omv test -V .test.w2d.omt

    cd testc302SigSim
    make clean all
    ./testc302NervousSystem --popString "DA DB DD VD VA VB" --popSize 10 --datString "CEOrig"
    cd ..
    cd ..

    #./regenerate.sh # regenerated NML & runs omv all -V
    #cd ..
    
    omv test -V .test.nmlNS.omt
    omv test -V .test.CEW2D_nml.omt

    cd neuromlLocal
    set -ex
    ./clean.sh 
    cd ..

    python regenerate_folder.py --folder exampleRun21W2D
    cd neuromlLocal
    omv test -V .test.21w2d.nrn.omt
    omv test -V .test.21w2d.omt

    cd testc302SigSim
    make clean all
    ./testc302NervousSystem --popString "AS DA DB DD VD VB VA" --popSize 7 --datString "21W2D"
    cd ..
    cd ..

    #./regenerate_21.sh # regenerated NML & runs omv all -V
    #cd ..

    python test2021W2D_nml.py

fi

make tests2

rm -rf test_output_2/*.dat
./tests2