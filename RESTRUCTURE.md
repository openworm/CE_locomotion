# Repository Restructure

## Goal

Make this repo an installable Python package named `worm2d` (`pip install worm2d`), with the C++ simulation code compiled and installed as part of that process via scikit-build-core + CMake.

---

## Implemented Structure

```
CE_locomotion/
├── src/
│   ├── cpp/                    # all C++ source
│   │   ├── main.cpp, Worm.cpp/h, WormBody.cpp/h, ...   # core simulation files
│   │   ├── tests.cpp, tests2.cpp
│   │   ├── neuromlLocal/       # NeuroML/c302 C++ integration
│   │   ├── Worm2D/             # Worm2D model (main, main_osc)
│   │   ├── RoyalSociety2018/   # RS18 model
│   │   ├── network2021/        # 2021 network model
│   │   └── CE_orientation/     # klinotaxis model
│   └── worm2d/                 # Python package (source tree)
│       ├── __init__.py         # exports run, run_main, get_worm2d_version
│       ├── run_main.py         # main entry point
│       ├── helper_funcs.py
│       ├── load_data.py
│       └── neuromlLocal/       # NeuroML Python subpackage
│           ├── __init__.py
│           ├── utils.py
│           └── ...
│
│   NOTE: C++ binaries are NOT in the source tree. CMake installs them into
│   site-packages/worm2d/bin/ at build time:
│       worm2d/bin/main
│       worm2d/bin/Worm2D/main
│       worm2d/bin/Worm2D/main_osc
│       worm2d/bin/RoyalSociety2018/main
│       worm2d/bin/network2021/main
│       worm2d/bin/CE_orientation/main
├── tests/
│   ├── test*.py                # pytest-compatible test scripts
│   ├── test_all.sh             # shell-based integration test suite
│   └── test_files/             # C++/Python interaction tests
├── CMakeLists.txt              # builds all C++ binaries, installs to worm2d/bin/
├── pyproject.toml              # scikit-build-core build backend
├── README.md
└── CLAUDE.md
```

---

## Build and install

```bash
# Install with C++ build (requires g++, nlohmann-json; cmake/ninja fetched automatically):
pip install .

# Editable install for development (--no-build-isolation reuses already-installed build tools):
CMAKE_BUILD_PARALLEL_LEVEL=4 pip install -e . --no-build-isolation

# Build C++ unit tests manually:
cmake -B build && cmake --build build --target worm2d_tests
./build/tests
```

## Usage after install

```python
from worm2d import run
run(outputFolderName="myRun", doEvol=True, popSize=6)
```

Or via the command line:
```bash
worm2d -R 1233 -p 6 --doEvol --outputFolderName exampleRun
```

---

## Key design decisions

- **No `core/` subdirectory**: core C++ files sit directly at `src/cpp/` so that `Worm2D/` files can use their existing `"../random.h"` relative includes without modification.
- **Binary installation**: CMake installs all compiled executables into `site-packages/worm2d/bin/<model_folder>/`. `run_main.py` resolves them via `importlib.resources.files('worm2d')`; for editable installs (where that points to the source tree rather than site-packages), it falls back to `sysconfig.get_path('platlib')` to locate the binaries in site-packages.
- **neuromlLocal split**: C++ files from `neuromlLocal/` live in `src/cpp/neuromlLocal/`; the Python subpackage lives in `src/worm2d/neuromlLocal/`.
- **`RoyalSociety2018/`, `network2021/`, `CE_orientation/`**: These subdirectories have self-contained copies of the simulation classes and are compiled independently.
