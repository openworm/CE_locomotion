from run_main import run

run(
    simduration=50,
    simtransient=0,
    maxGens=20,
    popSize=66,
    RandSeed=929921,
    modelName="W2D18",
    modelFolder="Worm2D",
    outputFolderName="testruns/W2D18gen/run",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    evo_type="Evo18",
    doTestRun=True,
    doOrigMuscInput=False,
    doOrigSRInput=False,
    randInitState=False,
)
