"""Compatibility entry point; prefer :mod:`worm2d.runner`."""

try:
    from src.worm2d.runner import *  # noqa: F403
    from src.worm2d.runner import (
        _resolve_evolution_run_controls,
        _store_evolution_run_controls,
        run_main,
    )
except ModuleNotFoundError:
    from worm2d.runner import *  # noqa: F403
    from worm2d.runner import (
        _resolve_evolution_run_controls,  # noqa: F401
        _store_evolution_run_controls,  # noqa: F401
        run_main,
    )


if __name__ == "__main__":
    run_main()
