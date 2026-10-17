# Worm2D package layout

The installable distribution contains the Python API and the `main_osc`
simulator. It intentionally excludes the legacy `main` executable and tests
which exercise that executable.

## Source layout

- `src/worm2d/`: public Python package.
- `native/simulator/`: JSON-configurable simulator and evolution classes.
- `native/`: shared body, nervous-system, muscle, search, and utility classes.
- `native/neuromlLocal/`: C++ bridge classes required by `main_osc`.
- `neuromlLocal/`: optional Python NeuroML integration.
- `tests/unit/`: Python unit tests for the installable package.

The root-level `run_main.py`, `helper_funcs.py`, `load_data.py`, and
`W2Djson_utils.py` modules are compatibility shims for existing notebooks.
New code should import from `worm2d`.

## Building

Build the native executable directly:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target main_osc
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
