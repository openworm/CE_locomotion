from run_main import run

run(
    simduration=20,
    simtransient=0,
    modelName="W2DSR",
    modelFolder="Worm2D",
    inputFolderName="exampleRunW2DCE",
    outputFolderName="experiments/exW2DSR",
    reRand=True,
    doTestRun=True,
)
