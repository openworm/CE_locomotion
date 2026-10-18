from run_main import run

run(
    popSize=96,
    RandSeed=1233,
    modelFolder="legacy/example",
    mainProcessName="main",
    outputFolderName="testruns/example/run",
    doEvol=True,
    overwrite=True,
)
