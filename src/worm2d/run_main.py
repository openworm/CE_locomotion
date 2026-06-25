import subprocess
import argparse
import os
import sys
import importlib.resources
from .neuromlLocal import utils
from . import helper_funcs as hf

# from importlib import import_module
import shutil
import glob
import pathlib

# from pyneuroml.utils.cli import build_namespace
import random
from datetime import datetime
import json


def _resolve_binary(model_folder: str, binary_name: str) -> str:
    """Return the path to an installed C++ binary inside the worm2d package."""
    try:
        pkg_dir = pathlib.Path(str(importlib.resources.files("worm2d")))
    except AttributeError:
        import pkg_resources
        pkg_dir = pathlib.Path(pkg_resources.resource_filename("worm2d", ""))
    bin_dir = pkg_dir / "bin"
    # Editable installs (scikit-build-core): Python sources stay in the source
    # tree but CMake outputs land in site-packages — fall back there if needed.
    if not bin_dir.is_dir():
        import sysconfig
        for scheme in ("platlib", "purelib"):
            candidate = pathlib.Path(sysconfig.get_path(scheme)) / "worm2d" / "bin"
            if candidate.is_dir():
                bin_dir = candidate
                break
    if model_folder in ("", "."):
        return str(bin_dir / binary_name)
    return str(bin_dir / model_folder / binary_name)


defaults_base_CO = {
    "popSize": 26,
    "duration": 50,
    "transient": 50,
    "simduration": 50,
    "simtransient": 50,
    "nervousSystemFileName": "main_sim",
    "doNML": 0,
    "doRandInit": 0,
    "maxGens": 40,
    "doMuscSim": 0,
    "evo_type": "Evo18",
}

defaults_base_celoc = {
    "popSize": 96,
    "duration": 24,
    "transient": 8,
    "simduration": 24,
    "simtransient": 8,
    "nervousSystemFileName": "main_sim",
    "doNML": 0,
    "doRandInit": 0,
    "maxGens": 10,
    "doMuscSim": 0,
    "evo_type": "EvoCE",
    "MutVar": 0.05,
    "CrossProb": 0.5,
}

defaults_base_2018 = {
    "popSize": 96,
    "duration": 50,
    "transient": 10,
    "simduration": 50,
    "simtransient": 10,
    "nervousSystemFileName": "main_sim",
    "doNML": 0,
    "doRandInit": 0,
    "maxGens": 1000,
    "doMuscSim": 0,
    "evo_type": "Evo18",
}

defaults_base_CO18 = {
    "popSize": 96,
    "duration": 50,
    "transient": 10,
    "simduration": 50,
    "simtransient": 10,
    "nervousSystemFileName": "main_sim",
    "doNML": 0,
    "doRandInit": 0,
    "maxGens": 1000,
    "doMuscSim": 0,
    "evo_type": "Evo18",
}


defaults_base_2021 = {
    "popSize": 100,
    "duration": 40,
    "transient": 10,
    "simduration": 40,
    "simtransient": 10,
    "nervousSystemFileName": "main_sim",
    "doNML": 0,
    "doRandInit": 0,
    "maxGens": 2000,
    "doMuscSim": 0,
    "evo_type": "Evo21",
}


DEFAULTS = {
    "popSize": None,  # 96,
    "duration": None,  # 24,
    "transient": None,
    "simduration": None,  # 24,
    "simtransient": None,
    "RandSeed": None,
    "outputFolderName": None,
    "doEvol": False,
    "overwrite": False,
    "doNML": None,
    "doMuscSim": None,
    "doRandInit": None,
    "crandSeed": None,
    "inputFolderName": None,
    "nervousSystemFileName": "main_sim",
    "mainProcessName": "main",
    "modelFolder": ".",
    "maxGens": None,
    "modelName": None,
    "reRand": False,
    "checkPointInterval": 1,
    "doCPT": True,
    "evo_type": "Evo21",
    # "MutVar" : 0.1,
    # "CrossProb" : 0.5
}


