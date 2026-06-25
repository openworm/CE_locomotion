from build_network import run as build_network_run
from create_new_lems_file import run as create_new_lems_run

# import sys
import os
from pathlib import Path

path_list = []
out_path_list = []
# outFolderBases = ["varyEvolSeeds", "varyEvolSeeds1", "varyEvolSeeds2", "varyEvolSeeds3"]
inputFolderBase = "izq_runs_nets"
current = os.path.dirname(os.path.realpath(__file__))  # location of this file!


dir = Path(__file__).parents[1]
path = os.path.join(dir, "experiments/" + inputFolderBase)
# path = current + "../experiments/" + inputFolderBase
out_path = path
dir_list = sorted(os.listdir(path))
path_list += [
    path + "/" + dir for dir in dir_list if os.path.isdir(os.path.join(path, dir))
]
out_path_list += [
    out_path + "/" + dir for dir in dir_list if os.path.isdir(os.path.join(path, dir))
]

population_structures = [
    "one population",
    "individual populations",
    "cell specific populations",
]

population_structure = population_structures[2]
# json_file = "../exampleRunCEW2D/worm_data.json"
# json_file = "../exampleRun21W2D/worm_data.json"
# json_file = "../exampleRunRS18W2D/worm_data.json"
# json_file = "../exampleRunRS18/worm_data.json"
for input_folder, output_folder in zip(path_list, out_path_list):
    json_file = input_folder + "/worm_data.json"

    build_network_run(
        population_structure=population_structure,
        json_file=json_file,
        output_folder=output_folder,
    )
    create_new_lems_run(
        population_structure=population_structure,
        json_file=json_file,
        output_folder=output_folder,
    )
