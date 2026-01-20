from run_main import run

run(
    simduration=50,
    simtransient=0,
    maxGens=20,
    popSize=66,
    RandSeed=929921,
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
