from run_main import run

run(
    simduration=20,
    simtransient=0,
    popSize=96,
    RandSeed=1233,
    maxGens=5,
    modelName="W2DCE",
    modelFolder="Worm2D",
    outputFolderName="testruns/exW2DCEs",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    doPlotEvol=True,
    doNML=False,
    # doCPT=True,
    evoType="EvoCE",
    AvgSpeed=0.0001,
    # SRType = "SR_TRANS_STRETCH"
    doTestRun=True,
    doOrigMuscInput=True,
    SRZeroGainsType=1,
)
