#!/bin/bash
set -ex

ruff format *.py
ruff check *.py

python worm2D_modelspec.py 

cat worm2d_example.json
