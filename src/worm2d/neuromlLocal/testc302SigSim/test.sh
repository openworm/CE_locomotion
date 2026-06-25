#!/bin/bash
set -ex

rm -rf arm64 x86_64 *txt *dat 
make clean
make all

./testc302NervousSystem --popString "DA DB DD VD VA VB" --popSize 10 --datString "CEOrig"
