from build_network import run as build_network_run
from create_new_lems_file import run as create_new_lems_run

population_structures = [
    "one population",
    "individual populations",
    "cell specific populations",
]
population_structure = population_structures[2]
json_file = "../exampleRun/worm_data.json"
build_network_run(population_structure=population_structure, json_file=json_file)
create_new_lems_run(population_structure=population_structure, json_file=json_file)
