from run_main import run

run(
    simduration=50,
    simtransient=0,
    maxGens=36,
    popSize=36,
    RandSeed=292,
    modelName="W2D18",
    modelFolder="Worm2D",
    outputFolderName="testruns/W2D18/run",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    evo_type="Evo18",
    doTestRun=True,
    doOrigMuscInput=True,
    doOrigSRInput=True,
    randInitState=False,
)
