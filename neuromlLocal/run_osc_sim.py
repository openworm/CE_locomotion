import sys
import os

sys.path.append("..")
# sys.path.append("../neuromlLocal")

from run_main import run
from run_main import make_directory
from regenerate import run as regenerate_run


output_folder= "../experiments/osc_sim"
output_folder_nml= "../experiments/osc_sim_nml"

duration = 40
transient = 10

doMuscles = False

print(output_folder)
run(
    simduration=10,
    simtransient=10,
    duration=duration,
    transient=transient,
    maxGens=20,
    popSize=22,
    RandSeed=4012128,
    modelName="W2Dosc",
    modelFolder="../Worm2D",
    outputFolderName=output_folder,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=False,
    doNML=False,
)
print(output_folder_nml)
regenerate_run(folder=output_folder, doMuscles=doMuscles)
run(
    simduration=10,
    simtransient=10,
    duration=duration,
    transient=transient,
    maxGens=20,
    popSize=22,
    RandSeed=4012128,
    modelName="W2Dosc",
    modelFolder="../Worm2D",
    outputFolderName=output_folder_nml,
    inputFolderName=output_folder,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=False,
    doNML=True,
)

