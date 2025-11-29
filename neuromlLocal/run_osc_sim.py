import sys


sys.path.append("..")
# sys.path.append("../neuromlLocal")

from run_main import run
from regenerate import run as regenerate_run


output_folder = "../experiments/osc_sim"
output_folder_nml = "../experiments/osc_sim_nml"

duration = 10
transient = 10

doMuscles = False

randseed = 4012128

print(output_folder)
run(
    simduration=duration,
    simtransient=transient,
    duration=50,
    transient=30,
    maxGens=10,
    popSize=22,
    RandSeed=4012128,
    modelName="W2Dosc",
    modelFolder="../Worm2D",
    outputFolderName=output_folder,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
)
print(output_folder_nml)
regenerate_run(folder=output_folder, doMuscles=doMuscles)
run(
    simduration=duration,
    simtransient=transient,
    RandSeed=randseed,
    modelName="W2Dosc",
    modelFolder="../Worm2D",
    outputFolderName=output_folder_nml,
    inputFolderName=output_folder,
    doEvol=False,
    overwrite=True,
    reRand=True,
    doPlotEvol=False,
    doNML=True,
)
