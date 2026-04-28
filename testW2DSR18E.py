import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal") add


doOrig = True
doNML = False
doMuscles = False

outputFolderName = "testruns/exW2DSR18E"

args = dict(
    simduration=100,
    simtransient=0,
    maxGens=10,
    popSize=1,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName="testruns/exW2D18gen",
    outputFolderName=outputFolderName,
    doEvol=True,
    reRand=False,
    doTestRun=True,
    doOrigMuscInput=False,
    doOrigSRInput=False,
    evoType="Evo18",
    overwrite=True,
    doCPT=True,
    checkPointInterval=5,
    randomInitialState=False,
    doLegacy=False,
    debug = True
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
