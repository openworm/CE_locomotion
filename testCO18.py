from run_main import run

run(
    simduration=10,
    simtransient=10,
    duration=50,
    transient=50,
    maxGens=40,
    popSize=26,
    RandSeed=1749493257,
    modelName="CO18",
    modelFolder="Worm2D/CO18",
    outputFolderName="exampleRunCO18",
    doEvol=True,
    overwrite=True,
)
