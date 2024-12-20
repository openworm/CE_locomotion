#!/bin/bash
set -ex

make clean
make

rm -rf exampleRun
time python run_main.py -R 1234 -p 96 --doEvol --folderName exampleRun

