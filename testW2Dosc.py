from run_main import run

run(
    simduration= 60,
    simtransient= 50,
    duration= 40,
    transient= 10,
    maxGens= 200,
    popSize= 62,
    RandSeed= 40088,
    modelName= "W2D21",
    #modelName="W2DCE",
    modelFolder= "Worm2D",
    inputFolderName= "experiments/izq_runs_nets/5",
    outputFolderName= "exampleRunW2Dosc",
    # outputFolderName="exampleRunW2Dosc_t1_nml",
    doEvol = True,
    overwrite = True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=False,
    doNML=False,
    #evoType= "EvoCE",
    evoType="Evo21R",
    doReverse=2,
    doForwardFirst = True,
    randInitState = True,
    SRType= "None",
    doTestRun = False,
    doCPT = True,
    #AvgSpeed = 0.0001, #CE
    AvgSpeed = 0.00022, #21
    # SRType = "SR_TRANS_CONTRACT",
    # SRType = "SR_TRANS_STRETCH",
    # SRType = "SR_TRANS_ABS",
    # SRType = "SR_TRANS_NEG"
)
