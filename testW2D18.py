from run_main import run

run(
    simduration=50,
    simtransient=0,
    maxGens=36,
    popSize=36,
    RandSeed=292,
    modelName="W2D18",
    modelFolder="Worm2D",
    outputFolderName="testruns/exW2D18",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    evoType="Evo18",
    doTestRun=True,
    doOrigMuscInput=True,
    doOrigSRInput=True,
    randInitState=False,
)
