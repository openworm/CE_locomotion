#!/bin/bash
set -ex

ruff format *py

python regenerate.py

omv all -V 

