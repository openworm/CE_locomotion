import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run

# sys.path.append("./neuromlLocal")
from W2Djson_utils import mergeJsons

Pi = 3.1415926


file1 = "testruns/COW2DSREfiles/RS18_worm_data.json"
file2 = "testruns/COW2DSREfiles/CO18Full_worm_data_evo.json"

mergeJsons(file1, file2)

doOrig = True
doNML = False
doMuscles = False

inputFolderName = "testruns/COW2DSREgen"
outputFolderName = "testruns/COW2DSREgen_out"

args = dict(
    simduration=500,
    simtransient=0,
    RandSeed=173257,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName=inputFolderName,
    outputFolderName=outputFolderName,
    reRand=True,
    doTestRun=True,
    doOrigMuscInput=False,
    modifyJson=False,
    doEvol=False,
    resetAgentBody=True,
    orient=0,
    rotation=Pi,
    doOrigSRInput=False,
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
