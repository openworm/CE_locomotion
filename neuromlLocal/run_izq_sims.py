import sys
import os

sys.path.append("..")
# sys.path.append("../neuromlLocal")

from run_main import run
from run_main import make_directory
from regenerate import run as regenerate_run

inputFolderBase = "../experiments/izq_selected"
outFolderBase = "../experiments/izq_runs_nets"
# inputFolderBase = "../experiments/test_selected"
# outFolderBase = "../experiments/test_nets"

# popSize = 10
if not make_directory(outFolderBase):
    sys.exit(1)
outFolderBase_nml = "../experiments/izq_runs_nets_nml"
# outFolderBase_nml = "../experiments/test_nets_nml"
if not make_directory(outFolderBase_nml):
    sys.exit(1)
outFolderBase_nml_musc = "../experiments/izq_runs_nets_nml_musc"
if not make_directory(outFolderBase_nml_musc):
    sys.exit(1)

duration = 40
transient = 10

doMuscles = True

path_list = []
out_path_list = []
out_path_list_nml = []
out_path_list_nml_musc = []
# outFolderBases = ["varyEvolSeeds", "varyEvolSeeds1", "varyEvolSeeds2", "varyEvolSeeds3"]

# current = os.path.dirname(os.path.realpath(__file__))  # location of this file!

# path = current + "/" + inputFolderBase
# out_path = current + "/" + outFolderBase
# path = inputFolderBase
# out_path = outFolderBase
dir_list = sorted(os.listdir(inputFolderBase))
# dir_list = dir_list[0:3]
path_list += [inputFolderBase + "/" + dir for dir in dir_list]
out_path_list += [outFolderBase + "/" + dir for dir in dir_list]
out_path_list_nml += [outFolderBase_nml + "/" + dir for dir in dir_list]
out_path_list_nml_musc += [outFolderBase_nml_musc + "/" + dir for dir in dir_list]


doMuscles = False
for input_folder, output_folder, output_folder_nml in zip(
    path_list, out_path_list, out_path_list_nml
):
    print(output_folder)
    run(
        outputFolderName=output_folder,
        inputFolderName=input_folder,
        modelName="Net21",
        modelFolder="../Worm2D",
        doEvol=False,
        overwrite=True,
        duration=duration,
        transient=transient,
        RandSeed=853982,
    )

    print(output_folder_nml)
    regenerate_run(folder=output_folder, doMuscles=doMuscles)
    run(
        outputFolderName=output_folder_nml,
        inputFolderName=output_folder,
        modelName="Net21",
        modelFolder="../Worm2D",
        doEvol=False,
        overwrite=True,
        doNML=True,
        doMuscSim=doMuscles,
        duration=duration,
        transient=transient,
        RandSeed=853982,
    )

doMuscles = True
for output_folder, output_folder_nml_musc in zip(out_path_list, out_path_list_nml_musc):
    print(output_folder_nml_musc)
    regenerate_run(folder=output_folder, doMuscles=doMuscles)
    run(
        outputFolderName=output_folder_nml_musc,
        inputFolderName=output_folder,
        modelName="Net21",
        modelFolder="../Worm2D",
        doEvol=False,
        overwrite=True,
        doNML=True,
        doMuscSim=doMuscles,
        duration=duration,
        transient=transient,
        RandSeed=853982,
    )
