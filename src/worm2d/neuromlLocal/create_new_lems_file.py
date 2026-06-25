"""

Example to create some new LEMS files for running NML2 models

"""

from pyneuroml.lems import LEMSSimulation

# from pyneuroml.lems import generate_lems_file_for_neuroml
import os
import glob

# import sys
import pprint
from pyneuroml.runners import run_jneuroml  # ,run_lems_with_jneuroml_neuron
import shutil

from . import utils
import matplotlib

colour_list = list(matplotlib.colors.cnames.values())


pp = pprint.PrettyPrinter(depth=6)


def run_main(args=None):
    if args is None:
        args = utils.process_args()
    run(a=args)


def run(a=None, **kwargs):
    a = utils.build_namespace(utils.DEFAULTS, a, **kwargs)

    NSIds, VMIds, DMIds = utils.getCellIdDicts()

    network_json_data = utils.getJsonFile(a.json_file)
    output_folder_name = a.output_folder
    model_name = utils.getMainModelName(network_json_data)

    if model_name is not None:
        default_dict = utils.default_cells[model_name]
    else:
        default_dict = None

    # cell_names = utils.getCellNames(network_json_data)
    # pop_names = utils.getPopNames(network_json_data)

    # population_structure = a.population_structure
    # rel_indices = utils.get_rel_index_list(population_structure, cell_names, pop_names)
    # pop_id_list = utils.get_pop_id_list(population_structure, cell_names, pop_names)

    cell_ids = []

    """ pop_id = 'PopDA'
    for rel_index in rel_indices:
            cell_ids.append(utils.get_cell_id_string_full(population_structure, pop_id, None, rel_index)) """

    for id in NSIds:
        cell_ids.append(id["CellId"])

    """ for pop_id in pop_id_list:
        for rel_index in rel_indices:
            cell_ids.append(
                utils.get_cell_id_string_full(
                    population_structure, pop_id, pop_id.replace("Pop", ""), rel_index
                )
            ) """

    ############################################
    #  Create a LEMS file "manually"...

    sim_id = "Worm2D"
    cur_wkd_dir = os.getcwd()
    ls = LEMSSimulation(sim_id, 50000, 1, "Worm2DNet")
    # ls.include_neuroml2_file("NML2_SingleCompHHCell.nml")

    try:
        os.chdir(output_folder_name)

        if default_dict is not None and "XML cell file" in default_dict:
            print(default_dict["XML cell file"])
            print(default_dict["XML cells file"])
            for XML_cell_file in default_dict["XML cell file"]:
                ls.include_lems_file(XML_cell_file)
            ls.include_lems_file(default_dict["XML cells file"])
        else:
            ls.include_lems_file("cell_syn_W2D.xml")
            ls.include_lems_file("cell_syn_W2D_cells.xml")
        doMuscles = a.doMuscles
        if doMuscles:
            ls.include_lems_file("musc_W2D.xml")
            ls.include_lems_file("musc_W2D_cells.xml")
        ls.include_neuroml2_file("Worm2D.net.nml", include_included=False)
    finally:
        os.chdir(cur_wkd_dir)

    disp0 = "display0"
    ls.create_display(disp0, "States", "-15", "10", timeScale="1ms")

    disp1 = "display1"
    ls.create_display(disp1, "Outputs", "-.1", "1", timeScale="1ms")

    def makeDisplay(Ids, file_str):
        vm_ids = []
        for id in Ids:
            vm_ids.append(id["CellId"])
        dispVM0 = "display0_" + file_str
        ls.create_display(dispVM0, "States", "-15", "10", timeScale="1ms")
        ofVM0 = "states_file_" + file_str
        ls.create_output_file(ofVM0, "%s.states_" % sim_id + file_str + ".dat")

        for index, (cell_id, colour) in enumerate(zip(vm_ids, colour_list)):
            cell_id_val = cell_id[3:]
            print("Displaying/saving cell %s" % cell_id_val)

            ls.add_line_to_display(
                dispVM0,
                cell_id_val.replace("Pop", "").replace("/", "_"),
                cell_id_val + "/state",
                "1",
                colour,
                timeScale="1ms",
            )
            ls.add_column_to_output_file(
                ofVM0,
                cell_id_val.replace("Pop", "").replace("/", "_"),
                cell_id_val + "/state",
            )

    if VMIds is not None:
        makeDisplay(VMIds, "VM")
    if DMIds is not None:
        makeDisplay(DMIds, "DM")

    # ls.add_line_to_display(disp0, "v", "AllCells[0]/v", "1mV", "#ffffff")

    cells_to_plot = 60
    of0 = "states_file"
    ls.create_output_file(of0, "%s.states.dat" % sim_id)
    of1 = "outputs_file"
    ls.create_output_file(of1, "%s.outputs.dat" % sim_id)

    for index, (cell_id, colour) in enumerate(
        zip(cell_ids[:cells_to_plot], colour_list)
    ):
        cell_id_val = cell_id[3:]

        print("Displaying/saving cell %s" % cell_id_val)

        # if '0' in cell_id:  # only display first - all in pops are same...
        # if "[" in cell_id:  # display all...
        ls.add_line_to_display(
            disp0,
            cell_id_val.replace("Pop", "").replace("/", "_"),
            cell_id_val + "/state",
            "1",
            colour,
            timeScale="1ms",
        )
        ls.add_column_to_output_file(
            of0,
            cell_id_val.replace("Pop", "").replace("/", "_"),
            cell_id_val + "/state",
        )

        ls.add_line_to_display(
            disp1,
            cell_id_val.replace("Pop", "").replace("/", "_"),
            cell_id_val + "/output",
            "1",
            colour,
            timeScale="1ms",
        )
        ls.add_column_to_output_file(
            of1,
            cell_id_val.replace("Pop", "").replace("/", "_"),
            cell_id_val + "/output",
        )

    # ls.add_column_to_output_file(of0, "v", "AllCells[0]/v")
    # ls.add_column_to_output_file(of0, "v", "PopDA[0]/v")

    ls.set_report_file("report.txt")

    print("Using information to generate LEMS: ")
    pp.pprint(ls.lems_info)
    print("\nLEMS: ")
    # print(ls.to_xml())

    file_name_1 = "LEMS_%s.xml" % sim_id
    file_name = output_folder_name + "/" + file_name_1
    ls.save_to_file(file_name)
    assert os.path.isfile(file_name)

    pre_args = ""
    post_args = "-lems-graph"
    exit_on_fail = True
    run_jneuroml(
        pre_args,
        file_name_1,
        post_args,
        # max_memory=args.java_max_memory,
        exec_in_dir=output_folder_name,
        exit_on_fail=exit_on_fail,
    )
    post_args = "-neuron"
    post_args += " -nogui"
    # post_args += " -compile"

    run_jneuroml(
        pre_args,
        file_name_1,
        post_args,
        verbose=True,
        report_jnml_output=True,
        exec_in_dir=output_folder_name,
        # max_memory=args.java_max_memory,
        exit_on_fail=exit_on_fail,
    )

    this_file_dir = os.path.dirname(os.path.realpath(__file__))
    generated_files = glob.glob(output_folder_name + "/*.mod")
    generated_files.append(output_folder_name + "/LEMS_Worm2D_nrn.py")
    generated_files.append(output_folder_name + "/LEMS_Worm2D.xml")
    for generated_file in generated_files:
        if os.path.isfile(generated_file):
            shutil.copyfile(
                generated_file,
                this_file_dir + "/" + os.path.basename(generated_file),
            )

    # run_lems_with_jneuroml_neuron(file_name_1, only_generate_scripts = True)

    if (
        not cur_wkd_dir == output_folder_name
        and os.path.isfile("LEMS_Worm2D_nrn.py")
        and not os.path.isfile(output_folder_name + "/LEMS_Worm2D_nrn.py")
    ):
        shutil.copyfile(
            "LEMS_Worm2D_nrn.py", output_folder_name + "/LEMS_Worm2D_nrn.py"
        )


if __name__ == "__main__":
    population_structures = [
        "one population",
        "individual populations",
        "cell specific populations",
    ]
    population_structure = population_structures[2]
    run(
        population_structure=population_structure,
        json_file="../exampleRun/worm_data.json",
        output_folder=".",
    )
