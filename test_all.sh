#!/bin/bash
set -ex

quick_test=0
parallel_jobs_for_make="-j4" # Adjust this based on your system's capabilities

if [[ ($# -eq 1) && ($1 == '-q') ]]; then
    quick_test=1
fi

OMV_ARGS=""
# check if running on macos and if so, just run the omv tests ignoring exit codes 
if [[ "$OSTYPE" == "darwin"* ]]; then
    OMV_ARGS=" --exit-zero" # Add this flag to ensure that OMV returns a zero exit code even if some tests fail, allowing the script to continue running all tests.
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
make ${parallel_jobs_for_make}
make main_osc
cd ..

ruff format *.py */*.py
ruff check *.py */*.py

if [ "$quick_test" == 0 ]; then

    rm -rf exampleRunRS18
    rm -rf exampleRunRS18W2D
    rm -rf testruns/exW2D18
    rm -rf testruns/exW2D18gen
    rm -rf testruns/exW2DSR18 testruns/exW2DSR18_nml testruns/exW2DSR18_nml_musc
    rm -rf testruns/exW2DSR18E
    rm -rf testruns/exW2D18genE
    rm -rf testruns/exW2DSR18srm

    rm -rf exampleRun
    rm -rf exampleRun_nml
    
    rm -rf exampleRunNet21
    rm -rf exampleRun21W2D
    rm -rf exampleRun21W2D_nml
    rm -rf experiments/izq_runs_nets/103 experiments/izq_runs_nets_nml/103 experiments/izq_runs_nets_nml_musc/103
    rm -rf experiments/izq_runs_nets/23 experiments/izq_runs_nets_nml/23 experiments/izq_runs_nets_nml_musc/23
    rm -rf experiments/izq_runs_nets_W2D21 experiments/izq_runs_nets_nml_W2D21 experiments/izq_runs_nets_nml_musc_W2D21
    rm -rf testruns/exW2DSR21 testruns/exW2DSR21_nml testruns/exW2DSR21_nml_musc
    rm -rf testruns/exW2D21
    rm -rf testruns/exW2D21E
    rm -rf testruns/exW2DSRE21
    rm -rf testruns/exW2DCEa
    rm -rf testruns/exW2DCEanm
    rm -rf testruns/exW2DCEanm2 testruns/exW2DCEanm2_1

    rm -rf exampleRunCEW2D exampleRunCEW2D_nml
    
    rm -rf exampleRunCOW2D
    rm -rf exampleRunCO
    rm -rf exampleRunW2DCE


    rm -rf testruns/exW2DCEFR testruns/exW2DCEFRv2 testruns/exW2DSRFR
    rm -rf testruns/exW2DCEs testruns/exW2DCEE testruns/exW2DSRE
    rm -rf testruns/exW2DSR testruns/exW2DSR_nml testruns/exW2DSR_nml_musc
    
    rm -rf testruns/exW2DCO
    
    rm -rf experiments/osc_sim experiments/osc_sim_nml experiments/osc_sim_nml_musc
    rm -rf experiments/osc_sim_21 experiments/osc_sim_21_nml experiments/osc_sim_21_nml_musc
    rm -rf experiments/osc_sim_21all experiments/osc_sim_21all_nml experiments/osc_sim_21all_nml_musc
    
    #rm -rf testruns/COW2DSREgen testruns/COW2DSREgen_out
    rm -rf testruns/COW2DSRE_test_out

    
    omv test -V .test.example.omt #Izq original test.example.mep
    #omv test -V .test.CEW2D.omt #main.cpp test.example.mep
    omv test -V .test.W2DCEa.omt #main_osc.cpp test.example.mep
    omv test -V .test.W2DCEanm.omt #main_osc.cpp test.W2DCEanm.mep, as W2DCEa but origMusc=False
    omv test -V .test.W2DCEanm2.omt
    omv test -V .test.W2DCE.omt
    omv test -V .test.W2DCEs.omt
    omv test -V .test.W2DSR.omt
    omv test -V .test.W2DSRE.omt
    omv test -V .test.W2DCEE.omt
    omv test -V .test.W2DCEFR.omt ${OMV_ARGS}
    omv test -V .test.W2DCEFRv2.omt ${OMV_ARGS}
    omv test -V .test.W2DSRFR.omt ${OMV_ARGS}
    omv test -V .test.CEW2D_all.omt #main.cpp test.example.mep
   


    omv test -V .test.COW2DSR2.omt
   
    omv test -V .test.osc_sim.omt ${OMV_ARGS}
    omv test -V .test.osc_sim_21.omt 
    omv test -V .test.osc_sim_21all.omt 


    omv test -V .test.2018.omt ${OMV_ARGS}  #Izq original .test.2018.mep
    omv test -V .test.2018W2D.omt ${OMV_ARGS} #main.cpp .test.2018.mep
    omv test -V .test.W2D18.omt ${OMV_ARGS} #main_osc.cpp.test.2018.mep



    omv test -V .test.W2D18gen.omt ${OMV_ARGS} #origMusc=False .test.2018gen.mep
    omv test -V .test.W2DSR18.omt ${OMV_ARGS} #.test.2018gen.mep inputFolderName="testruns/exW2D18gen",
    omv test -V .test.W2DSR18E.omt ${OMV_ARGS} #inputFolderName="testruns/exW2D18gen",
    omv test -V .test.W2D18genE.omt ${OMV_ARGS}
    omv test -V .test.W2DSR18srm.omt ${OMV_ARGS}

    

    omv test -V .test.CO.omt 
    omv test -V .test.COW2D.omt 
    omv test -V .test.W2DCO.omt
    #omv test -V .test.COW2DSR.omt
    
    omv test -V .test.2021.omt 
    omv test -V .test.2021W2D.omt 
    omv test -V .test.izq_sim.omt 
    
    omv test -V .test.izq_sim_W2D21.omt
    omv test -V .test.W2D21.omt 
    omv test -V .test.W2D21E.omt 
    omv test -V .test.W2DSRE21.omt 
    omv test -V .test.W2DSR21.omt

   
    
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
    #omv test -V .test.CEW2D_nml.omt

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