def process_args():
    """Parse command-line arguments.

    :returns: None
    """
    parser = argparse.ArgumentParser(
        description=(
            "A script for supplying arguments to execute Worm2D "
            + hf.get_worm2d_version()
        )
    )

    parser.add_argument(
        "-O",
        "--modelName",
        type=str,
        metavar="<model name>",
        default=DEFAULTS["modelName"],
        help=(
            "Name of model, required if Worm2D is the model folder.\n"
            "Options include: RS18, CE, Net21."
            # "Default is: %s" % DEFAULTS["modelName"]
        ),
    )

    parser.add_argument(
        "-ET",
        "--evo_type",
        type=str,
        metavar="<evo_type>",
        default=DEFAULTS["evo_type"],
        help=(
            "Name of evolution function.\nOptions include: Evo21, Evo18"
            # "Default is: %s" % DEFAULTS["modelName"]
        ),
    )
    parser.add_argument(
        "--evoType",
        type=str,
        dest="evo_type",
        default=argparse.SUPPRESS,
        help=argparse.SUPPRESS,
    )

    parser.add_argument(
        "-M",
        "--mainProcessName",
        type=str,
        metavar="<main process name>",
        default=DEFAULTS["mainProcessName"],
        help=("Name of main process, default: %s" % DEFAULTS["mainProcessName"]),
    )

    parser.add_argument(
        "-T",
        "--modelFolder",
        type=str,
        metavar="<model folder name>",
        default=DEFAULTS["modelFolder"],
        help=(
            "Name of model code folder. Default is the current folder.\n"
            "Other options include `RoyalSociety2018', 'network2021' and 'Worm2D'\n"
        ),
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
            "Run the equivalent neuroML simulation without muscles instead of C++ simulation if True."
        ),
    )

    parser.add_argument(
        "-cpt",
        "--doCPT",
        action="store_true",
        # metavar="<run NML>",
        default=DEFAULTS["doCPT"],
        help=(
            "Start evolution from checkpoint file if found, default true. Remove checkpoint file if false."
        ),
    )

    parser.add_argument(
        "-cpti",
        "--checkPointInterval",
        type=int,
        metavar="<checkPointInterval>",
        default=DEFAULTS["checkPointInterval"],
        help=(
            "Store evolution checkpoint file to resume search later at this interval."
            "If a checkpoint file is found search will be started from that."
        ),
    )

    parser.add_argument(
        "-rr",
        "--reRand",
        action="store_true",
        # metavar="<run NML>",
        default=DEFAULTS["reRand"],
        help=(
            "If true a new random seed is generated to replace the filed value. Ignored if RandSeed is set."
        ),
    )

    parser.add_argument(
        "-U",
        "--doMuscSim",
        action="store_true",
        # metavar="<run NML>",
        default=DEFAULTS["doMuscSim"],
        help=(
            "Run the equivalent neuroML muscle simulation with muscles instead of C++ simulation if True"
        ),
    )

    parser.add_argument(
        "-i",
        "--doRandInit",
        action="store_true",
        # metavar="<run NML>",
        default=DEFAULTS["doRandInit"],
        help=("Use seed to initialize the simulation initial condition."),
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
        help="Duration of simulation for evolution in ms.",
    )

    parser.add_argument(
        "-t",
        "--transient",
        type=float,
        metavar="<transient>",
        default=DEFAULTS["transient"],
        help="Duration of transient for evolution in ms.",
    )

    parser.add_argument(
        "-sd",
        "--simduration",
        type=float,
        metavar="<simduration>",
        default=DEFAULTS["simduration"],
        help="Duration of simulation for best worm in ms.",
    )

    parser.add_argument(
        "-st",
        "--simtransient",
        type=float,
        metavar="<simtransient>",
        default=DEFAULTS["simtransient"],
        help="Duration of transient for best worm in ms.",
    )

    parser.add_argument(
        "-p",
        "--popSize",
        type=int,
        metavar="<pop size>",
        default=DEFAULTS["popSize"],
        help="Population size for evolutionary algorithm.",
    )

    parser.add_argument(
        "-G",
        "--maxGens",
        type=int,
        metavar="<max generations>",
        default=DEFAULTS["maxGens"],
        help="Maximum number of generations for evolutionary algorithm.",
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

    """     parser.add_argument(
        "-muva",
        "--MutVar",
        type=float,
        metavar="<MutVar>",
        default=DEFAULTS["MutVar"],
        help="Mutation Variance for evolution.",
    )

    parser.add_argument(
        "-crpr",
        "--CrossProb",
        type=float,
        metavar="<CrossProb>",
        default=DEFAULTS["CrossProb"],
        help="Crossover probability for evolution.",
    ) """

    return parser.parse_args()


def run_main(args=None):
    if args is None:
        args = process_args()
    run(a=args)


def build_namespace(DEFAULTS={}, a=None, **kwargs):
    if a is None:
        a = argparse.Namespace()

    provided_args = set(vars(a).keys())

    # Add arguments passed in by keyword.
    for key, value in kwargs.items():
        setattr(a, key, value)
        provided_args.add(key)

    # Add defaults for arguments not provided.
    for key, value in DEFAULTS.items():
        if not hasattr(a, key):
            setattr(a, key, value)

    a._provided_args = provided_args
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


def TFtoInt(val):
    if val is True:
        return 1
    if val is False:
        return 0
    return val
    print("TFtoInt error")
    sys.exit(1)


JSON_CONFIG_FILES = ["worm_data_worm.json", "worm_data_evo.json", "worm_data.json"]


def _evotag_to_string(evotag):
    if isinstance(evotag, int):
        return "evotag_" + str(evotag)
    return str(evotag)


def _active_evotags_from_ranges(evolvable_ranges):
    if not isinstance(evolvable_ranges, dict):
        return []

    tags = []
    if isinstance(evolvable_ranges.get("value"), list):
        for index, entry in enumerate(evolvable_ranges["value"], start=1):
            if not isinstance(entry, dict):
                continue
            if "evotag" in entry:
                body = entry
                tag = _evotag_to_string(entry["evotag"])
            elif len(entry) == 1:
                tag, body = next(iter(entry.items()))
            else:
                tag = "evotag_" + str(index)
                body = entry
            if not isinstance(body, dict) or body.get("active", True):
                tags.append(tag)
        return tags

    for tag, body in evolvable_ranges.items():
        if tag == "value" or not isinstance(body, dict):
            continue
        if body.get("active", True):
            tags.append(tag)
    return tags


def _evolved_used_from_json(json_data):
    evolved_used = json_data.get("evolved_used", {})
    if isinstance(evolved_used, dict):
        evolved_used = evolved_used.get("value", [])
    if not isinstance(evolved_used, list):
        return []
    return [_evotag_to_string(tag) for tag in evolved_used]


def _get_run_config_json(folder_name):
    for filename in JSON_CONFIG_FILES:
        path = os.path.join(folder_name, filename)
        if os.path.isfile(path):
            with open(path, "r", encoding="utf-8") as f:
                return json.load(f), path
    return None, None


def _same_evotag_set(active_tags, evolved_used_tags):
    return (
        len(active_tags) > 0
        and len(evolved_used_tags) > 0
        and len(active_tags) == len(set(active_tags))
        and len(evolved_used_tags) == len(set(evolved_used_tags))
        and set(active_tags) == set(evolved_used_tags)
    )


def _can_copy_previous_evolution_files(input_folder, output_folder):
    output_json, output_path = _get_run_config_json(output_folder)
    if output_json is None or "evolvable_ranges" not in output_json:
        return True

    active_tags = _active_evotags_from_ranges(output_json["evolvable_ranges"])
    used_tags = _evolved_used_from_json(output_json)

    if not used_tags:
        input_json, _ = _get_run_config_json(input_folder)
        if input_json is not None:
            used_tags = _evolved_used_from_json(input_json)

    if not used_tags:
        return True

    if _same_evotag_set(active_tags, used_tags):
        return True

    print(
        "Input evolution history will not be copied because active "
        "evolvable_ranges evotags in " + output_path + " do not match evolved_used."
    )
    return False


def run(a=None, **kwargs):
    a = build_namespace(DEFAULTS, a, **kwargs)

    if a.doEvol:
        do_evol = 1
    else:
        do_evol = 0

    outputFolderName = a.outputFolderName
    if a.inputFolderName is not None:
        if not os.path.isdir(a.inputFolderName):
            print("Input folder does not exist!")
            sys.exit(1)

    if a.outputFolderName is None:
        print(
            "No output folder name. You need to supply an output folder name, and an optional input folder name."
        )
        sys.exit(1)

    if (
        not do_evol
        and not os.path.isdir(outputFolderName)
        and a.inputFolderName is None
    ):
        print("Setting doEvol to True since the output folder will be created.")
        do_evol = 1

    if do_evol:
        str1 = "all the contents"
    else:
        str1 = "the simulation results"

    if not hf.make_directory(a.outputFolderName, a.overwrite, str1):
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
        if hasattr(a, "addPrefix"):
            prefix = getattr(a, "addPrefix") + "_"
        else:
            prefix = ""

        can_copy_previous_evolution_files = _can_copy_previous_evolution_files(
            a.inputFolderName, a.outputFolderName
        )
        previous_evolution_files = {
            "best.gen.dat",
            "phenotype.dat",
            "best.phen.dat",
            "best.pheno.dat",
            "genhistory.dat",
            "search.cpt",
        }

        files = [
            "fitness.dat",
            "simulation_pars.json",
            "seed.dat",
            "worm_data.json",
            "best.gen.dat",
            "phenotype.dat",
            "best.pheno.dat",
            "worm_data_evo.json",
            "worm_data_worm.json",
            "genhistory.dat",
        ]
        if a.doCPT:
            files.append("search.cpt")

        if a.doNML:
            files += [
                "cell_Ids.json",
                "Worm2D.net.nml",
                "LEMS_Worm2D.xml",
                "LEMS_Worm2D_nrn.py",
                "cell_syn_W2D.xml",
                "cell_syn_W2D_cells.xml",
                "cell_W2Dosc.xml",
                "cell_W2Dosc_cells.xml",
                "syn_W2D.xml",
                "musc_W2D.xml",
                "musc_W2D_cells.xml",
                ".mod",
            ]

        for file in files:
            input_filenames = glob.glob(a.inputFolderName + "/*" + file)
            # print(input_filenames)
            for file1 in input_filenames:
                filename1 = pathlib.Path(file1).name
                if (
                    not can_copy_previous_evolution_files
                    and filename1 in previous_evolution_files
                ):
                    continue
                input_path = a.inputFolderName + "/" + filename1
                output_path = a.outputFolderName + "/" + prefix + filename1
                if os.path.isfile(input_path):
                    if os.path.isfile(output_path):
                        continue
                    shutil.copyfile(input_path, output_path)
        if hasattr(a, "modifyJson"):
            if getattr(a, "modifyJson"):
                json_path_mod = a.outputFolderName + "/" + prefix + "worm_data_evo.json"
                network_json_data_mod = utils.getJsonFile(json_path_mod)
                ns_mod = utils.getNervousSystem(network_json_data_mod)
                taus = utils.getNSvalue(network_json_data_mod, "taus")
                rtaus = utils.getNSvalue(network_json_data_mod, "Rtaus")
                for i in range(len(taus)):
                    taus[i] = 1
                    if rtaus is not None:
                        rtaus[i] = 1
                if "nervous_system" in network_json_data_mod:
                    for cell_name in utils.getCellNamesFull(network_json_data_mod):
                        ns_mod["cells"][cell_name]["tau"]["value"] = 1
                else:
                    if rtaus is not None:
                        ns_mod["Rtaus"]["value"] = rtaus
                    ns_mod["taus"]["value"] = taus
                with open(json_path_mod, "w", encoding="utf-8") as f:
                    json.dump(network_json_data_mod, f, ensure_ascii=False, indent=4)

    check_nan_files = ["fitness.dat", "genhistory.dat"]
    for file in check_nan_files:
        input_filenames = glob.glob(a.outputFolderName + "/*" + file)
        for file1 in input_filenames:
            filename1 = pathlib.Path(file1).name
            hf.clean_ragged_numeric_file(file1, a.outputFolderName + "/" + filename1)

            # np.loadtxt(file1)
            # arrs1 = hf.load_nonragged_arrays(file1)
            # data = arrs1[len(arrs1)-1] #use only last array
            # data = np.genfromtxt(file1, dtype=float)
            # data = np.nan_to_num(data, nan=0.0)
            # np.savetxt(a.outputFolderName + "/" + filename1, data, fmt="%.6g")

    sim_par_file = a.outputFolderName + "/simulation_pars.json"
    if os.path.isfile(sim_par_file):
        with open(sim_par_file) as f:
            sim_data = json.load(f)
    else:
        sim_data = {}

    if a.reRand:
        if "seed" in sim_data:
            del sim_data["seed"]

    random.seed(datetime.now().timestamp())
    random_seed = random.randint(1, 1000000)

    do_randInit = None
    if a.doRandInit is not None:
        if a.doRandInit:
            do_randInit = 1
        else:
            do_randInit = 0

    model_names = {
        ".": "CE",
        "RoyalSociety2018": "RS18",
        "network2021": "Net21",
        "CE_orientation": "CO",
        # "Worm2D/CO18": "CO18",
    }

    doW2D = True
    model_name = None
    model_folder = a.modelFolder

    model_folder_list = [
        "Worm2D",
        "../Worm2D",
        "Worm2D/CO18",
        "W2Dmoddev/src",
        "../W2Dmoddev/src",
    ]

    if a.modelName is None:
        worm_data = utils.getJsonFile(a.outputFolderName + "/worm_data_evo.json")
        if worm_data is not None:
            model_name = utils.getModelName(worm_data)
            if model_name == "CE":
                model_name = "W2DCE"
            model_folder = "Worm2D"
        if model_name is None:
            if a.modelFolder in model_names:
                model_name = model_names[a.modelFolder]
                doW2D = False
                model_folder = a.modelFolder
            if a.modelFolder in model_folder_list:
                print(
                    "'modelName' parameter is required if `Worm2D' or subfolder is the model folder.\n"
                )
                sys.exit(1)
        print("'modelName' parameter is not set, so will use the json file value.\n")
    else:
        model_name = a.modelName

    json_config_files = ["worm_data_worm.json", "worm_data_evo.json", "worm_data.json"]
    if model_name == "W2DSR" and not any(
        os.path.isfile(os.path.join(a.outputFolderName, filename))
        for filename in json_config_files
    ):
        searched = ", ".join(
            os.path.join(a.outputFolderName, filename) for filename in json_config_files
        )
        print(
            "The W2DSR model requires a JSON configuration file, but none was found.\n"
            f"Searched for: {searched}\n"
            "Provide an inputFolderName containing worm_data_worm.json, worm_data_evo.json, "
            "or worm_data.json, or write one of these files to outputFolderName before "
            "calling run()."
        )
        sys.exit(1)

    if a.modelFolder in model_names:
        if model_name is None:
            model_name = model_names[a.modelFolder]
        doW2D = False
        # model_folder = a.modelFolder

    model_name_list = [
        "W2Dosc",
        "W2DoscH",
        "W2Dosc21",
        "W2Dosc21all",
        "W2Dosc21Coup",
        "W2Dosc21CF",
        "W2Dosc21S",
        "W2D21",
        "W2DCE",
        "W2DCESR",
        "W2D21R",
        "W2DSR",
        "W2D18",
        "W2DCO",
    ]

    mainProcessName = a.mainProcessName
    if model_name in model_name_list:
        mainProcessName = "main_osc"

    defaults_bases = {
        "CE": defaults_base_celoc,
        "RS18": defaults_base_2018,
        "Net21": defaults_base_2021,
        "CO": defaults_base_CO,
        "CO18": defaults_base_CO18,
        "CO18Full": defaults_base_CO18,
        "W2Dosc": defaults_base_2021,
        "W2DoscH": defaults_base_2021,
        "W2Dosc21": defaults_base_2021,
        "W2Dosc21all": defaults_base_2021,
        "W2Dosc21Coup": defaults_base_2021,
        "W2Dosc21CF": defaults_base_2021,
        "W2Dosc21S": defaults_base_2021,
        "W2D21": defaults_base_2021,
        "W2DCE": defaults_base_celoc,
        "W2DCESR": defaults_base_celoc,
        "W2D21R": defaults_base_2021,
        "W2DSR": defaults_base_celoc,
        "W2D18": defaults_base_2018,
        "W2DCO": defaults_base_CO,
    }

    defaults_base = defaults_bases[model_name]
    # plot_format = model_name

    evol_extra_parameters = {}
    evol_extra_parameters["network_size"] = 6
    evol_extra_parameters["do_reverse"] = 0
    evol_extra_parameters["doAlternateEvo"] = 0
    evol_extra_parameters["sr_type"] = "None"
    # evol_extra_parameters["ab_level"] = 1
    evol_extra_parameters["ab_output_level"] = 1
    # evol_extra_parameters["random_initial_state"] = False
    evol_extra_parameters["random_initial_state"] = False
    evol_extra_parameters["MutVar"] = 0.1
    evol_extra_parameters["CrossProb"] = 0.5
    evol_extra_parameters["avg_speed"] = 0.00022
    evol_extra_parameters["fit_type"] = 0
    evol_extra_parameters["sr_form"] = 0
    evol_extra_parameters["sr_evo_bot"] = 0
    evol_extra_parameters["sr_evo_top"] = 200
    evol_extra_parameters["sr_evo_bot_a"] = 0
    evol_extra_parameters["sr_evo_top_a"] = 200
    evol_extra_parameters["sr_offset"] = 0
    evol_extra_parameters["sr_seg_per_sr"] = 6
    evol_extra_parameters["do_orig_musc_input"] = True
    evol_extra_parameters["do_orig_sr_input"] = True
    evol_extra_parameters["do_angle_diff"] = False
    evol_extra_parameters["StepSize"] = 0.005

    evol_extra_parameters["reset_agent_body"] = False
    evol_extra_parameters["useSupCPT"] = False
    evol_extra_parameters["modPar"] = True

    sim_extra_parameters = {}
    sim_extra_parameters["rotation"] = 0
    sim_extra_parameters["orient"] = 0
    sim_extra_parameters["do_test_run"] = False
    sim_extra_parameters["doForwardFirst"] = True
    sim_extra_parameters["sr_zero_gains_type"] = 0
    sim_extra_parameters["useGenJson"] = True
    sim_extra_parameters["SimStepSize"] = 0.005
    sim_extra_parameters["SimSkipSteps"] = 10
    sim_extra_parameters["do_legacy"] = True
    sim_extra_parameters["prioritizeCmd"] = 0
    sim_extra_parameters["init_ns_from_json"] = True
    sim_extra_parameters["input_ind"] = -1
    sim_extra_parameters["debug"] = False
    run_extra_parameters = {}
    run_extra_parameters["showPlot"] = False

    main_cmd = _resolve_binary(model_folder, mainProcessName)
    cmd = [main_cmd]

    evol_pars = [
        "Duration",
        "PopulationSize",
        "randomseed",
        "MaxGenerations",
        "Transient",
        "CheckpointInterval",
        "evo_type",
    ]

    evol_args = [
        a.duration,
        a.popSize,
        a.RandSeed,
        a.maxGens,
        a.transient,
        a.checkPointInterval,
        a.evo_type,
    ]

    evol_defaults = [
        defaults_base["duration"],
        defaults_base["popSize"],
        random_seed,
        defaults_base["maxGens"],
        defaults_base["transient"],
        0,
        defaults_base["evo_type"],
    ]

    a_replacements = {
        "randInitState": "random_initial_state",
        "randomInitialState": "random_initial_state",
        "ABLevel": "ab_output_level",
        "AB_output_level": "ab_output_level",
        "doReverse": "do_reverse",
        "SRType": "sr_type",
        "AvgSpeed": "avg_speed",
        "fitType": "fit_type",
        "SRForm": "sr_form",
        "SREvoBot": "sr_evo_bot",
        "SREvoTop": "sr_evo_top",
        "SREvoBotA": "sr_evo_bot_a",
        "SREvoTopA": "sr_evo_top_a",
        "SROffset": "sr_offset",
        "SRSegPerSR": "sr_seg_per_sr",
        "doOrigMuscInput": "do_orig_musc_input",
        "doOrigSRInput": "do_orig_sr_input",
        "doAngleDiff": "do_angle_diff",
        "resetAgentBody": "reset_agent_body",
        "doTestRun": "do_test_run",
        "SRZeroGainsType": "sr_zero_gains_type",
        "doLegacy": "do_legacy",
        "initNSFromJson": "init_ns_from_json",
        "inputInd": "input_ind",
        "evoType": "evo_type",
    }
    legacy_parameter_names = {
        new_key: old_key for old_key, new_key in a_replacements.items()
    }
    for key, val in a_replacements.items():
        if hasattr(a, key):
            if key in a._provided_args or not hasattr(a, val):
                setattr(a, val, getattr(a, key))
            if key in a._provided_args:
                a._provided_args.remove(key)
                a._provided_args.add(val)
            delattr(a, key)

    for new_key, old_key in legacy_parameter_names.items():
        if old_key in sim_data:
            if new_key not in sim_data:
                sim_data[new_key] = sim_data[old_key]
            del sim_data[old_key]

    for parameter_key in evol_extra_parameters:
        if hasattr(a, parameter_key):
            evol_pars.append(parameter_key)
            evol_args.append(getattr(a, parameter_key))
            evol_defaults.append(evol_extra_parameters[parameter_key])
            cmd += ["--" + parameter_key, str(TFtoInt(getattr(a, parameter_key)))]
            # cmd += ["--" + parameter_key, str(getattr(a, parameter_key))]

    for key, val in run_extra_parameters.items():
        if hasattr(a, key):
            newval = getattr(a, key)
            run_extra_parameters[key] = newval

    evol_data = {}
    evol_par_file_base = a.outputFolderName + "/evolution_pars.json"
    evol_par_file = a.outputFolderName + "/worm_data.json"
    if not os.path.isfile(evol_par_file):
        evol_par_file = a.outputFolderName + "/worm_data_evo.json"
    if os.path.isfile(evol_par_file):
        with open(evol_par_file) as f:
            worm_data = json.load(f)
            for key in evol_pars:
                if key in worm_data["Evolutionary Optimization Parameters"]:
                    evol_data[key] = worm_data["Evolutionary Optimization Parameters"][
                        key
                    ]["value"]
                elif (
                    key == "evo_type"
                    and "evoType" in worm_data["Evolutionary Optimization Parameters"]
                ):
                    evol_data[key] = worm_data["Evolutionary Optimization Parameters"][
                        "evoType"
                    ]["value"]
                elif (
                    key == "evo_type"
                    and "EvolutionType"
                    in worm_data["Evolutionary Optimization Parameters"]
                ):
                    evol_data[key] = worm_data["Evolutionary Optimization Parameters"][
                        "EvolutionType"
                    ]["value"]
                elif (
                    key in legacy_parameter_names
                    and legacy_parameter_names[key]
                    in worm_data["Evolutionary Optimization Parameters"]
                ):
                    evol_data[key] = worm_data["Evolutionary Optimization Parameters"][
                        legacy_parameter_names[key]
                    ]["value"]
                else:
                    print(f"Parameter {key} not found in worm_data.json")
    elif os.path.isfile(evol_par_file_base):
        with open(evol_par_file_base) as f:
            evol_data = json.load(f)

    for new_key, old_key in legacy_parameter_names.items():
        if old_key in evol_data:
            if new_key not in evol_data:
                evol_data[new_key] = evol_data[old_key]
            del evol_data[old_key]

    for old_key in ["evoType", "EvolutionType"]:
        if old_key in evol_data:
            if "evo_type" not in evol_data:
                evol_data["evo_type"] = evol_data[old_key]
            del evol_data[old_key]

    if a.reRand and do_evol and ("randomseed" in evol_data):
        del evol_data["randomseed"]

    same_vals = True
    # if do_evol:
    for par, arg, default in zip(evol_pars, evol_args, evol_defaults):
        if not setDict(evol_data, par, arg, default):
            same_vals = False
    if do_evol and same_vals:
        print(
            "Evolution not needed as evolution parameters are the same as the existing ones."
        )
        do_evol = 0

    do_nml = None
    if a.doNML is not None:
        if a.doNML:
            do_nml = 1
        else:
            do_nml = 0

    do_muscsim = None
    if a.doMuscSim is not None:
        if a.doMuscSim:
            do_muscsim = 1
            do_nml = 1
        else:
            do_muscsim = 0

    sim_pars = ["doNML", "seed", "Duration", "doRandInit", "Transient", "doMuscSim"]
    sim_args = [
        do_nml,
        a.RandSeed,
        a.simduration,
        do_randInit,
        a.simtransient,
        do_muscsim,
    ]
    sim_defaults = [
        defaults_base["doNML"],
        random_seed,
        defaults_base["simduration"],
        defaults_base["doRandInit"],
        defaults_base["simtransient"],
        defaults_base["doMuscSim"],
    ]

    for parameter_key in sim_extra_parameters:
        if hasattr(a, parameter_key):
            sim_pars.append(parameter_key)
            sim_args.append(getattr(a, parameter_key))
            sim_defaults.append(sim_extra_parameters[parameter_key])
            cmd += ["--" + parameter_key, str(TFtoInt(getattr(a, parameter_key)))]
            # cmd += ["--" + parameter_key, str(getattr(a, parameter_key))]

    doPlotEvol = True
    if hasattr(a, "doPlotEvol"):
        doPlotEvol = getattr(a, "doPlotEvol")

    same_vals = True
    for par, arg, default in zip(sim_pars, sim_args, sim_defaults):
        if not setDict(sim_data, par, arg, default):
            same_vals = False

    if not do_evol and same_vals:
        if a.overwrite:
            print(
                "Simulation parameters are the same as the existing ones, "
                "but overwrite is true so the simulation will be rerun."
            )
        else:
            print(
                "Simulation not needed as simulation parameters are the same as the existing ones.\n"
                "Please supply new command line arguments. Setting reRand to true will generate "
                "a new simulation seed and rerun the simulation. If rand_initial_state is false, "
                "the nervous-system initial states are still taken from the stored JSON values."
            )
            sys.exit(1)

    with open(sim_par_file, "w", encoding="utf-8") as f:
        json.dump(sim_data, f, ensure_ascii=False, indent=4)

    with open(evol_par_file_base, "w", encoding="utf-8") as f:
        json.dump(evol_data, f, ensure_ascii=False, indent=4)

    # cmd = ["./main",]

    # main_cmd = "../main"
    # main_cmd = "/home/adam/uclwork/CE_locomotion/experiments/.main"

    if mainProcessName == "main_osc":
        provided = a._provided_args

        if "crandSeed" in provided and a.crandSeed is not None:
            cmd += ["-r", str(a.crandSeed)]
        elif "RandSeed" in provided or ("reRand" in provided and a.reRand):
            if do_evol:
                cmd += ["-R", str(evol_data["randomseed"])]
            else:
                cmd += ["-R", str(sim_data["seed"])]

        if "popSize" in provided:
            cmd += ["--population_size", str(evol_data["PopulationSize"])]
        if "duration" in provided:
            cmd += ["--duration", str(evol_data["Duration"])]
        if "transient" in provided:
            cmd += ["--transient", str(evol_data["Transient"])]
        if "maxGens" in provided:
            cmd += ["--max_generations", str(evol_data["MaxGenerations"])]
        if "simduration" in provided:
            cmd += ["-sd", str(sim_data["Duration"])]
        if "simtransient" in provided:
            cmd += ["-st", str(sim_data["Transient"])]
        if "doEvol" in provided:
            cmd += ["--doevol", str(do_evol)]
        if "checkPointInterval" in provided:
            cmd += ["--checkpoint_interval", str(evol_data["CheckpointInterval"])]
        if "doRandInit" in provided:
            cmd += ["--dorandinit", str(sim_data["doRandInit"])]
        if "doNML" in provided:
            cmd += ["--donml", str(sim_data["doNML"])]

        cmd += ["--folder", str(a.outputFolderName)]
        if "modelName" in provided:
            cmd += ["--modelname", str(model_name)]

        if "doMuscSim" in provided:
            cmd += ["--domusc", str(sim_data["doMuscSim"])]
        if "doCPT" in provided:
            cmd += ["-docpt", str(TFtoInt(a.doCPT))]
        if "evo_type" in provided:
            cmd += ["--evo_type", str(a.evo_type)]
    else:
        if a.crandSeed is not None:
            cmd += ["-r", str(a.crandSeed)]
        else:
            if do_evol:
                cmd += ["-R", str(evol_data["randomseed"])]
            else:
                cmd += ["-R", str(sim_data["seed"])]

        # cmd += ["-sr", str(sim_data["seed"])]
        cmd += ["-p", str(evol_data["PopulationSize"])]
        cmd += ["-d", str(evol_data["Duration"])]
        cmd += ["-t", str(evol_data["Transient"])]
        cmd += ["--maxgens", str(evol_data["MaxGenerations"])]
        cmd += ["-sd", str(sim_data["Duration"])]
        cmd += ["-st", str(sim_data["Transient"])]
        cmd += ["--doevol", str(do_evol)]
        cmd += ["-cpt", str(evol_data["CheckpointInterval"])]

        cmd += ["--dorandinit", str(sim_data["doRandInit"])]
        cmd += ["--donml", str(sim_data["doNML"])]
        cmd += ["--folder", str(a.outputFolderName)]
        cmd += ["--modelname", str(model_name)]
        cmd += ["--domusc", str(sim_data["doMuscSim"])]
        cmd += ["-docpt", str(TFtoInt(a.doCPT))]
        cmd += ["--evo_type", str(evol_data["evo_type"])]

    print(
        "\n  Running Worm2D " + hf.get_worm2d_version() + " with the following command:"
    )

    print(cmd)
    # sys.exit(1)
    # env = os.environ.copy()
    # env["XKB_CONFIG_ROOT"] = "/usr/share/X11/xkb"

    # subprocess.run(["./main_osc"], env=env)
    # Run the C++
    if True:
        # result = subprocess.Popen(cmd, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        # result = subprocess.run(cmd, capture_output=True, text=True, cwd = home_dir)
        env = os.environ.copy()
        env_bin = os.path.dirname(sys.executable)
        env["PATH"] = env_bin + os.pathsep + env.get("PATH", "")
        env.setdefault("NEURON_MODULE_OPTIONS", "-nogui")
        result = subprocess.run(cmd, capture_output=True, text=True, env=env)
        if result.stdout:
            print(result.stdout)

        if result.stderr:
            print("Error:")
            print(result.stderr)

        if result.returncode != 0:
            sys.exit(result.returncode)

    # hf.dir_name = a.outputFolderName

    """     if a.modelFolder == ".":
        module_name = "load_data"
    else:
        module_name = a.modelFolder + ".load_data"
    rsr = import_module(module_name).reload_single_run
    rsr(show_plot=False, plot_format = plot_format) """

    if model_folder != "CE_orientation":
        from .load_data import reload_single_run

        # reload_single_run(show_plot=False, plot_format=plot_format)
        reload_single_run(
            showPlot=False, folderName=a.outputFolderName, modelName=model_name
        )

        if doW2D and doPlotEvol:
            from .load_data import plot_evols

            plot_evols(a, folderName=a.outputFolderName, modelName=model_name)

    if model_name == "CO18" or model_name == "CO18Full":
        reload_single_run(
            showPlot=False, folderName=a.outputFolderName, modelName="RS18"
        )

    if do_nml:
        if a.inputFolderName is not None and a.inputFolderName != a.outputFolderName:
            files_sub = [".xml", ".nml", ".mod"]
            files_pre = ["Worm2DNet", "LEMS", "cell_Ids.json"]

        input_filenames = []
        for file in files_sub:
            input_filenames += glob.glob(a.inputFolderName + "/*" + file)
        for file in files_pre:
            input_filenames += glob.glob(a.inputFolderName + "/" + file + "*")

            # print(input_filenames)
        for file1 in input_filenames:
            filename1 = pathlib.Path(file1).name
            input_path = a.inputFolderName + "/" + filename1
            if os.path.isfile(input_path):
                shutil.move(input_path, a.outputFolderName + "/" + filename1)

    print("Finished!")


if __name__ == "__main__":
    run_main()
