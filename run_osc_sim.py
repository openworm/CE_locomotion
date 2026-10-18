import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run

# sys.path.append("..")
# sys.path.append("../neuromlLocal")


output_folder = "testruns/osc_sim/run"
output_folder_nml = "testruns/osc_sim/nml"
output_folder_nml_musc = "testruns/osc_sim/nml_musc"

duration = 10
transient = 10

do_muscles = True
do_nml = True

randseed = 4077

print(output_folder)
run(
    simduration=duration,
    simtransient=transient,
    duration=50,
    transient=0,
    maxGens=10,
    popSize=62,
    RandSeed=randseed,
    modelName="W2Dosc",
    modelFolder="Worm2D",
    outputFolderName=output_folder,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=False,
    doPlotEvol=True,
    doNML=False,
    doOrigMuscInput=False,
    doOrigSRInput=True,
    doTestRun=True,
)


if do_nml:
    try:
        os.chdir("./neuromlLocal")
    except Exception:
        print("Can't change to neuromlLocal.")
        print(sys.exc_info())

    regenerate_run(folder="../" + output_folder, doMuscles=False)
    os.chdir("../")

    run(
        simduration=duration,
        simtransient=transient,
        RandSeed=randseed,
        modelName="W2Dosc",
        modelFolder="Worm2D",
        outputFolderName=output_folder_nml,
        inputFolderName=output_folder,
        doEvol=False,
        overwrite=True,
        reRand=False,
        doPlotEvol=False,
        doNML=True,
        doOrigMuscInput=False,
        doOrigSRInput=True,
        doMuscSim=False,
        doTestRun=True,
    )


if do_muscles:
    try:
        os.chdir("./neuromlLocal")
    except Exception:
        print("Can't change to neuromlLocal.")
        print(sys.exc_info())

    regenerate_run(folder="../" + output_folder, doMuscles=True)
    os.chdir("../")

    run(
        simduration=duration,
        simtransient=transient,
        RandSeed=randseed,
        modelName="W2Dosc",
        modelFolder="Worm2D",
        outputFolderName=output_folder_nml_musc,
        inputFolderName=output_folder,
        doEvol=False,
        overwrite=True,
        reRand=False,
        doPlotEvol=False,
        doNML=True,
        doOrigMuscInput=False,
        doOrigSRInput=True,
        doMuscSim=True,
        doTestRun=True,
    )
