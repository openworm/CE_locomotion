from run_main import run

run(
    simduration=10,
    simtransient=10,
    duration=20,
    transient=10,
    maxGens=40,
    popSize=32,
    RandSeed=48611,
    modelName="W2Dosc21all",
    modelFolder="Worm2D",
    # inputFolderName="exampleRunW2Dosc_t1",
    outputFolderName="exampleRunW2Dosc21_t1",
    # outputFolderName="exampleRunW2Dosc_t1_nml",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    doCPT = True
)
