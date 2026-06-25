#!/bin/bash
set -ex

./clean.sh 


python regenerate.py --folder ../exampleRun21W2D 

omv test -V .test.21w2d.nrn.omt
omv test -V .test.21w2d.omt

cd testc302SigSim
make clean all
./testc302NervousSystem --popString "AS DA DB DD VD VB VA" --popSize 7 --datString "21W2D"
cd ..



