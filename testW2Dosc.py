from run_main import run

run(
    simduration=60,
    simtransient=50,
    duration=20,
    transient=10,
    maxGens=50,
    popSize=96,
    RandSeed=4918,
    # modelName="W2D21R",
    #modelName="W2DCE",
    modelName="W2DCESR",
    modelFolder="Worm2D",
    # inputFolderName="experiments/izq_runs_nets/23",
    # inputFolderName="exampleRunW2DCE_FR",
    outputFolderName="exampleRunW2Dosc",
    # outputFolderName="exampleRunW2Dosc_t1_nml",
    doEvol=False,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    evoType="EvoCE",
    # evoType="Evo21R",
    # evoType="Evo21",
    doReverse=2,
    doForwardFirst=True,
    randInitState=True,
    doTestRun=False,
    doCPT=True,
    AvgSpeed=0.0001,  # CE
    # AvgSpeed=0.00022,  # 21
    SRType="None",
    # SRType = "SR_TRANS_CONTRACT",
    # SRType = "SR_TRANS_STRETCH",
    # SRType = "SR_TRANS_ABS",
    # SRType = "SR_TRANS_NEG"
    ABLevel=1,
    # MutVar = 0.1,
    # CrossProb = 0.5
    fitType=1,
    SRForm=0,
    SREvoBot=0.0,
    SRSegPerSR = 6,
    SROffset = -4
)
