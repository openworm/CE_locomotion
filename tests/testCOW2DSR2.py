import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run

Pi = 3.1415926

indir = "testruns/COW2DSRE_test"

doOrig = True
doNML = False
doMuscles = False

inputFolderName = indir
outputFolderName = indir + "_out"
doInputFolder = True

args = dict(
    duration=50,
    transient=10,
    simduration=100,
    simtransient=0,
    maxGens=4,
    popSize=16,
    RandSeed=187857,
    modelName="W2DSR",
    modelFolder="Worm2D",
    outputFolderName=outputFolderName,
    reRand=True,
    doTestRun=True,
    modifyJson=False,
    doEvol=True,
    resetAgentBody=True,
    orient=Pi / 4,
    rotation=Pi / 4,
    doOrigSRInput=False,
    doOrigMuscInput=False,
    checkPointInterval=5,
    evo_type="EvoCO2",
    overwrite=True,
)

if doInputFolder is True:
    args["inputFolderName"] = inputFolderName

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
