import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal")


doOrig = True
doNML = False
doMuscles = False

outputFolderName = "testruns/exW2DCEE"

args = dict(
    simduration=20,
    simtransient=0,
    maxGens=15,
    popSize =10,
    modelName="W2DCE",
    modelFolder="Worm2D",
    inputFolderName="testruns/exW2DCEs",
    outputFolderName=outputFolderName,
    reRand=False,
    doTestRun=True,
    doOrigMuscInput=False,
    doOrigSRInput=False,
    overwrite=True,
    doCPT=True,
    checkPointInterval=5,
    evoType="EvoCE",
    doEvol=True,
    doLegacy = False,
    randomInitialState = True,
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
