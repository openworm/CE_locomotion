#!/bin/bash
set -ex

make clean
make

rm *dat ExampleActivity.png

./main
python load_data.py