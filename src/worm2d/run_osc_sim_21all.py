import os
import sys
from run_main import run
from .neuromlLocal.regenerate import run as regenerate_run

# sys.path.append("..")
# sys.path.append("../neuromlLocal")


output_folder = "experiments/osc_sim_21all"
output_folder_nml = "experiments/osc_sim_21all_nml"
output_folder_nml_musc = "experiments/osc_sim_21all_nml_musc"

duration = 10
transient = 10

do_muscles = True
do_nml = True

randseed = 4012128
model_name = "W2Dosc21all"
print(output_folder)
run(
    simduration=duration,
    simtransient=transient,
    duration=50,
    transient=30,
    maxGens=10,
    popSize=22,
    RandSeed=randseed,
    modelName=model_name,
    modelFolder="Worm2D",
    outputFolderName=output_folder,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    doOrigMuscInput=False,
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
        modelName=model_name,
        modelFolder="Worm2D",
        outputFolderName=output_folder_nml,
        inputFolderName=output_folder,
        doEvol=False,
        overwrite=True,
        reRand=True,
        doPlotEvol=False,
        doNML=True,
        doOrigMuscInput=False,
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
        modelName=model_name,
        modelFolder="Worm2D",
        outputFolderName=output_folder_nml_musc,
        inputFolderName=output_folder,
        doEvol=False,
        overwrite=True,
        reRand=True,
        doPlotEvol=False,
        doNML=True,
        doOrigMuscInput=False,
        doMuscSim=True,
        doTestRun=True,
    )
