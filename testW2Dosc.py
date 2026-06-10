from run_main import run

run(
    simduration=10,
    simtransient=10,
    duration=70,
    transient=10,
    maxGens=20,
    popSize=20,
    RandSeed=40212,
    modelName="W2Dosc21CF",
    modelFolder="Worm2D",
    # inputFolderName="exampleRunW2Dosc",
    outputFolderName="exampleRunW2Dosc",
    # outputFolderName="exampleRunW2Dosc_nml",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    # evo_type="Evo18",
    # evo_type="EvoCENZ",
    # evo_type="Evo21R",
    evo_type="Evo21",
    # randInitState=True,
    doTestRun=True,
    doCPT=True,
    doOrigMuscInput=False,
    fitType=1,
    doAngleDiff=True,
    # AvgSpeed=0.0001,  # CE
    # AvgSpeed=0.00022,  # 21
)
