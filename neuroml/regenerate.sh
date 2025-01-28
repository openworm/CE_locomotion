#!/bin/bash
set -ex

python regenerate.py

omv all -V 

