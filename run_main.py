import subprocess
import argparse
import os
import sys
import helper_funcs as hf
# from pyneuroml.utils.cli import build_namespace

DEFAULTS = {
    "popSize": 96,
    "duration": 24,
    "simsep": False,
    "RandSeed": 42,
    "randSeed": 42,
    "folderName": None,
    "doEvol": False,
    "overwrite": False,
    "nervousSystemFileName" : 'NervousSystem'
}


def process_args():
    """Parse command-line arguments.

    :returns: None
    """
    parser = argparse.ArgumentParser(
        description=("A script for supplying arguments to execute Worm2D")
    )

    parser.add_argument(
        "-f",
        "--folderName",
        type=str,
        metavar="<folder name>",
        default=DEFAULTS["folderName"],
        help=(
            "Name of directory for output.\n"
            "If not supplied, both evolutionary algorithm and simulation of best worm are performed,\n"
            "and results placed in current directory."
        ),
    )

    parser.add_argument(
        "-n",
        "--nervousSystemFileName",
        type=str,
        metavar="<nervous system file name>",
        default=DEFAULTS["nervousSystemFileName"],
        help=(
            "Name of nervous system file name.\n" 
            "If none entered the original worm simulation will be run.\n"
        ),
    )

    parser.add_argument(
        "-o",
        "--overwrite",
        action="store_true",
        default=DEFAULTS["overwrite"],
        help=("Overwrite the contents of the specified simulation output directory."),
    )

    parser.add_argument(
        "-E",
        "--doEvol",
        action="store_true",
        default=DEFAULTS["doEvol"],
        help=(
            "If used and a directory name has also been supplied, the directory is created,"
            "the evolutionary algorithm is executed, the best worm simulation performed,"
            "and results are deposited in the directory."
            "If not used but an existing directory name has been supplied, the simulation"
            "in the directory is executed and results deposited in it."
        ),
    )

    parser.add_argument(
        "-S",
        "--simsep",
        action="store_true",
        default=DEFAULTS["simsep"],
        help=("If used, user input of the directory name is interactively requested."),
    )

    parser.add_argument(
        "-d",
        "--duration",
        type=float,
        metavar="<duration>",
        default=DEFAULTS["duration"],
        help="Duration of simulation for evolution and best worm in ms, default: %sms"
        % DEFAULTS["duration"],
    )

    parser.add_argument(
        "-p",
        "--popSize",
        type=int,
        metavar="<pop size>",
        default=DEFAULTS["popSize"],
        help="Population size for evolutionary algorithm, default: %s"
        % DEFAULTS["popSize"],
    )

    parser.add_argument(
        "-R",
        "--RandSeed",
        type=int,
        metavar="<Rand seed>",
        default=DEFAULTS["RandSeed"],
        help="Absolute seed value for evolutionary algorithm, default: %s"
        % DEFAULTS["RandSeed"],
    )

    parser.add_argument(
        "-r",
        "--randSeed",
        type=int,
        metavar="<rand seed>",
        default=DEFAULTS["randSeed"],
        help="Seed value relative to system time for evolutionary algorithm, default: %s"
        % DEFAULTS["randSeed"],
    )

    return parser.parse_args()


def make_directory(directory_name, overwrite):
    try:
        os.mkdir(directory_name)
        print(f"Directory '{directory_name}' created successfully.")
        return True
    except FileExistsError:
        if overwrite:
            print(
                f"Directory '{directory_name}' already exists and contents will be overwritten."
            )
            return True
        else:
            print(f"Directory '{directory_name}' already exists.")
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


def run(a=None, **kwargs):
    a = build_namespace(DEFAULTS, a, **kwargs)

    
    folder_name = ""
    do_evol = 1

    if a.simsep:
        while True:
            do_evol_str = input(
                "Do you want to perform an evolutionary search (E) or run a simulation (S)? "
            )
            if do_evol_str == "E":
                while True:
                    folder_name = input(
                        "Please enter the name of a folder to store data: "
                    )
                    if make_directory(folder_name, a.overwrite):
                        break
                break
            if do_evol_str == "S":
                do_evol = 0
                while True:
                    folder_name = str(
                        input("Please enter the name of a folder to read data: ")
                    )
                    if os.path.isdir(folder_name):
                        break
                    print("Folder does not exist.")
                print("Running simulation.")
                break
    elif a.folderName:
        folder_name = a.folderName
        if a.doEvol:
            if not make_directory(folder_name, a.overwrite):
                sys.exit(1)
        else:
            do_evol = 0
            if not os.path.isdir(folder_name):
                print(f"Directory '{folder_name}' does not exist.")
                sys.exit(1)
    else:
        print("Running in default mode.")
    
    if a.RandSeed is not None:
        cmd = ["./main", "-R", str(a.RandSeed)]
    else:
        cmd = ["./main", "-r", str(a.randSeed)]

    cmd += [
        "-p",
        str(a.popSize),
        "-d",
        str(a.duration),
        "--doevol",
        str(do_evol),
        "--folder",
        folder_name,
        "--nervous",
        a.nervousSystemFileName
    ]

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

    if folder_name != "":
        # if args.simsep or args.evolve_folder or args.sim_folder:
        hf.dir_name = folder_name
        from load_data import reload_single_run

        reload_single_run(show_plot=False)


if __name__ == "__main__":
    run_main()
