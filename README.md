# Worm2D

Worm2D constructs and optimizes configurable neuromechanical models of
_C. elegans_ locomotion. The installable package contains the Python API,
configuration and plotting helpers, and the JSON-configurable `main_osc` C++
simulator. The legacy `main` executable is not part of the package.

## Installation

Worm2D requires a C++ compiler, CMake, Python development headers, and the
`nlohmann-json` headers. From a source checkout, install it with:

```bash
python -m pip install .
```

For editable Python development:

```bash
python -m pip install --editable .
```

## Python API

```python
from worm2d import run

run(
    modelName="W2DSR",
    inputFolderName="input_model",
    outputFolderName="output_model",
    overwrite=True,
)
```

The installed package locates its bundled `main_osc` executable automatically.
The `modelFolder` argument remains available as an explicit development
override. Existing root-level imports such as `from run_main import run` are
temporarily supported by compatibility modules.

See `PACKAGING.md` for the source layout, native build commands, testing, and
migration notes.
