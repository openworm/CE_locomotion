from run_main import run

run(
    simduration=30,
    simtransient=0,
    maxGens=36,
    popSize=26,
    RandSeed=292,
    modelName="W2D18",
    modelFolder="Worm2D",
    outputFolderName="testruns/exW2D18gen",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    evoType="Evo18",
    doTestRun=True,
    doOrigMuscInput=False,
    doOrigSRInput=False,
    randInitState=False,
)
