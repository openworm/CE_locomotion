from run_main import run

run(
    simduration=20,
    simtransient=0,
    duration=50,
    transient=50,
    maxGens=25,
    popSize=78,
    RandSeed=1749493257,
    modelName="W2DCO",
    modelFolder="Worm2D",
    outputFolderName="testruns/exW2DCO",
    doEvol=True,
    overwrite=True,
    checkPointInterval=5,
    reRand=True,
    evo_type="EvoCO",
    doTestRun=True,
    doOrigMuscInput=True,
)
