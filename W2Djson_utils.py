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

NSname = "Nervous system"

jsonNames = {"List" : {NSname : ["biases", "taus", "gains", "states"]},
            "Weights" : {NSname : ["Chemical weights", "Electrical weights"]}}  
                                 


def addNewNeuron(j1, parameters):
    j1[NSname]["size"]["value"] = j1[NSname]["size"]["value"] + 1
    indVal = j1[NSname]["size"]["value"]
    for parval in jsonNames["List"][NSname]:
        if parameters[NSname][parval] is dict:
            j1[NSname][parval]["value"].append(parameters[NSname][parval]["value"])
            j1[NSname][parval]["evolvable"].append(
                {"ind":indVal, "val": parameters[NSname][parval]["evolvable"]})
        else:
            j1[NSname][parval]["value"].append(parameters[NSname][parval])
    return indVal

def run(a=None, **kwargs):
    a = hf.build_namespace(hf.DEFAULTS, a, **kwargs)

    print(a.folderName)
    worm_file = a.folderName + "/worm_data.json"
    network_json_data = utils.getJsonFile(worm_file)
    
    hf.make_directory("test_json_utils", overwrite=True)
    addedNeurons = []

    parameters = {NSname : {"biases" : {"value" : -100, "evolvable" : 3},
                            "taus" : -100, 
                            "gains" : {"value" : -100, "evolvable" : 7}, 
                            "states" : -100}}
    addedNeurons.append({"index" : addNewNeuron(network_json_data, parameters), 
                         "parameters" : parameters})
    
    #print(addedNeurons)                
    #unity indices

    with open("test_json_utils/test.json", "w", encoding="utf-8") as json_file:
        json.dump(network_json_data, json_file, indent=4, ensure_ascii=False)

if __name__ == "__main__":

    run(folderName="exampleRunRS18W2D_keep3")

