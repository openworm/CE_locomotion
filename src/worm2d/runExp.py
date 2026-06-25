import os
import sys
from run_main import run
from .neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal")

doOrig = True
doNML = True
doMuscles = False

args = dict(
    simduration=50,
    simtransient=30,
    inputFolderName="experiments/demoRun_1",
    outputFolderName="experiments/demoRun_1_out",
    reRand=True,
    overwrite=True,
    randInitState=False,
    prioritizeCmd=True,
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
