from run_main import run

run(
    maxGens=100,
    popSize=16,
    RandSeed=8813751,
    modelName="RS18",
    modelFolder="Worm2D",
    inputFolderName="exampleRunRS18W2D",
    outputFolderName="exampleRunRS18W2D_p1",
    doEvol=False,
    overwrite=True,
)
