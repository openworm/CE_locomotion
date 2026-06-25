import os
import sys
from run_main import run
from .neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal")


doOrig = True
doMuscles = True
do_nml = True

duration = 10
transient = 10
randseed = 4012128

args = dict(
    simduration=duration,
    simtransient=transient,
    duration=40,
    transient=10,
    maxGens=10,
    popSize=22,
    RandSeed=randseed,
    modelName="W2Dosc21all",
    modelFolder="Worm2D",
    outputFolderName="experiments/exW2Dosc",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    doOrigMuscInput=False,
)

if doOrig:
    run(**args)

if do_nml:
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
    args["doEvol"] = False
    args["reRand"] = False
    run(**args)
