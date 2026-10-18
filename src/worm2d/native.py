"""Locate packaged Worm2D executables."""

from __future__ import annotations

import os
import shutil
from pathlib import Path


def _is_executable(path: Path) -> bool:
    return path.is_file() and os.access(path, os.X_OK)


def resolve_main_osc(model_folder=None) -> Path:
    """Return the requested or installed ``main_osc`` executable path."""
    candidates = []
    if model_folder:
        candidates.append(Path(model_folder).expanduser() / "main_osc")

    package_dir = Path(__file__).resolve().parent
    candidates.append(package_dir / "bin" / "main_osc")

    # Source-tree fallbacks support editable installs and existing notebooks.
    repository_root = package_dir.parents[1]
    candidates.extend(
        [
            repository_root / "native" / "build" / "main_osc",
            repository_root / "build" / "main_osc",
            repository_root / "Worm2D" / "main_osc",
        ]
    )

    command = shutil.which("worm2d-main-osc") or shutil.which("main_osc")
    if command:
        candidates.append(Path(command))

    for candidate in candidates:
        if _is_executable(candidate):
            return candidate.resolve()

    searched = "\n  - ".join(str(path) for path in candidates)
    raise FileNotFoundError(
        "Could not find the Worm2D main_osc executable. Searched:\n  - "
        + searched
        + "\nBuild it with 'cmake --build build' or reinstall Worm2D."
    )


_LEGACY_MODEL_DIRECTORIES = {
    ".": "example",
    "example": "example",
    "legacy/example": "example",
    "RoyalSociety2018": "rs18",
    "rs18": "rs18",
    "legacy/rs18": "rs18",
    "CE_orientation": "co",
    "co": "co",
    "legacy/co": "co",
    "network2021": "net21",
    "net21": "net21",
    "legacy/net21": "net21",
}


def resolve_legacy_main(model_folder) -> Path:
    """Return the legacy ``main`` executable for ``model_folder``."""
    requested = Path(model_folder).expanduser()
    requested_executable = requested / "main"
    if _is_executable(requested_executable):
        return requested_executable.resolve()

    model_key = _LEGACY_MODEL_DIRECTORIES.get(str(model_folder))
    if model_key is None:
        model_key = _LEGACY_MODEL_DIRECTORIES.get(requested.as_posix())
    if model_key is None:
        model_key = _LEGACY_MODEL_DIRECTORIES.get(requested.name)
    if model_key is None:
        raise ValueError(f"Unknown legacy Worm2D model folder: {model_folder}")

    package_dir = Path(__file__).resolve().parent
    repository_root = package_dir.parents[1]
    candidates = [
        package_dir / "legacy" / model_key / "main",
        repository_root / "build" / "legacy" / model_key / "main",
        repository_root / "legacy" / model_key / "main",
    ]
    for candidate in candidates:
        if _is_executable(candidate):
            return candidate.resolve()

    searched = "\n  - ".join(str(path) for path in candidates)
    raise FileNotFoundError(
        f"Could not find the Worm2D legacy '{model_key}' executable. Searched:\n  - "
        + searched
        + "\nBuild it with 'cmake --build build' or reinstall Worm2D."
    )
