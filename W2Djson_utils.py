"""Compatibility module; prefer :mod:`worm2d.json_utils`."""

try:
    from src.worm2d.json_utils import *  # noqa: F401,F403
except ModuleNotFoundError:
    from worm2d.json_utils import *  # noqa: F401,F403
