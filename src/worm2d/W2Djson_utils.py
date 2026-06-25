from . import helper_funcs as hf
from .neuromlLocal import utils
import json
import pathlib


def joinJson(json1, json2):
    j1_size = utils.getNervousSystemSize(json1)
    j2_size = utils.getNervousSystemSize(json2)
    return j1_size + j2_size


def incNSvals(j1):
    pass


NSname = "Nervous system"
EOP = "Evolutionary Optimization Parameters"


def normalize_evolvable_range_entries(evolvable_ranges):
    entries = []

    def evotag_number(evotag):
        if isinstance(evotag, int):
            return evotag
        if isinstance(evotag, str) and evotag.startswith("evotag_"):
            return int(evotag.replace("evotag_", "", 1))
        return evotag

    for entry in evolvable_ranges.get("value", []):
        if "evotag" in entry:
            entry = dict(entry)
            entry["evotag"] = evotag_number(entry["evotag"])
            entries.append(entry)
            continue

        if len(entry) != 1:
            continue

        evotag_key, attrs = next(iter(entry.items()))

        attrs = dict(attrs)
        attrs["evotag"] = evotag_number(evotag_key)
        entries.append(attrs)

    for evotag_key, attrs in evolvable_ranges.items():
        if evotag_key == "value" or not isinstance(attrs, dict):
            continue

        attrs = dict(attrs)
        attrs["evotag"] = evotag_number(evotag_key)
        entries.append(attrs)

    return entries


jsonNames = {
    # "List": {NSname: ["biases", "taus", "gains", "states", "externalinputs"]},
    "List": {
        NSname: ["biases", "taus", "gains", "states", "Cell name", "Section name"]
    },
    "Weights": {
        NSname: ["Chemical weights", "Electrical weights"],
        "Driving input": ["weights"],
        "OutputNS": ["weights"],
    },
}

jsonLists = {"Driving input": ["strengths"]}


def addNewNeuron(j1, parameters):
    j1[NSname]["size"]["value"] = j1[NSname]["size"]["value"] + 1
    indVal = j1[NSname]["size"]["value"]
    if "Cell name" not in parameters:
        parameters["Cell name"] = {"value": "Cell_" + str(indVal)}
    if "Section name" not in parameters:
        parameters["Section name"] = {"value": "interneuron"}
    for parval in jsonNames["List"][NSname]:
        if parval not in j1[NSname]:
            j1[NSname][parval] = {}
        if "value" not in j1[NSname][parval]:
            j1[NSname][parval]["value"] = []
        j1[NSname][parval]["value"].append(parameters[parval]["value"])
        if "evolvable" in parameters[parval]:
            if "evolvable" not in j1[NSname][parval]:
                j1[NSname][parval]["evolvable"] = []
            j1[NSname][parval]["evolvable"].append(
                {"ind": indVal, "val": parameters[parval]["evolvable"]}
            )
    return indVal


def addConnection(j1, wp):
    if wp["module"] not in jsonNames["Weights"]:
        print(wp["module"], " not in ", jsonNames["Weights"])
        exit()
    if wp["type"] not in jsonNames["Weights"][wp["module"]]:
        print(wp["type"], " not in ", jsonNames["Weights"][wp["module"]])
        exit()
    if wp["type"] not in j1[wp["module"]]:
        j1[wp["module"]][wp["type"]] = {}
        j1[wp["module"]][wp["type"]]["value"] = []
    j1[wp["module"]][wp["type"]]["value"].append(wp["value"])
    if "evolvable" in wp:
        if "evolvable" not in j1[wp["module"]][wp["type"]]:
            j1[wp["module"]][wp["type"]]["evolvable"] = []
        j1[wp["module"]][wp["type"]]["evolvable"].append(
            {
                "from": wp["value"]["from"],
                "to": wp["value"]["to"],
                "val": wp["evolvable"],
            }
        )


def makeWeightParameters(modulename, parname, val, evolind=None):
    weight_parameters_1 = {"module": modulename, "type": parname, "value": val}
    if evolind is not None:
        weight_parameters_1["evolvable"] = evolind
    return weight_parameters_1


