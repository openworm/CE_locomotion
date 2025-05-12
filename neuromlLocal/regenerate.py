import sys

# import os
from build_network import run as build_network_run
from create_new_lems_file import run as create_new_lems_run


def run(output_folder):
    # current = os.path.dirname(os.path.realpath(__file__))
    json_file = output_folder + "/worm_data.json"

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


if __name__ == "__main__":
    folder_name = sys.argv[1]
    output_folder = folder_name
    run(output_folder=output_folder)
