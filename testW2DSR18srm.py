import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run

# sys.path.append("./neuromlLocal")
import json
import helper_funcs as hf


doOrig = True
doNML = False
doMuscles = False

outputFolderName = "testruns/exW2DSR18srm"
inputFolderName = "testruns/exW2D18gen"

hf.make_directory(outputFolderName, True, "Output folder for W2DSR18srm")

with open(inputFolderName + "/worm_data_worm.json") as json_file:
    json_data = json.load(json_file)

json_data["stretch_receptor"]["set_direct"]["value"] = True

with open(outputFolderName + "/worm_data_worm.json", "w") as f:
    json.dump(json_data, f)


args = dict(
    simduration=50,
    simtransient=0,
    modelName="W2DSR",
    modelFolder="Worm2D",
    outputFolderName=outputFolderName,
    reRand=True,
    doTestRun=True,
    doOrigMuscInput=False,
    doOrigSRInput=True,
    modifyJson=False,
    debug=True,
    overwrite=True,
)

if doOrig:
    run(**args)

if doNML:
    try:
        os.chdir("./neuromlLocal")
    except Exception:
        print("Can't change to neuromlLocal.")
        print(sys.exc_info())

    regenerate_run(folder="../" + outputFolderName, doMuscles=False)
    os.chdir("../")

    args["inputFolderName"] = outputFolderName
    args["outputFolderName"] = outputFolderName + "_nml"
    args["doNML"] = True
    args["reRand"] = False
    run(**args)

if doMuscles:
    try:
        os.chdir("./neuromlLocal")
    except Exception:
        print("Can't change to neuromlLocal.")
        print(sys.exc_info())

    regenerate_run(folder="../" + outputFolderName, doMuscles=True)
    os.chdir("../")

    args["inputFolderName"] = outputFolderName
    args["outputFolderName"] = outputFolderName + "_nml_musc"
    args["doNML"] = True
    args["reRand"] = False
    args["doMuscSim"] = True
    run(**args)


""" run(
    simduration=30,
    simtransient=0,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName="testruns/exW2D18gen",
    outputFolderName="testruns/exW2DSR18",
    reRand=True,
    doTestRun=True,
    doOrigMuscInput=False,
)
 """