def incToFromWeight(val, tval=0, fval=0):
    return {"from": val["from"] + fval, "to": val["to"] + tval, "weight": val["weight"]}


def run(a=None, **kwargs):
    a = hf.build_namespace(hf.DEFAULTS, a, **kwargs)


def mergeJsons(file1, file2, outdir):
    print(file1)
    print(file2)
    # worm_file = a.file1  # + "/worm_data.json"
    network_json_data = utils.getJsonFile(file1)

    addedNeurons = []
    appended_json_data = utils.getJsonFile(file2)
    network_worm = network_json_data.setdefault(
        "worm", network_json_data.pop("Worm", {})
    )
    appended_worm = appended_json_data.get("worm", appended_json_data.get("Worm", {}))
    # "W2Dmoddev/testruns/testCO18Full/CO18Full_worm_data_evo.json"

    appendedSize = utils.getNervousSystemSize(appended_json_data)
    origSize = utils.getNervousSystemSize(network_json_data)
    # appendedDrivingSize = len(appended_json_data["Driving input"]["strengths"]["value"])
    origDrivingSize = len(network_json_data["Driving input"]["strengths"]["value"])

    network_worm["N_size"]["value"] += appended_worm["N_size"]["value"]

    # network_json_data[NSname]["Model name"]["value"] = "COW2DSR"
    # json_model_name = network_json_data[NSname]["Model name"]["value"]

    section_names = utils.getNSvalue(network_json_data, "Section name")
    if section_names is None:
        json_model_name = utils.getModelName(network_json_data)
        section_names = utils.default_cells[json_model_name]["Section name"]
        if "Section name" not in network_json_data[NSname]:
            network_json_data[NSname]["Section name"] = {}
        network_json_data[NSname]["Section name"]["value"] = section_names

    if "main_model_name" not in network_worm:
        network_worm["main_model_name"] = {}
    network_worm["main_model_name"]["value"] = "COW2DSR"

    if "section sizes" in appended_json_data[NSname]:
        for key in appended_json_data[NSname]["section sizes"]:
            keyval = key
            if key in network_json_data[NSname]["section sizes"]:
                keyval = key + " 2"
            network_json_data[NSname]["section sizes"][keyval] = appended_json_data[
                NSname
            ]["section sizes"][key]

    # print(appendedDrivingSize, "sdd ", origDrivingSize)

    for modulename in jsonLists:
        for parname in jsonLists[modulename]:
            network_json_data[modulename][parname]["value"] += appended_json_data[
                modulename
            ][parname]["value"]

    for key, val in appended_worm.items():
        if key not in network_worm:
            network_worm[key] = val

    if EOP not in network_json_data:
        network_json_data[EOP] = {}

    if EOP in appended_json_data:
        for key, val in appended_json_data[EOP].items():
            if key not in network_json_data[EOP]:
                network_json_data[EOP][key] = val

    for modulename in jsonNames["Weights"]:
        addmodulename = modulename
        for parname in jsonNames["Weights"][modulename]:
            addparname = parname
            if parname in appended_json_data[modulename]:
                for val in appended_json_data[modulename][parname]["value"]:
                    if modulename == NSname:
                        val2 = incToFromWeight(val, tval=origSize, fval=origSize)
                    elif modulename == "Driving input":
                        val2 = incToFromWeight(val, tval=origSize, fval=origDrivingSize)
                    elif modulename == "OutputNS":
                        val2 = incToFromWeight(val, tval=0, fval=origSize)
                        addmodulename = NSname
                        addparname = "Chemical weights"
                    else:
                        val2 = val
                    addConnection(
                        network_json_data,
                        makeWeightParameters(addmodulename, addparname, val2),
                    )

    for ind in range(appendedSize):
        cell_parameters_1 = {}
        for parname in jsonNames["List"][NSname]:
            if parname in appended_json_data[NSname]:
                cell_parameters_1[parname] = {}
                cell_parameters_1[parname]["value"] = appended_json_data[NSname][
                    parname
                ]["value"][ind]
        addedNeurons.append(
            {
                "index": addNewNeuron(network_json_data, cell_parameters_1),
                "parameters": cell_parameters_1,
            }
        )

    joined_model_name = (
        utils.getModelName(network_json_data)
        + "_"
        + utils.getModelName(appended_json_data)
    )
    if "nervous_system" in network_json_data:
        network_json_data["nervous_system"].setdefault("model_name", {})["value"] = (
            joined_model_name
        )
    else:
        network_json_data[NSname].setdefault("Model name", {})["value"] = (
            joined_model_name
        )

    # hf.make_directory("test_json_utils", overwrite=True)
    network_json_data["Driving input"]["size"]["value"] += appended_json_data[
        "Driving input"
    ]["size"]["value"]

    print(addedNeurons)
    # unity indices
    hf.make_directory(outdir, True)

    with open(outdir + "/worm_data.json", "w", encoding="utf-8") as json_file:
        json.dump(network_json_data, json_file, indent=4, ensure_ascii=False)


