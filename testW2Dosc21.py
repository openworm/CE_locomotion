from run_main import run

run(
    simduration=10,
    simtransient=10,
    duration=50,
    transient=10,
    maxGens=200,
    popSize=22,
    RandSeed=4012128,
    modelName="W2Dosc21",
    modelFolder="Worm2D",
    # inputFolderName="exampleRunW2Dosc_t1",
    outputFolderName="exampleRunW2Dosc_t1",
    # outputFolderName="exampleRunW2Dosc_t1_nml",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=False,
    doNML=False,
)
