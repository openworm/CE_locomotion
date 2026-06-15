import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run
# sys.path.append("./neuromlLocal")
import helper_funcs as hf

doOrig = True
doNML = False
doMuscles = False

#indir = 'COW2DSRE_test_3'
indir = 'exW2DCEFR'
#indir = 'COW2DSRE_test'
outdir = indir + "switch"
pfolder = 'testruns'
#pfolder = 'notebooks'

inputFolderName = pfolder + "/" + indir
outputFolderName = pfolder + "/" + outdir

doInputFolder = True
renewJson = True

args = dict(
    simduration=20,
    simtransient=10,
    duration=10,
    transient=10,
    maxGens=15,
    popSize=10,
    RandSeed=987,
    modelName="W2DSR",
    modelFolder="Worm2D",
    outputFolderName=outputFolderName,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    doCPT=True,
    evo_type="EvoCE",
    doReverse=2,
    SRType="None",
    ABLevel=3,
    doForwardFirst=False,
    # SRType="SR_TRANS_CONTRACT",
    # SRType = "SR_TRANS_STRETCH",
    # SRType = "SR_TRANS_ABS",
    # SRType = "SR_TRANS_NEG"
    randInitState=True,
    # MutVar = 0.5
    doOrigMuscInput=False,
    doOrigSRInput=False,
    doLegacy=False,
    SRZeroGainsType=1,
    # doLegacy = True
    debug=True,
    doTestRun=True,
)

if doInputFolder is True:
    args["inputFolderName"] = inputFolderName
    hf.delete_subfolder_directory(pfolder, outdir)

    if renewJson is True:
        json_data = hf.get_worm_json(inputFolderName)
        json_data = hf.set_input_switcher_schedule(
            json_data,
            time_periods=[20, 10],
            input_indices=[1, 0],
            time_offset=0,
        )
        json_data = hf.set_funcable_schedule(
    json_data,
    function_index=2,
    time_intervals=[20, 10],
    condvals=[1, 0],
    time_offset=0,
    schedule_name="alternating_gains",
)
        hf.write_worm_json(outputFolderName, json_data)
   





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
