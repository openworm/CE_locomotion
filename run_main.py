import subprocess
import argparse
import os
import sys
import helper_funcs as hf

# from pyneuroml.utils.cli import build_namespace
import random
from datetime import datetime
import json

defaults_base = {
    "popSize": 96,
    "duration": 24,
    "nervousSystemFileName": "main_sim",
    "doNML": 0,
}

DEFAULTS = {
    "popSize": None,  # 96,
    "duration": None,  # 24,
    "RandSeed": None,
    "outputFolderName": None,
    "doEvol": False,
    "overwrite": False,
    "doNML": None,
    "crandSeed": None,
    "inputFolderName": None,
    "nervousSystemFileName": "main_sim",
}


def process_args():
    """Parse command-line arguments.

    :returns: None
    """
    parser = argparse.ArgumentParser(
        description=("A script for supplying arguments to execute Worm2D")
    )

    parser.add_argument(
        "-g",
        "--inputFolderName",
        type=str,
        metavar="<input folder name>",
        default=DEFAULTS["inputFolderName"],
        help=(
            "Optional name of the folder for the default evolution and simulation parameters.\n"
            "This folder will not be altered.\n"
        ),
    )

    parser.add_argument(
        "-f",
        "--outputFolderName",
        type=str,
        metavar="<output folder name>",
        default=DEFAULTS["outputFolderName"],
        help=(
            "Name of directory for output. This must be supplied.\n"
            "If the directory exists overwrite must be\n"
            "set to True and evolution and simulation parameter defaults from it will be used.\n"
            "If an input folder is supplied, these parameter defaults will be overwritten by the input folder ones.\n"
            "Any supplied command line arguments will replace their corresponding defaults.\n"
            "If the directory does not exist and and an input folder is not supplied, initial random seeds and\n"
            "initial parameters will be used. If doEvol is false only the simulation will be performed.\n"
            "If doEvol is true the evolution will also be performed.\n"
        ),
    )

    parser.add_argument(
        "-n",
        "--nervousSystemFileName",
        type=str,
        metavar="<nervous system file name>",
        default=DEFAULTS["nervousSystemFileName"],
        help=("Name of nervous system file for neuroml simulation."),
    )

    parser.add_argument(
        "-N",
        "--doNML",
        action="store_true",
        # metavar="<run NML>",
        default=DEFAULTS["doNML"],
        help=(
            "Run the equivalent neuroML simulation instead of C++ simulation if True."
        ),
    )

    parser.add_argument(
        "-o",
        "--overwrite",
        action="store_true",
        # metavar="<overwrite>",
        default=DEFAULTS["overwrite"],
        help=(
            "Overwrite the results in the folder. If doEvol is set True\n"
            "all results will be overwritten. If doEvol is False\n"
            "only the simulation results will be overwritten."
        ),
    )

    parser.add_argument(
        "-E",
        "--doEvol",
        action="store_true",
        # metavar="<run optimization>",
        default=DEFAULTS["doEvol"],
        help=(
            "If True both evolution and simulation will be performed. If False (the default)\n"
            "just the simulation will be performed."
        ),
    )

    parser.add_argument(
        "-d",
        "--duration",
        type=float,
        metavar="<duration>",
        default=DEFAULTS["duration"],
        help="Duration of simulation for evolution and best worm in ms, reverts to: %sms if not provided."
        % defaults_base["duration"],
    )

    parser.add_argument(
        "-p",
        "--popSize",
        type=int,
        metavar="<pop size>",
        default=DEFAULTS["popSize"],
        help="Population size for evolutionary algorithm, reverts to: %s if not provided."
        % defaults_base["popSize"],
    )

    parser.add_argument(
        "-R",
        "--RandSeed",
        type=int,
        metavar="<rand seed>",
        default=DEFAULTS["RandSeed"],
        help="Seed value for evolution and simulation, or just simulation if doEvol is False."
        "If not set the relevant seed in the input directory will be used."
        "If there is no such seed, a random seed will be generated.",
        # % DEFAULTS["RandSeed"],
    )

    parser.add_argument(
        "-c",
        "--crandSeed",
        type=int,
        metavar="<c rand seed>",
        default=DEFAULTS["crandSeed"],
        help="Seed value relative to system time, (do not use: only included for consistency with original code).",
        # % DEFAULTS["crandSeed"],
    )

    return parser.parse_args()


def make_directory(directory_name, overwrite, str1="the contents"):
    try:
        os.mkdir(directory_name)
        print(f"Directory '{directory_name}' created successfully.")
        return True
    except FileExistsError:
        if overwrite:
            print(
                f"Directory '{directory_name}' already exists and "
                + str1
                + " will be overwritten."
            )
            return True
        else:
            print(
                f"Directory '{directory_name}' already exists and overwrite is false."
            )
            return False
    except PermissionError:
        print(f"Permission denied: Unable to create '{directory_name}'.")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)


def run_main(args=None):
    if args is None:
        args = process_args()
    run(a=args)


def build_namespace(DEFAULTS={}, a=None, **kwargs):
    if a is None:
        a = argparse.Namespace()

    # Add arguments passed in by keyword.
    for key, value in kwargs.items():
        setattr(a, key, value)

    # Add defaults for arguments not provided.
    for key, value in DEFAULTS.items():
        if not hasattr(a, key):
            setattr(a, key, value)

    return a


