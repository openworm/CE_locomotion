from run_main import run

run(
    duration = 50,
    transient = 50,
    maxGens=40,
    popSize=26,
    RandSeed=1749493257,
    modelName="CO",
    modelFolder="Worm2D",
    outputFolderName="exampleRunCOW2D",
    doEvol=True,
    overwrite=True,
)
