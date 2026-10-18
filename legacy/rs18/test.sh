#!/bin/bash
set -ex

./main -R 292 -p 6 -d 50 -t 10 --maxgens 6 -sd 50 -st 10 --doevol 1 -cpt 0 --dorandinit 0 --donml 0 --folder testRS18 --modelname RS18 --domusc 0 -docpt 1 --evo_type Evo21

python ../load_data.py testRS18/
