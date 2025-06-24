from run_main import run

run(
    maxGens=36,
    popSize=36,
    RandSeed=292,
    # RandSeed=1749493257,
    modelName="CO18",
    modelFolder="Worm2D/CO18",
    outputFolderName="exampleRunCO18",
    doEvol=True,
    overwrite=True,
)
