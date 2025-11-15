import sys
import os
import json
from collections import defaultdict
import matplotlib.pyplot as plt
import seaborn as sns
# import helper_funcs as hf

sys.path.append("..")

from run_main import make_directory


wormPhenoPars = {}

wormPhenoPars["Net21"] = [
    "NMJ_AS",
    "NMJ_DA",
    "NMJ_DB",
    "NMJ_DD",
    "NMJ_VD",
    "NMJ_VB",
    "NMJ_VANMJ_Gain_Map",
]

wormPhenoPars["CE"] = ["NMJ_DA", "NMJ_DB", "NMJ_VD", "NMJ_VB", "NMJ_VA", "NMJ_DD"]


def getCellIndices(cell_names, cell_name):
    return [i for i, val in enumerate(cell_names) if val == cell_name]


def getCellNames(network_json_data):
    return network_json_data["Nervous system"]["Cell name"]["value"]


def getCellNameRep(cell_names, cell_ind):
    cell_name = cell_names[cell_ind]
    indslist = getCellIndices(cell_names, cell_name)
    return cell_name, indslist.index(cell_ind)


def getNervousSystemVal(network_json_data, val):
    return network_json_data["Nervous system"][val]["value"]


""" def getPopNamesUnsorted(network_json_data, pop_num=6):
    cell_names = getCellNames(network_json_data)
    return cell_names[:pop_num] """


def check_equal(list):
    return all(i == list[0] for i in list)


def getWeight(weights, pre_cell_ind, post_cell_ind):
    return [
        weight["weight"]
        for weight in weights
        if ((weight["from"] == pre_cell_ind) & (weight["to"] == post_cell_ind))
    ][0]


def getParsDict(vals, wormPhenoPars):
    new_vals = {}
    for key, val in vals.items():
        if key in wormPhenoPars:
            new_vals[key] = val["value"]
    return new_vals


def getValsDict(vals, cell_names):
    new_vals = {}
    for ind, val in enumerate(vals):
        kk = cell_names[ind]
        if kk in new_vals:
            if new_vals[kk] != val:
                print("Vals not equal!")
                sys.exit(1)
        else:
            new_vals[kk] = val
    return new_vals


def getWeightsDict(weights, cell_names):
    new_weights = {}
    # kk_inds = {}
    for weight in weights:
        name_from, ind_from = getCellNameRep(cell_names, weight["from"] - 1)
        name_to, ind_to = getCellNameRep(cell_names, weight["to"] - 1)
        kk = name_from + name_to + str(ind_to - ind_from)
        if kk in new_weights:
            if new_weights[kk] != weight["weight"]:
                print("Weights not equal!")
                sys.exit(1)
        else:
            new_weights[kk] = weight["weight"]
    return new_weights


def getUniques(list1):
    result1 = defaultdict(list)
    for d in list1:
        for key, value in d.items():
            result1[key].append(value)
    return result1


model_name = "Net21"
path_list = []
# outFolderBases = ["varyEvolSeeds", "varyEvolSeeds1", "varyEvolSeeds2", "varyEvolSeeds3"]
outFolderBases = ["varyEvolSeedsNet21_4"]
# outFolderBases = ["izq_runs_nets"]
current = os.path.dirname(os.path.realpath(__file__))  # location of this file!
for outFolderBase in outFolderBases:
    path = current + "/" + outFolderBase
    dir_list = sorted(os.listdir(path))
    path_list += [path + "/" + dir for dir in dir_list]


# path_list = hf.get_path_list(outFolderBases)

print(len(path_list))
# sys.exit(1)


fitness_keys = ["Best", "Average", "Variance"]
weights_list = []
elec_weights_list = []
fitness_list = []
biases_list = []
taus_list = []
worm_vals_list = []
do_fitness = True
for dir in path_list:
    json_file = dir + "/worm_data.json"
    if not os.path.isfile(json_file):
        break
    if not os.path.isfile(dir + "/fitness.dat"):
        break
    with open(json_file, "r") as file:
        worm_data = json.load(file)
    cell_names = getCellNames(worm_data)
    chem_weights = getNervousSystemVal(worm_data, "Chemical weights")
    weights_list.append(getWeightsDict(chem_weights, cell_names))
    elec_weights = getNervousSystemVal(worm_data, "Electrical weights")
    elec_weights_list.append(getWeightsDict(elec_weights, cell_names))
    biases = getNervousSystemVal(worm_data, "biases")
    biases_list.append(getValsDict(biases, cell_names))
    taus = getNervousSystemVal(worm_data, "taus")
    taus_list.append(getValsDict(taus, cell_names))
    worm_vals = getParsDict(worm_data["Worm"], wormPhenoPars[model_name])
    if model_name == "CE":
        worm_vals["SR_A_gain"] = worm_data["Stretch receptor"]["SR_A_gain"]["value"]
        worm_vals["SR_B_gain"] = worm_data["Stretch receptor"]["SR_B_gain"]["value"]

    worm_vals_list.append(worm_vals)
    ol1 = None
    with open(dir + "/fitness.dat", "r") as file:
        while True:
            l1 = file.readline()
            if l1:
                ol1 = l1
            else:
                break
            # print(l1)
        file.close()
    if ol1 is not None:
        d1 = {}
        for key, val in zip(fitness_keys, ol1.split()):
            d1[key] = float(val)
        fitness_list.append(d1)
    else:
        print("No fitness")
        do_fitness = False


""" print(fitness_list)
sys.exit(1)
print(weights_list)
sys.exit(1)
 """
# print(worm_vals_list)
# sys.exit(1)

# all_weights = getUniques(weights_list)
if do_fitness:
    all_fitnesses = getUniques(fitness_list)
    # print(all_weights)
    print(all_fitnesses)


# sys.exit(1)

out_dir_name = "results21"
if not make_directory(out_dir_name, overwrite=True):
    sys.exit(1)
fit_level = 0.9

results_titles = ["ChemWei", "ElectWei", "Bias", "Tau", "Worm"]
data_results_list = [
    weights_list,
    elec_weights_list,
    biases_list,
    taus_list,
    worm_vals_list,
]

for title, data_result in zip(results_titles, data_results_list):
    res1 = getUniques(data_result)
    for key, val in res1.items():
        sns.displot(val, bins=10, kde=True)
        title_str = title + "_" + key + ".png"
        plt.savefig(out_dir_name + "/" + title_str)
        plt.close()
        if do_fitness:
            best_fit = [
                val1
                for val1, fitness in zip(val, all_fitnesses["Best"])
                if fitness > fit_level
            ]
            sns.displot(best_fit, bins=10, kde=True)
            plt.savefig(out_dir_name + "/best_fit_" + title_str)
            plt.close()
