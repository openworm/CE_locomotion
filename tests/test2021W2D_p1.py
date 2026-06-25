from run_main import run

run(
    maxGens=200,
    popSize=100,
    RandSeed=951065,
    modelName="Net21",
    modelFolder="Worm2D",
    inputFolderName="exampleRun21W2D_p1",
    outputFolderName="exampleRun21W2D_p2",
    doEvol=False,
    overwrite=True,
)
