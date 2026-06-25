from importlib.metadata import version as _pkg_version, PackageNotFoundError as _PNFError
try:
    __version__ = "v" + _pkg_version("worm2d")
except _PNFError:
    __version__ = "v0.0.0"

from .run_main import run, run_main  # noqa: F401
from .helper_funcs import get_worm2d_version  # noqa: F401
