from run_main import run

run(
    simduration=10,
    simtransient=10,
    duration=100,
    transient=30,
    maxGens=100,
    popSize=22,
    RandSeed=4012128,
    modelName="W2D21",
    modelFolder="Worm2D",
    # inputFolderName="exampleRunW2Dosc_t1",
    outputFolderName="exampleRunW2Dosc",
    # outputFolderName="exampleRunW2Dosc_t1_nml",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    evoType="EvoCE",
    doReverse=2,
)
