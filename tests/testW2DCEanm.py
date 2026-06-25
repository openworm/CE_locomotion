from worm2d import run

run(
    simduration=30,
    simtransient=0,
    popSize=96,
    RandSeed=1233,
    modelName="W2DCE",
    modelFolder="Worm2D",
    outputFolderName="../testruns/exW2DCEanm",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    doOrigMuscInput=False,
    evo_type="EvoCE",
    doTestRun=False,
    SRZeroGainsType=1,
    doReverse=0,
    fitType=0,
    AvgSpeed=0.0001,
    inputInd=2,
    debug=False,
)
