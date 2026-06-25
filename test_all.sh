#!/bin/bash
set -ex

quick_test=0
parallel_jobs_for_make="4" # Adjust this based on your system's capabilities

if [[ ($# -eq 1) && ($1 == '-q') ]]; then
    quick_test=1
fi

OMV_ARGS=""
# check if running on macos and if so, just run the omv tests ignoring exit codes 
if [[ "$OSTYPE" == "darwin"* ]]; then
    OMV_ARGS=" --exit-zero" # Add this flag to ensure that OMV returns a zero exit code even if some tests fail, allowing the script to continue running all tests.
fi

CMAKE_BUILD_PARALLEL_LEVEL=$parallel_jobs_for_make pip install -e .[all] --no-build-isolation -v

cd tests

# Directly run simple example
python test.py


if [ "$quick_test" == 0 ]; then

    rm -rf ../exampleRunRS18
    rm -rf ../exampleRunRS18W2D
    rm -rf ../testruns/exW2D18
    rm -rf ../testruns/exW2D18gen
    rm -rf ../testruns/exW2DSR18 ../testruns/exW2DSR18_nml ../testruns/exW2DSR18_nml_musc
    rm -rf ../testruns/exW2DSR18E
    rm -rf ../testruns/exW2D18genE
    rm -rf ../testruns/exW2DSR18srm

    rm -rf ../exampleRun
    rm -rf ../exampleRun_nml
    
    rm -rf ../exampleRunNet21
    rm -rf ../exampleRun21W2D
    rm -rf ../exampleRun21W2D_nml
    rm -rf ../experiments/izq_runs_nets/103 ../experiments/izq_runs_nets_nml/103 ../experiments/izq_runs_nets_nml_musc/103
    rm -rf ../experiments/izq_runs_nets/23 ../experiments/izq_runs_nets_nml/23 ../experiments/izq_runs_nets_nml_musc/23
    rm -rf ../experiments/izq_runs_nets_W2D21 ../experiments/izq_runs_nets_nml_W2D21 ../experiments/izq_runs_nets_nml_musc_W2D21
    rm -rf ../testruns/exW2DSR21 ../testruns/exW2DSR21_nml ../testruns/exW2DSR21_nml_musc
    rm -rf ../testruns/exW2D21
    rm -rf ../testruns/exW2D21E
    rm -rf ../testruns/exW2DSRE21
    rm -rf ../testruns/exW2DCEa
    rm -rf ../testruns/exW2DCEanm
    rm -rf ../testruns/exW2DCEanm2 ../testruns/exW2DCEanm2_1

    rm -rf ../exampleRunCEW2D ../exampleRunCEW2D_nml
    
    rm -rf ../exampleRunCOW2D
    rm -rf ../exampleRunCO
    rm -rf ../exampleRunW2DCE


    rm -rf ../testruns/exW2DCEFR ../testruns/exW2DCEFRv2 ../testruns/exW2DSRFR
    rm -rf ../testruns/exW2DCEs ../testruns/exW2DCEE ../testruns/exW2DSRE
    rm -rf ../testruns/exW2DSR ../testruns/exW2DSR_nml ../testruns/exW2DSR_nml_musc
    
    rm -rf ../testruns/exW2DCO
    
    rm -rf ../experiments/osc_sim ../experiments/osc_sim_nml ../experiments/osc_sim_nml_musc
    rm -rf ../experiments/osc_sim_21 ../experiments/osc_sim_21_nml ../experiments/osc_sim_21_nml_musc
    rm -rf ../experiments/osc_sim_21all ../experiments/osc_sim_21all_nml ../experiments/osc_sim_21all_nml_musc
    
    #rm -rf ../testruns/COW2DSREgen ../testruns/COW2DSREgen_out
    rm -rf ../testruns/COW2DSRE_test_out

    
    omv test -V .test.example.omt #Izq original test.example.mep

fi
