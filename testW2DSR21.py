import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal")


doOrig = True
doNML = True
doMuscles = True

inputFolderName = "testruns/izq_sim_W2D21/run/103"
outputFolderName = "testruns/W2DSR21/run"

args = dict(
    simduration=60,
    simtransient=50,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName=inputFolderName,
    outputFolderName=outputFolderName,
    reRand=True,
    doTestRun=True,
    doOrigMuscInput=False,
    doOrigSRInput=True,
    modifyJson=False,
    debug=True,
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
    args["outputFolderName"] = "testruns/W2DSR21/nml"
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
    args["outputFolderName"] = "testruns/W2DSR21/nml_musc"
    args["doNML"] = True
    args["reRand"] = False
    args["doMuscSim"] = True
    run(**args)
