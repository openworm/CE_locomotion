import os
import sys
from run_main import run
from .neuromlLocal.regenerate import run as regenerate_run

# sys.path.append("..")
# sys.path.append("../neuromlLocal")


output_folder = "experiments/osc21alldemo_p2"
output_folder_nml = output_folder + "_nml"
output_folder_nml_musc = output_folder + "_nml_musc"

duration = 10
transient = 10

do_muscles = False
do_nml = False

randseed = 40112
model_name = "W2Dosc21all"
print(output_folder)
run(
    simduration=duration,
    simtransient=transient,
    duration=30,
    transient=0,
    maxGens=30,
    popSize=92,
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
    useGenJson=True,
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
    )
