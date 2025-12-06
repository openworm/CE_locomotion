import sys
from . import utils
import os
# from .utils import *

from .build_network import run as build_network_run

# from build_network import run as build_network_run
from .create_new_lems_file import run as create_new_lems_run


def run_main(args=None):
    if args is None:
        args = utils.process_args()
    run(a=args)


def run(a=None, **kwargs):
    a = utils.build_namespace(utils.DEFAULTS, a, **kwargs)

    # def run(output_folder, doMuscles=False):
    # current = os.path.dirname(os.path.realpath(__file__))

    if a.folder is None:
        print("worm_data.json folder is required for nml generation")
        sys.exit()

    json_file = a.folder + "/worm_data_evo.json"
    if not os.path.isfile(json_file):
        json_file = a.folder + "/worm_data.json"
    if not os.path.isfile(json_file):
        json_file = a.folder + "/worm_data_worm.json"

    print(json_file)

    if hasattr(a, "output_folder"):
        output_folder = getattr(a, "output_folder")
    else:
        output_folder = a.folder

    if hasattr(a, "popstruct"):
        popstruct = getattr(a, "popstruct")
    else:
        popstruct = 2

    population_structures = [
        "one population",
        "individual populations",
        "cell specific populations",
    ]
    population_structure = population_structures[popstruct]
    # json_file = "../exampleRunCEW2D/worm_data.json"
    # json_file = "../exampleRun21W2D/worm_data.json"
    # json_file = "../exampleRunRS18W2D/worm_data.json"
    # json_file = "../exampleRunRS18/worm_data.json"
    build_network_run(
        population_structure=population_structure,
        json_file=json_file,
        output_folder=output_folder,
        doMuscles=a.doMuscles,
    )
    create_new_lems_run(
        population_structure=population_structure,
        json_file=json_file,
        output_folder=output_folder,
        doMuscles=a.doMuscles,
    )


if __name__ == "__main__":
    # folder = sys.argv[1]
    run_main()
