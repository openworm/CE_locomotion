from run_main import run

run(
    simduration=10,
    simtransient=10,
    duration=50,
    transient=50,
    maxGens=25,
    popSize=78,
    RandSeed=1749493257,
    modelName="CO",
    modelFolder="CE_orientation",
    outputFolderName="exampleRunCO",
    doEvol=True,
    overwrite=True,
    doOrigMuscInput=True,
)
