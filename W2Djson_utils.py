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
        if "value" not in j1[NSname][parval]:
            j1[NSname][parval]["value"] = []
        j1[NSname][parval]["value"].append(parameters[parval]["value"])
        if "evolvable" in parameters[parval]:
            if "evolvable" not in j1[NSname][parval]:
                j1[NSname][parval]["evolvable"] = []
            j1[NSname][parval]["evolvable"].append(
                {"ind":indVal, "val": parameters[parval]["evolvable"]})
    return indVal

def addConnection(j1, wp):
    if wp["module"] not in jsonNames["Weights"]:
        print(wp["module"], " not in ", jsonNames["Weights"])
        exit()
    if wp["type"] not in jsonNames["Weights"][wp["module"]]:
        print(wp["type"], " not in ", jsonNames["Weights"][wp["module"]])
    if wp["type"] not in j1[wp["module"]]:
        j1[wp["module"]][wp["type"]] = {}
        j1[wp["module"]][wp["type"]]["value"] = []
    j1[wp["module"]][wp["type"]]["value"].append(wp["value"])    
    if "evolvable" in wp:
        if "evolvable" not in j1[wp["module"]][wp["type"]]:
            j1[wp["module"]][wp["type"]]["evolvable"] = []
        j1[wp["module"]][wp["type"]]["evolvable"].append(
            {"from" : wp["value"]["from"], 
             "to" : wp["value"]["to"],
            "val" : wp["evolvable"]
             }
            ) 
    


def run(a=None, **kwargs):
    a = hf.build_namespace(hf.DEFAULTS, a, **kwargs)

    print(a.folderName)
    worm_file = a.folderName #+ "/worm_data.json"
    network_json_data = utils.getJsonFile(worm_file)
    
    hf.make_directory("test_json_utils", overwrite=True)
    addedNeurons = []
    

    cell_parameters = {"biases" : {"value" : -100, "evolvable" : 3},
                            "taus" : {"value": -100}, 
                            "gains" : {"value" : -100, "evolvable" : 7}, 
                            "states" : {"value":-100}
                            }
    addedNeurons.append({"index" : addNewNeuron(network_json_data, cell_parameters), 
                         "parameters" : cell_parameters})
    
    weight_parameters = {"module": NSname,
                         "type": "Chemical weights",
                         "value" :
                        {"from" : addedNeurons[-1]["index"], 
                        "to" : 15, 
                        "weight" : -1}, 
                        "evolvable" : 2}                                   
    addConnection(network_json_data, weight_parameters)
    


    #print(addedNeurons)                
    #unity indices

    with open("test_json_utils/test.json", "w", encoding="utf-8") as json_file:
        json.dump(network_json_data, json_file, indent=4, ensure_ascii=False)

if __name__ == "__main__":
    filename = "W2Dmoddev/testruns/testCO18Full/RS18_worm_data.json"
    run(folderName=filename)

