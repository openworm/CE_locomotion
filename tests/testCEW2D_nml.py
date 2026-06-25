from run_main import run

run(
    popSize=96,
    RandSeed=1233,
    modelName="CE",
    modelFolder="Worm2D",
    outputFolderName="exampleRunCEW2D_nml",
    inputFolderName="exampleRunCEW2D",
    doEvol=False,
    overwrite=True,
    doNML=True,
)
