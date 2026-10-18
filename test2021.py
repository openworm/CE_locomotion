from run_main import run

run(
    maxGens=30,
    popSize=100,
    RandSeed=95166,
    modelFolder="legacy/net21",
    mainProcessName="main",
    outputFolderName="testruns/2021/run",
    doEvol=True,
    overwrite=True,
    # doOrigMuscInput=True,
    # doOrigSRInput=True,
)
