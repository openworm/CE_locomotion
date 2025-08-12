from run_main import run

run(
    simduration=10,
    simtransient=10,
    duration=50,
    transient=50,
    maxGens=100,
    popSize=2,
    RandSeed=11792247,
    modelName="W2Dosc",
    modelFolder="Worm2D",
    outputFolderName="exampleRunW2Dosc",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=False,
)
