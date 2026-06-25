from worm2d import run
from worm2d.helper_funcs import get_worm_json, write_worm_json, delete_directory

outputFolderName = "../testruns/exW2DCEanm2"
outputFolderName_2 = "../testruns/exW2DCEanm2_1"
origFolderName = "../testruns/exW2DCEanm"


# x = 0.1
delete_directory(outputFolderName_2)
delete_directory(outputFolderName)
json_data = get_worm_json(origFolderName)
# json_data['evolvable_ranges']['evotag_2']["active"] = False
write_worm_json(outputFolderName, json_data)

args = dict(
    modelFolder="Worm2D",  # folder containing the compiled C++ binary code
    modelName="W2DSR",  # rerun using the generic json loaded model
    maxGens=5,  # number of generations to run
    popSize=10,
    doEvol=True,
    # inputFolderName=origFolderName,  # input folder
    # outputFolderName=outputFolderName,  # output folder
    overwrite=True,  # allow overwrite of output folder
    # randInitState=True, #use fixed initial conditions from JSON file
    # doTestRun=True, #run a single simple simulation
    reRand=True,
    RandSeed=900351,
)

args["outputFolderName"] = outputFolderName
run(**args)


json_data = get_worm_json(origFolderName)
json_data["evolvable_ranges"]["ns_cells_bias_0"]["active"] = False
json_data["evolvable_ranges"]["ns_chemcons_1"]["active"] = False
json_data["evolvable_ranges"]["DA_0_1_tau"] = {
    "active": True,
    "lower_limit": 0.1,
    "upper_limit": 2.5,
    "name": "DA_tau",
}
json_data["nervous_system"]["cells"]["DA_0"]["tau"]["evotag"] = "DA_0_1_tau"
json_data["nervous_system"]["cells"]["DA_1"]["tau"]["evotag"] = "DA_0_1_tau"
write_worm_json(outputFolderName_2, json_data)
args["outputFolderName"] = outputFolderName_2
run(**args)  #
