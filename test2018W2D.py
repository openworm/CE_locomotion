from run_main import run
from helper_funcs import delete_directory

outputFolderName = "exampleRunRS18W2D"
delete_directory(outputFolderName)

run(
    maxGens=36,
    popSize=36,
    RandSeed=292,
    modelName="RS18",
    modelFolder="Worm2D",
    outputFolderName=outputFolderName,
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    doOrigMuscInput=True,
    doOrigSRInput=True,
)
