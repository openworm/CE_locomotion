#!/bin/bash
set -ex

./main -R 292 -p 5 -d 50 -t 10 --maxgens 5 -sd 50 -st 10 --doevol 1 -cpt 0 --dorandinit 0 --donml 0 --folder testRS18 --modelname RS18 --domusc 0 -docpt 1 --evoType Evo21

python ../load_data.py testRS18/