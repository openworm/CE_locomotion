import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal")


doOrig = True
doNML = False
doMuscles = False

outputFolderName = "testruns/exW2DSRFR"

args = dict(
    simduration=10,
    simtransient=10,
    duration=10,
    transient=10,
    maxGens=30,
    popSize=10,
    RandSeed= 987,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName="testruns/exW2DCEFR",
    outputFolderName=outputFolderName,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    doCPT=True,
    evoType="EvoCE",
    doReverse=2,
    SRType="None",
    ABLevel=3,
    doForwardFirst=False,
    # SRType="SR_TRANS_CONTRACT",
    # SRType = "SR_TRANS_STRETCH",
    # SRType = "SR_TRANS_ABS",
    # SRType = "SR_TRANS_NEG"
    randInitState= True,
    # MutVar = 0.5
    doOrigMuscInput=False,
    doOrigSRInput=False,
    doLegacy = False
    #doLegacy = True
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
