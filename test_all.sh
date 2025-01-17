#!/bin/bash
set -ex

make clean
make tests

rm -rf test_output/*.dat
./tests

make

rm -rf exampleRun
#time python run_main.py -R 1233 -p 96 --doEvol --folderName exampleRun

time omv all -V 
