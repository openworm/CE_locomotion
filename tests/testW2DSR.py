import os
import sys
from worm2d import run
from worm2d.neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal")


doOrig = True
doNML = True
doMuscles = True

outputFolderName = "../testruns/exW2DSR"

args = dict(
    simduration=20,
    simtransient=0,
    RandSeed=655117,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName="exampleRunW2DCE",
    outputFolderName=outputFolderName,
    reRand=False,
    doTestRun=True,
    doOrigMuscInput=False,
    doOrigSRInput=False,
    # SRZeroGainsType=1,
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
