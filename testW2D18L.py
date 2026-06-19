import os
import sys
from run_main import run
from neuromlLocal.regenerate import run as regenerate_run
import helper_funcs as hf

Pi = 3.1415926


# indir = 'COW2DSRE_test_3'
indir = "testW2D18L"
# indir = 'COW2DSRE_test'
outdir = indir + "_out"
pfolder = "testruns"
# pfolder = 'notebooks'

doOrig = True
doNML = False
doMuscles = False

inputFolderName = pfolder + "/" + indir
outputFolderName = pfolder + "/" + outdir


doInputFolder = True
doDelete = False
renewJson = doInputFolder
addSelfConns = False


random_seed = 187566


args = dict(
    duration=50,
    transient=10,
    simduration=500,
    simtransient=0,
    maxGens=20,
    RandSeed=random_seed,
    modelName="W2DSR",
    modelFolder="Worm2D",
    outputFolderName=outputFolderName,
    reRand=True,
    doTestRun=True,
    modifyJson=False,
    doEvol=True,
    resetAgentBody=True,
    orient=Pi / 4,
    rotation=Pi / 4,
    doOrigSRInput=False,
    doOrigMuscInput=False,
    checkPointInterval=5,
    evo_type="EvoCO2",
    overwrite=True,
)

args["popSize"] = 16
args["doCPT"] = True

if doInputFolder is True:
    args["inputFolderName"] = inputFolderName
    hf.delete_subfolder_directory(pfolder, outdir)


if renewJson is True:
    json_data = hf.get_worm_json(inputFolderName)
    old_names = ["Cell_41_0", "Cell_42_0", "Cell_43_0", "Cell_44_0"]

    if doDelete is True:
        for cell_name in old_names:
            json_data = hf.remove_nervous_system_cell(json_data, cell_name)
        json_data = hf.delete_sensor(json_data, "sensor_1")
        json_data = hf.delete_environment(json_data, "environment_1")

    json_data = hf.delete_all_evotags(json_data)

    json_data, cell_names = hf.add_random_cell_network(
        json_data, 4, 1, random_seed=random_seed
    )
    for old_cell_name, cell_name in zip(old_names, cell_names):
        json_data = hf.rename_cell(json_data, cell_name, old_cell_name)
    cell_names = old_names
    if addSelfConns is True:
        for cell_name in cell_names:
            json_data = hf.add_cell_connection(
                json_data, cell_name, cell_name, random_seed=random_seed
            )
            json_data = hf.add_chemical_connection_evotag(
                json_data, cell_name, cell_name
            )
    json_data = hf.add_environment(json_data, "salt_environment")
    json_data = hf.add_sensor(json_data, "salt_environment")
    keys = ["sensors", "sensor_1", "sensor_m"]
    json_data = hf.add_evotag(json_data, keys)
    keys = ["sensors", "sensor_1", "sensor_n"]
    json_data = hf.add_evotag(json_data, keys)
    evolvable_pars = ["tau", "bias"]
    for cell_name in cell_names:
        json_data = hf.add_sensor_connection(
            json_data, "sensor_1", "output_1", cell_name, make_evolvable=True
        )
        json_data = hf.add_sensor_connection(
            json_data, "sensor_1", "output_2", cell_name, make_evolvable=True
        )
        for evolvable_par in evolvable_pars:
            json_data = hf.add_cell_parameter_evotag(
                json_data, cell_name, evolvable_par
            )
            for cell_name_2 in cell_names:
                if cell_name_2 != cell_name:
                    json_data = hf.add_chemical_connection_evotag(
                        json_data, cell_name, cell_name_2
                    )
    head_cells = ["SMDD_0", "SMDV_0"]
    for cell_name, head_cell in zip(cell_names[0:2], head_cells):
        json_data = hf.add_cell_connection(
            json_data, cell_name, head_cell, random_seed=random_seed
        )
        json_data = hf.add_chemical_connection_evotag(json_data, cell_name, head_cell)

    # json_data=hf.delete_sensor(json_data, 'sensor_1')
    # json_data=hf.delete_environment(json_data, 'environment_1')
    # json_data['evolvable_ranges']['evotag_2']["active"] = False
    hf.write_worm_json(outputFolderName, json_data)


if doOrig:
    run(**args)

if doNML:
    try:
        os.chdir("./neuromlLocal")
    except Exception:
        print("Can't change to neuromlLocal.")
        print(sys.exc_info())

    regenerate_run(folder="../" + outputFolderName, doMuscles=False)
    os.chdir("../")

    args["inputFolderName"] = outputFolderName
    args["outputFolderName"] = outputFolderName + "_nml"
    args["doNML"] = True
    args["reRand"] = False
    run(**args)

if doMuscles:
    try:
        os.chdir("./neuromlLocal")
    except Exception:
        print("Can't change to neuromlLocal.")
        print(sys.exc_info())

    regenerate_run(folder="../" + outputFolderName, doMuscles=True)
    os.chdir("../")

    args["inputFolderName"] = outputFolderName
    args["outputFolderName"] = outputFolderName + "_nml_musc"
    args["doNML"] = True
    args["reRand"] = False
    args["doMuscSim"] = True
    run(**args)
