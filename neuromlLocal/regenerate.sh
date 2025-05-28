#!/bin/bash
set -ex

./clean.sh 

ruff format *py

python regenerate.py

omv all -V 

cd testc302SigSim
make clean all
./testc302NervousSystem
cd ..

