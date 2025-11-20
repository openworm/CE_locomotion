from run_main import run

run(
    simduration=30,
    simtransient=0,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName="testruns/exW2D18gen",
    outputFolderName="testruns/exW2DSR18",
    reRand=True,
    doTestRun=True,
    doOrigMuscInput=False,
)
