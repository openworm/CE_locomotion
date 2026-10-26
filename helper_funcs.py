"""Compatibility module; prefer :mod:`worm2d.helper_funcs`."""

try:
    from src.worm2d.helper_funcs import *  # noqa: F401,F403
except ModuleNotFoundError:
    from worm2d.helper_funcs import *  # noqa: F401,F403
