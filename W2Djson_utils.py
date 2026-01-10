import helper_funcs as hf
import os
import neuromlLocal.utils as utils
import json


def joinJson(json1, json2):
    j1_size = json1["Nervous system"]["size"]["value"]
    j2_size = json2["Nervous system"]["size"]["value"]
    pass


def incNSvals(j1):
    pass


jsonNames = {"List" : {"Nervous system" : ["biases", "taus", "gains", "states"]},
            "Weights" : {"Nervous system" : ["Chemical weights", "Electrical weights"]}}  
                                 


def addNewNeuron(j1, parameters, name = None):
    for key, val in j1["Nervous system"].items():
        val2 = j1["Nervous system"][key]["value"]
        if key in parameters:
           j1["Nervous system"][key]["value"].append(val)


def run(a=None, **kwargs):
    a = hf.build_namespace(hf.DEFAULTS, a, **kwargs)

    print(a.folderName)
    worm_file = a.folderName + "/worm_data.json"
    network_json_data = utils.getJsonFile(worm_file)
    
    hf.make_directory("test_json_utils", overwrite=True)

    with open("test_json_utils/test.json", "w", encoding="utf-8") as json_file:
        json.dump(network_json_data, json_file, indent=4, ensure_ascii=False)

if __name__ == "__main__":

    run(folderName="exampleRunRS18W2D_keep3")

