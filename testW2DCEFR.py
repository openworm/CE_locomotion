from run_main import run

run(
    simduration=10,
    simtransient=10,
    duration=10,
    transient=10,
    maxGens=15,
    popSize=36,
    RandSeed=879712,
    modelName="W2DCE",
    modelFolder="Worm2D",
    # inputFolderName="exampleRunW2DCE_reverse",
    outputFolderName="testruns/exW2DCEFR",
    # outputFolderName="exampleRunW2Dosc_t1_nml",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    doCPT=True,
    evoType="EvoCE",
    doReverse=2,
    SRType="None",
    ABLevel=3,
    doForwardFirst=False,
    # SRType="SR_TRANS_CONTRACT",
    # SRType = "SR_TRANS_STRETCH",
    # SRType = "SR_TRANS_ABS",
    # SRType = "SR_TRANS_NEG"
    randInitState = True,
    doOrigMuscInput = False,
    doOrigSRInput = False,
    doLegacy = False
    # MutVar = 0.5
)
