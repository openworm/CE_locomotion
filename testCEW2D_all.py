import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run


output_folder = "exampleRunCEW2D"
output_folder_nml = "exampleRunCEW2D_nml"

run(
    popSize=96,
    RandSeed=1233,
    modelName="CE",
    modelFolder="Worm2D",
    outputFolderName=output_folder,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    doOrigMuscInput=True,
)

try:
    os.chdir("./neuromlLocal")
except Exception:
    print("Can't change to neuromlLocal.")
    print(sys.exc_info())

regenerate_run(folder="../" + output_folder, doMuscles=False)
os.chdir("../")

run(
    popSize=96,
    RandSeed=1233,
    modelName="CE",
    modelFolder="Worm2D",
    outputFolderName=output_folder_nml,
    inputFolderName=output_folder,
    doEvol=False,
    overwrite=True,
    doNML=True,
)
