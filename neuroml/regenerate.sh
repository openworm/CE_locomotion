#!/bin/bash
set -ex

python regenerate.py

pynml LEMS_Worm2D.xml -nogui

