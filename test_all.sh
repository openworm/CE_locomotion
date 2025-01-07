#!/bin/bash
set -ex

make clean
make tests

./tests

make

rm -rf exampleRun
#time python run_main.py -R 1233 -p 96 --doEvol --folderName exampleRun

time omv all -V 
