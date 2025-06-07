from run_main import run

run(
    maxGens=10,
    popSize=96,
    RandSeed=95166,
    modelName="CO",
    modelFolder="Worm2D",
    outputFolderName="exampleRunCOW2D",
    doEvol=True,
    overwrite=True,
)
