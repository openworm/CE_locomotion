from run_main import run
from neuromlLocal.regenerate import run as regenerate_run
import os
import sys

output_folder = "testruns/exW2DCEa"
output_folder_nml = "testruns/exW2DCEa_nml"

run(
    simduration=30,
    simtransient=0,
    popSize=96,
    RandSeed=1233,
    modelName="W2DCE",
    modelFolder="Worm2D",
    outputFolderName=output_folder,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    doOrigMuscInput=True,
    evo_type="EvoCE",
    doTestRun=False,
    SRZeroGainsType=1,
    doReverse=0,
    fitType=0,
    AvgSpeed=0.0001,
    inputInd=2,
    debug=False,
)

try:
    os.chdir("./neuromlLocal")
except Exception:
    print("Can't change to neuromlLocal.")
    print(sys.exc_info())

regenerate_run(folder="../" + output_folder, doMuscles=False)
os.chdir("../")

run(
    simduration=30,
    simtransient=0,
    popSize=96,
    RandSeed=1233,
    modelName="W2DCE",
    modelFolder="Worm2D",
    outputFolderName=output_folder_nml,
    inputFolderName=output_folder,
    doEvol=False,
    overwrite=True,
    doNML=True,
    checkPointInterval=5,
    doOrigMuscInput=True,
    evo_type="EvoCE",
    doTestRun=False,
    SRZeroGainsType=1,
    doReverse=0,
    fitType=0,
    AvgSpeed=0.0001,
    inputInd=2,
    debug=False,
)
