#!/bin/bash
set -e

rm -rf arm64 x86_64 __pycache__

mv *.dat *.mod *_nrn.py *gv* report.txt /tmp || true
