import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal")


doOrig = True
doNML = True
doMuscles = False

args = dict(
    simduration=20,
    simtransient=0,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName="exampleRunW2DCE",
    outputFolderName="experiments/exW2DSR_2",
    reRand=True,
    doTestRun=True,
    doOrigMuscInput=False,
)

if doOrig:
    run(**args)

if doNML:
    try:
        os.chdir("./neuromlLocal")
    except Exception:
        print("Can't change to neuromlLocal.")
        print(sys.exc_info())

    regenerate_run(folder="../" + args["outputFolderName"], doMuscles=doMuscles)
    os.chdir("../")

    args["inputFolderName"] = args["outputFolderName"]
    args["outputFolderName"] = args["outputFolderName"] + "_nml"
    args["doNML"] = True
    args["reRand"] = False
    run(**args)
