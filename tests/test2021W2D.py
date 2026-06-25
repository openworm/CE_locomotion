from run_main import run

run(
    maxGens=30,
    popSize=100,
    RandSeed=95166,
    modelName="Net21",
    modelFolder="Worm2D",
    outputFolderName="exampleRun21W2D",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
)
