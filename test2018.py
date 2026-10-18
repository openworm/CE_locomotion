from run_main import run

run(
    maxGens=36,
    popSize=36,
    RandSeed=292,
    modelFolder="legacy/rs18",
    mainProcessName="main",
    outputFolderName="testruns/2018/run",
    doEvol=True,
    overwrite=True,
    # doOrigMuscInput=True,
    # doOrigSRInput=True,
)