def setDict(dictval, keyval, parval, default_val):
    init_val = None
    if keyval in dictval:
        init_val = dictval[keyval]
    if parval is not None:
        dictval[keyval] = parval
        return dictval[keyval] == init_val
    if keyval not in dictval:
        dictval[keyval] = default_val
        return False
    return True


def getValFromJson(dictval, keyval):
    return dictval[keyval]["value"]


def run(a=None, **kwargs):
    a = build_namespace(DEFAULTS, a, **kwargs)

    if a.doEvol:
        do_evol = 1
    else:
        do_evol = 0

    if a.inputFolderName is not None:
        if not os.path.isdir(a.inputFolderName):
            print("Input folder does not exist!")
            sys.exit(1)

    if a.outputFolderName is None:
        print(
            "No output folder name. You need to supply an output folder name, and an optional input folder name."
        )
        sys.exit(1)

    if not do_evol and not os.path.isdir(a.outputFolderName):
        print("Setting doEvol to True since the output folder will be created.")
        do_evol = 1

    if do_evol:
        str1 = "all the contents"
    else:
        str1 = "the simulation results"

    if not make_directory(a.outputFolderName, a.overwrite, str1):
        print(
            "Please change output directory name, or set overwrite to True\n"
            "and doEvol to True to overwrite the evolution and simulation results,\n"
            "or set overwrite to True and doEvol to False (the default) if you want\n"
            "to just overwrite the simulation results. Alternatively supply this directory\n"
            "as the `inputFolderName' parameter (which will not be\n"
            "modified), and provide a different novel name for the output directory."
        )
        sys.exit(1)

    if a.inputFolderName is not None and a.inputFolderName != a.outputFolderName:
        import shutil

        files = [
            "fitness.dat",
            "simulation_pars.json",
            "seed.dat",
            "worm_data.json",
            "best.gen.dat",
            "phenotype.dat",
        ]
        for file in files:
            shutil.copyfile(
                a.inputFolderName + "/" + file, a.outputFolderName + "/" + file
            )

    sim_par_file = a.outputFolderName + "/simulation_pars.json"
    if os.path.isfile(sim_par_file):
        with open(sim_par_file) as f:
            sim_data = json.load(f)
    else:
        sim_data = {}

    random.seed(datetime.now().timestamp())
    random_seed = random.randint(1, 1000000)

    evol_data = {}
    evol_pars = ["Duration", "pop_size", "randomseed"]
    evol_args = [a.duration, a.popSize, a.RandSeed]
    evol_defaults = [defaults_base["duration"], defaults_base["popSize"], random_seed]

    evol_par_file = a.outputFolderName + "/worm_data.json"
    if os.path.isfile(evol_par_file):
        with open(evol_par_file) as f:
            worm_data = json.load(f)
            for key in evol_pars:
                if key in worm_data["Evolutionary Optimization Parameters"]:
                    evol_data[key] = worm_data["Evolutionary Optimization Parameters"][
                        key
                    ]["value"]
                else:
                    print("Parameter not found in worm_data.json")

    same_vals = True
    if do_evol:
        for par, arg, default in zip(evol_pars, evol_args, evol_defaults):
            if not setDict(evol_data, par, arg, default):
                same_vals = False
    if do_evol and same_vals:
        print(
            "Evolution not needed as evolution parameters are the same as the existing ones."
        )
        do_evol = False

    do_nml = None
    if a.doNML is not None:
        if a.doNML:
            do_nml = 1
        else:
            do_nml = 0

    same_vals = True
    sim_pars = ["doNML", "seed", "Duration"]
    sim_args = [do_nml, a.RandSeed, a.duration]
    sim_defaults = [defaults_base["doNML"], random_seed, defaults_base["duration"]]
    for par, arg, default in zip(sim_pars, sim_args, sim_defaults):
        if not setDict(sim_data, par, arg, default):
            same_vals = False

    if not do_evol and same_vals:
        print(
            "Simulation not needed as simulation parameters are the same as the existing ones.\n"
            "Please supply new command line arguments."
        )
        sys.exit(1)

    with open(sim_par_file, "w", encoding="utf-8") as f:
        json.dump(sim_data, f, ensure_ascii=False, indent=4)

    # cmd = ["./main",]

    if a.crandSeed is not None:
        cmd = ["./main", "-r", str(a.crandSeed)]
    else:
        cmd = ["./main", "-R", str(evol_data["randomseed"])]

    cmd += ["-sr", str(sim_data["seed"])]
    cmd += ["-p", str(evol_data["pop_size"])]
    cmd += ["-d", str(evol_data["Duration"])]
    cmd += ["-sd", str(sim_data["Duration"])]
    cmd += ["--doevol", str(do_evol)]

    cmd += ["--donml", str(sim_data["doNML"])]
    cmd += ["--folder", str(a.outputFolderName)]

    # Run the C++
    result = subprocess.run(cmd, capture_output=True, text=True)
    # result = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    # output, errors = result.communicate()

    # p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    # print(output)

    if result.stdout:
        print(result.stdout)

    if result.stderr:
        print("Error:")
        print(result.stderr)

    hf.dir_name = a.outputFolderName
    from load_data import reload_single_run

    reload_single_run(show_plot=False)


if __name__ == "__main__":
    run_main()
