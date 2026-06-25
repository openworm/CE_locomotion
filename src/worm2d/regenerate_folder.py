import os
import sys
from .neuromlLocal.regenerate import run as regenerate_run
from .neuromlLocal import utils


def run_main(args=None):
    if args is None:
        args = utils.process_args()
    run(a=args)


def run(a=None, **kwargs):
    a = utils.build_namespace(utils.DEFAULTS, a, **kwargs)

    if a.folder is None:
        print("folder is required for nml generation")
        sys.exit()

    try:
        os.chdir("./neuromlLocal")
    except Exception:
        print("Can't change to neuromlLocal.")
        print(sys.exc_info())

    regenerate_run(folder="../" + a.folder)
    os.chdir("../")


if __name__ == "__main__":
    run_main()
