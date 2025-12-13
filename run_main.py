import subprocess
import argparse
import os
import sys
import neuromlLocal.utils as utils

# import helper_funcs as hf
# from importlib import import_module
import shutil
import glob
import pathlib

# from pyneuroml.utils.cli import build_namespace
import random
from datetime import datetime
import json


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
    "evoType": "Evo18",
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
    "evoType": "EvoCE",
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
    "evoType": "Evo18",
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
    "evoType": "Evo18",
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
    "evoType": "Evo21",
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
    "checkPointInterval": 0,
    "doCPT": True,
    "evoType": "Evo21",
    # "MutVar" : 0.1,
    # "CrossProb" : 0.5
}


def process_args():
    """Parse command-line arguments.

    :returns: None
    """
    parser = argparse.ArgumentParser(
        description=("A script for supplying arguments to execute Worm2D")
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
        "--evoType",
        type=str,
        metavar="<evoType>",
        default=DEFAULTS["evoType"],
        help=(
            "Name of evolution function.\nOptions include: Evo21, Evo18"
            # "Default is: %s" % DEFAULTS["modelName"]
        ),
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


def make_directory(directory_name, overwrite=False, str1="the contents"):
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


def TFtoInt(val):
    if val is True:
        return 1
    if val is False:
        return 0
    return val
    print("TFtoInt error")
    sys.exit(1)


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

    if (
        not do_evol
        and not os.path.isdir(a.outputFolderName)
        and a.inputFolderName is None
    ):
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
        if hasattr(a, "addPrefix"):
            prefix = getattr(a, "addPrefix") + "_"
        else:
            prefix = ""

        files = [
            "fitness.dat",
            "simulation_pars.json",
            "seed.dat",
            "worm_data.json",
            "best.gen.dat",
            "phenotype.dat",
            "best.pheno.dat",
            "search.cpt",
            "worm_data_evo.json",
            "genhistory.dat",
        ]

        for file in files:
            input_filenames = glob.glob(a.inputFolderName + "/*" + file)
            # print(input_filenames)
            for file1 in input_filenames:
                filename1 = pathlib.Path(file1).name
                input_path = a.inputFolderName + "/" + filename1
                if os.path.isfile(input_path):
                    shutil.copyfile(
                        input_path, a.outputFolderName + "/" + prefix + filename1
                    )
        if hasattr(a, "modifyJson"):
            if getattr(a, "modifyJson") == True:
                json_path_mod =  a.outputFolderName + "/" + prefix + "worm_data_evo.json"
                network_json_data_mod = utils.getJsonFile(json_path_mod)
                rtaus = network_json_data_mod["Nervous system"]["Rtaus"]["value"]
                taus = network_json_data_mod["Nervous system"]["taus"]["value"]
                for i in range(len(taus)):
                    taus[i]=1
                    rtaus[i]=1
                network_json_data_mod["Nervous system"]["Rtaus"]["value"] = rtaus
                network_json_data_mod["Nervous system"]["taus"]["value"] = taus
                with open(json_path_mod, "w", encoding="utf-8") as f:
                    json.dump(network_json_data_mod, f, ensure_ascii=False, indent=4)


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
    evol_extra_parameters["doReverse"] = 0
    evol_extra_parameters["doAlternateEvo"] = 0
    evol_extra_parameters["SRType"] = "None"
    evol_extra_parameters["ABLevel"] = 1
    evol_extra_parameters["randInitState"] = False
    evol_extra_parameters["MutVar"] = 0.1
    evol_extra_parameters["CrossProb"] = 0.5
    evol_extra_parameters["AvgSpeed"] = 0.00022
    evol_extra_parameters["fitType"] = 0
    evol_extra_parameters["SRForm"] = 0
    evol_extra_parameters["SREvoBot"] = 0
    evol_extra_parameters["SREvoTop"] = 200
    evol_extra_parameters["SREvoBotA"] = 0
    evol_extra_parameters["SREvoTopA"] = 200
    evol_extra_parameters["SROffset"] = 0
    evol_extra_parameters["SRSegPerSR"] = 6
    evol_extra_parameters["SRZeroGainsTypeEvo"] = 1
    evol_extra_parameters["doOrigMuscInput"] = True
    evol_extra_parameters["doOrigSRInput"] = True
    evol_extra_parameters["doAngleDiff"] = False
    evol_extra_parameters["StepSize"] = 0.005

    sim_extra_parameters = {}
    sim_extra_parameters["rotation"] = 0
    sim_extra_parameters["orient"] = 0
    sim_extra_parameters["doTestRun"] = False
    sim_extra_parameters["doForwardFirst"] = True
    sim_extra_parameters["SRZeroGainsType"] = 0
    sim_extra_parameters["useGenJson"] = True

    sim_extra_parameters["prioritizeCmd"] = 0

    main_cmd = model_folder + "/" + mainProcessName
    cmd = [main_cmd]

    evol_pars = [
        "Duration",
        "PopulationSize",
        "randomseed",
        "MaxGenerations",
        "Transient",
        "CheckpointInterval",
        "EvolutionType",
    ]

    evol_args = [
        a.duration,
        a.popSize,
        a.RandSeed,
        a.maxGens,
        a.transient,
        a.checkPointInterval,
        a.evoType,
    ]

    evol_defaults = [
        defaults_base["duration"],
        defaults_base["popSize"],
        random_seed,
        defaults_base["maxGens"],
        defaults_base["transient"],
        0,
        defaults_base["evoType"],
    ]

    for parameter_key in evol_extra_parameters:
        if hasattr(a, parameter_key):
            evol_pars.append(parameter_key)
            evol_args.append(getattr(a, parameter_key))
            evol_defaults.append(evol_extra_parameters[parameter_key])
            cmd += ["--" + parameter_key, str(TFtoInt(getattr(a, parameter_key)))]
            # cmd += ["--" + parameter_key, str(getattr(a, parameter_key))]

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
                else:
                    print("Parameter not found in worm_data.json")
    elif os.path.isfile(evol_par_file_base):
        with open(evol_par_file_base) as f:
            evol_data = json.load(f)

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
        print(
            "Simulation not needed as simulation parameters are the same as the existing ones.\n"
            "Please supply new command line arguments."
        )
        sys.exit(1)

    with open(sim_par_file, "w", encoding="utf-8") as f:
        json.dump(sim_data, f, ensure_ascii=False, indent=4)

    with open(evol_par_file_base, "w", encoding="utf-8") as f:
        json.dump(evol_data, f, ensure_ascii=False, indent=4)

    # cmd = ["./main",]

    # main_cmd = "../main"
    # main_cmd = "/home/adam/uclwork/CE_locomotion/experiments/.main"

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
    cmd += ["--evoType", str(a.evoType)]

    print(cmd)
    # sys.exit(1)

    # Run the C++
    if True:
        # result = subprocess.Popen(cmd, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        # result = subprocess.run(cmd, capture_output=True, text=True, cwd = home_dir)
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.stdout:
            print(result.stdout)

        if result.stderr:
            print("Error:")
            print(result.stderr)

    # hf.dir_name = a.outputFolderName

    """     if a.modelFolder == ".":
        module_name = "load_data"
    else:
        module_name = a.modelFolder + ".load_data"
    rsr = import_module(module_name).reload_single_run
    rsr(show_plot=False, plot_format = plot_format) """

    if model_folder != "CE_orientation":
        from load_data import reload_single_run

        # reload_single_run(show_plot=False, plot_format=plot_format)
        reload_single_run(
            showPlot=False, folderName=a.outputFolderName, modelName=model_name
        )

        if doW2D and doPlotEvol:
            from load_data import plot_evols

            plot_evols(folderName=a.outputFolderName, modelName=model_name)

    if model_name == "CO18" or model_name == "CO18Full":
        reload_single_run(
            showPlot=False, folderName=a.outputFolderName, modelName="RS18"
        )

    if do_nml:
        if a.inputFolderName is not None and a.inputFolderName != a.outputFolderName:
            files_sub = [".xml", ".nml"]
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


if __name__ == "__main__":
    run_main()
