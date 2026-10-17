"""Compatibility module; prefer :mod:`worm2d.load_data`."""

try:
    from worm2d.load_data import *  # noqa: F401,F403
except ModuleNotFoundError:
    from src.worm2d.load_data import *  # noqa: F401,F403
