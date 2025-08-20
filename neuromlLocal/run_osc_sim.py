import sys
import os

sys.path.append("..")
# sys.path.append("../neuromlLocal")

from run_main import run
from run_main import make_directory
from regenerate import run as regenerate_run


output_folder= "../experiments/osc_sim"
output_folder_nml= "../experiments/osc_sim_nml"

duration = 10
transient = 10

doMuscles = False

print(output_folder)
run(
    simduration=duration,
    simtransient=transient,
    duration=50,
    transient=20,
    maxGens=100,
    popSize=22,
    RandSeed=4128,
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
    RandSeed=4128,
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

