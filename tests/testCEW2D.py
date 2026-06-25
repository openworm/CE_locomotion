from run_main import run

run(
    popSize=96,
    RandSeed=1233,
    modelName="CE",
    modelFolder="Worm2D",
    outputFolderName="exampleRunCEW2D",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    doOrigMuscInput=True,
)
