# Worm2D package layout

The installable distribution contains the Python API, the `main_osc`
simulator, and four isolated legacy-model executables retained for regression
and reproducibility tests.

## Source layout

- `src/worm2d/`: public Python package.
- `native/simulator/`: only the simulator and evolution classes required by
  W2D18, W2D21, W2DCE, and W2DSR.
- `native/`: shared body, nervous-system, muscle, search, and utility classes.
- `native/neuromlLocal/`: C++ bridge classes required by `main_osc`.
- `legacy/example/`: original example model and its own `main` executable.
- `legacy/rs18/`: 2018 model and its own `main` executable.
- `legacy/co/`: chemotaxis/orientation model and its own `main` executable.
- `legacy/net21/`: 2021 network model and its own `main` executable.
- `neuromlLocal/`: optional Python NeuroML integration.
- `F2_fig_behavior.py`: behavior plotting used by retained simulation runs.
- `test_all.sh`: the selected `main_osc` and legacy regression suite.

The source distribution includes only the selected OMV tests, these four
legacy model families, and their input fixtures. Other model families,
generated results, papers, notebooks, and historical experiment directories
are not packaged.

The root-level `run_main.py`, `helper_funcs.py`, `load_data.py`, and
`W2Djson_utils.py` modules are compatibility shims for existing notebooks.
New code should import from `worm2d`.

## Building

Build the native executable directly:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target main_osc legacy_example_main legacy_rs18_main \
    legacy_co_main legacy_net21_main
```

Build an installable wheel:

```bash
python -m pip install build
python -m build
```

Test the resulting wheel from a clean virtual environment and from a working
directory outside this repository. This verifies that imports and executable
discovery do not depend on the source checkout.

## History and recovery

Migration commits use `git mv`, so history for moved files is available with
`git log --follow <path>`. The `test_apdp10` branch remains at the pre-migration
commit and can be checked out to recover the former layout.
