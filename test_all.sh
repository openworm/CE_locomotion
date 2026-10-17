#!/bin/bash
set -euo pipefail

quick_test=0
if [[ $# -eq 1 && $1 == "-q" ]]; then
    quick_test=1
fi

omv_args=()
if [[ ${OSTYPE:-} == darwin* ]]; then
    omv_args+=(--exit-zero)
fi

# The package and this regression suite use only the JSON-configurable
# main_osc executable.
make -C Worm2D clean
make -C Worm2D -j4 main_osc

ruff check \
    src/worm2d \
    run_main.py helper_funcs.py load_data.py W2Djson_utils.py F2_fig_behavior.py \
    testW2D18.py testW2D18gen.py testW2DSR18.py testW2DSR18E.py \
    testW2D18genE.py testW2DSR18srm.py run_izq_sims_W2D21.py \
    testW2D21.py testW2D21E.py testW2DSRE21.py testW2DSR21.py \
    testW2DCE.py testW2DCEs.py testW2DSR.py testW2DSRE.py testW2DCEE.py \
    testW2DCEFR.py testW2DCEFRv2.py testW2DSRFR.py

if [[ $quick_test -eq 1 ]]; then
    exit 0
fi

rm -rf \
    testruns/W2D18 testruns/W2D18gen testruns/W2DSR18 \
    testruns/W2DSR18E testruns/W2D18genE testruns/W2DSR18srm \
    testruns/izq_sim_W2D21 testruns/W2D21 testruns/W2D21E \
    testruns/W2DSRE21 testruns/W2DSR21 testruns/W2DCE \
    testruns/W2DCEs testruns/W2DSR testruns/W2DSRE \
    testruns/W2DCEE testruns/W2DCEFR testruns/W2DCEFRv2 \
    testruns/W2DSRFR

# CE-derived models.
omv test -V .test.W2DCE.omt "${omv_args[@]}"
omv test -V .test.W2DCEs.omt "${omv_args[@]}"
omv test -V .test.W2DSR.omt "${omv_args[@]}"
omv test -V .test.W2DSRE.omt "${omv_args[@]}"
omv test -V .test.W2DCEE.omt "${omv_args[@]}"
omv test -V .test.W2DCEFR.omt "${omv_args[@]}"
omv test -V .test.W2DCEFRv2.omt "${omv_args[@]}"
omv test -V .test.W2DSRFR.omt "${omv_args[@]}"

# 2018-derived models.
omv test -V .test.W2D18.omt "${omv_args[@]}"
omv test -V .test.W2D18gen.omt "${omv_args[@]}"
omv test -V .test.W2DSR18.omt "${omv_args[@]}"
omv test -V .test.W2DSR18E.omt "${omv_args[@]}"
omv test -V .test.W2D18genE.omt "${omv_args[@]}"
omv test -V .test.W2DSR18srm.omt "${omv_args[@]}"

# 2021-derived models. The first test creates the shared source model.
omv test -V .test.izq_sim_W2D21.omt "${omv_args[@]}"
omv test -V .test.W2D21.omt "${omv_args[@]}"
omv test -V .test.W2D21E.omt "${omv_args[@]}"
omv test -V .test.W2DSRE21.omt "${omv_args[@]}"
omv test -V .test.W2DSR21.omt "${omv_args[@]}"
