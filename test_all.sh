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

python test.py
