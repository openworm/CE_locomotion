#!/bin/bash
set -ex

make clean
make

rm -rf exampleRun
time python run_main.py -R 1234 -p 6 --doEvol --folderName exampleRun

