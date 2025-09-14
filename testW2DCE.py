from run_main import run

run(
    simduration=20,
    simtransient=0,
    popSize=96,
    RandSeed=1233,
    modelName="W2DCE",
    modelFolder="Worm2D",
    # inputFolderName="exampleRunW2Dosc_t1",
    outputFolderName="exampleRunW2DCE",
    # outputFolderName="exampleRunW2Dosc_t1_nml",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    doCPT=True,
    evoType="EvoCE",
    # SRType = "SR_TRANS_STRETCH"
    # doTestRun = True
)
