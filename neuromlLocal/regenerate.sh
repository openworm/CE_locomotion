#!/bin/bash
set -ex

./clean.sh 

ruff format *py

python regenerate.py ../exampleRun

#omv all -V 
omv test -V .test.w2d.nrn.omt
omv test -V .test.w2d.omt

python regenerate.py ../exampleRunCEW2D

omv test -V .test.w2d.nrn.omt
omv test -V .test.w2d.omt

cd testc302SigSim
make clean all
./testc302NervousSystem --popString "DA DB DD VD VA VB" --popSize 10 --datString "CEOrig"
cd ..