def addEvolvable(network_json_data):
    if "evolvable_ranges" not in network_json_data:
        network_json_data["evolvable_ranges"] = network_json_data.get(
            "Evolvable", {"value": []}
        )
    network_json_data.pop("Evolvable", None)

    evolvables = normalize_evolvable_range_entries(
        network_json_data["evolvable_ranges"]
    )
    print(evolvables)


def addCells(network_json_data):
    cell_names = utils.getCellNames(network_json_data)
    section_names = utils.getNSvalue(network_json_data, "Section name")
    json_model_name = utils.getModelName(network_json_data)
    if cell_names is None:
        cell_names = utils.default_cells[json_model_name]["names"]
        network_json_data[NSname]["Cell name"]["value"] = cell_names
    if section_names is None:
        section_names = utils.default_cells[json_model_name]["Section name"]
        if "Section name" not in network_json_data[NSname]:
            network_json_data[NSname]["Section name"] = {}
        network_json_data[NSname]["Section name"]["value"] = section_names

    SMDD_cell_ind = utils.getIndOfNthVal("SMDD", cell_names, 0)
    SMDD_cell_ind

    addedNeurons = []
    cell_parameters = {
        "biases": {"value": -100, "evolvable": 3},
        "taus": {"value": -100},
        "gains": {"value": -100, "evolvable": 7},
        "states": {"value": -100},
    }
    addedNeurons.append(
        {
            "index": addNewNeuron(network_json_data, cell_parameters),
            "parameters": cell_parameters,
        }
    )

    weight_parameters = {
        "module": NSname,
        "type": "Chemical weights",
        "value": {"from": addedNeurons[-1]["index"], "to": SMDD_cell_ind, "weight": -1},
        "evolvable": 2,
    }
    addConnection(network_json_data, weight_parameters)

    print(addedNeurons)
    # unity indices
    filename1 = pathlib.Path(file1).name
    dirpath = str(pathlib.Path(file1).parent)

    with open(dirpath + "/" + "Sup_" + filename1, "w", encoding="utf-8") as json_file:
        json.dump(network_json_data, json_file, indent=4, ensure_ascii=False)


if __name__ == "__main__":
    # file1 = "W2Dmoddev/testruns/testCO18Full/RS18_worm_data.json"
    # file2 = "W2Dmoddev/testruns/testCO18Full/CO18Full_worm_data_worm.json"
    # file1 = "W2Dmoddev/experiments/jan14/run_0/RS18_worm_data.json"
    # file2 = "W2Dmoddev/experiments/jan14/run_0/CO18Full_worm_data_evo.json"
    # file1 = "W2Dmoddev/experiments/CO18Full_demo_k2_3/RS18_worm_data.json"
    # file2 = "W2Dmoddev/experiments/CO18Full_demo_k2_3/CO18Full_worm_data_evo.json"
    file1 = "/home/adam/uclwork/CE_locomotion/testruns/COW2DSREgen/worm_data.json"
    # run(folderName=filename)
    network_json_data = utils.getJsonFile(file1)
    addCells(network_json_data)
    # mergeJsons(file1=file1, file2=file2)
