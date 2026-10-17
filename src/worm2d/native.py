"""Locate the packaged main_osc executable."""

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
