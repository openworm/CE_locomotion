"""Compatibility entry point; prefer :mod:`worm2d.runner`."""

try:
    from worm2d.runner import *  # noqa: F403
    from worm2d.runner import run_main
except ModuleNotFoundError:
    from src.worm2d.runner import *  # noqa: F403
    from src.worm2d.runner import run_main


if __name__ == "__main__":
    run_main()
