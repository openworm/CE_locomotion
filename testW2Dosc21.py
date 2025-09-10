from run_main import run

run(
    simduration=10,
    simtransient=10,
    duration=10,
    transient=10,
    maxGens=20,
    popSize=96,
    RandSeed=33981,
    modelName="W2DCE",
    modelFolder="Worm2D",
    #inputFolderName="exampleRunW2DCE_reverse",
    outputFolderName="exampleRunW2DCE_test",
    # outputFolderName="exampleRunW2Dosc_t1_nml",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    doCPT=True,
    evoType="EvoCE",
    doReverse = 2,
    SRType = "None"
    #SRType = "SR_TRANS_CONTRACT",
    #SRType = "SR_TRANS_STRETCH",  
    #SRType = "SR_TRANS_ABS", 
    #SRType = "SR_TRANS_NEG"
)
