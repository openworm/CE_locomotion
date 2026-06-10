import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal")


doOrig = True
doNML = False
doMuscles = False

inputFolderName = "experiments/izq_runs_nets_W2D21/103"
outputFolderName = "testruns/exW2DSRE21"

args = dict(
    simduration=50,
    simtransient=0,
    duration=40,
    transient=10,
    maxGens=5,
    popSize=5,
    RandSeed=292,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName=inputFolderName,
    outputFolderName=outputFolderName,
    overwrite=True,
    reRand=False,
    doTestRun=True,
    doOrigMuscInput=False,
    modifyJson=False,
    doEvol=True,
    checkPointInterval=5,
    evo_type="Evo21",
    SimStepSize=0.01,
    SimSkipSteps=4,
    randomInitialState=False,
    doLegacy=False,
    debug=True,
    # initNSFromJson = False
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
