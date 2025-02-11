"""

Example to create some new LEMS files for running NML2 models

"""

from pyneuroml.lems import LEMSSimulation
from pyneuroml.lems import generate_lems_file_for_neuroml
import os
import sys
import pprint

import utils
import matplotlib

colour_list = list(matplotlib.colors.cnames.values())


pp = pprint.PrettyPrinter(depth=6)


def run_main(args=None):
    if args is None:
        args = utils.process_args()
    run(a=args)


def run(a=None, **kwargs):
    a = utils.build_namespace(utils.DEFAULTS, a, **kwargs)

    network_json_data = utils.getJsonFile(a.json_file)
    cell_names = utils.getCellNames(network_json_data)
    pop_names = utils.getPopNames(network_json_data)

    population_structure = a.population_structure
    rel_indices = utils.get_rel_index_list(population_structure, cell_names, pop_names)
    pop_id_list = utils.get_pop_id_list(population_structure, cell_names, pop_names)

    cell_ids = []
    """ pop_id = 'PopDA'
    for rel_index in rel_indices:
            cell_ids.append(utils.get_cell_id_string_full(population_structure, pop_id, None, rel_index)) """

    for pop_id in pop_id_list:
        for rel_index in rel_indices:
            cell_ids.append(
                utils.get_cell_id_string_full(
                    population_structure, pop_id, None, rel_index
                )
            )

    ############################################
    #  Create a LEMS file "manually"...

    sim_id = "Worm2D"
    ls = LEMSSimulation(sim_id, 50000, 1, "Worm2DNet")
    # ls.include_neuroml2_file("NML2_SingleCompHHCell.nml")

    ls.include_lems_file("cell_syn_X.xml")
    ls.include_lems_file("cell_syn_X_cells.xml")
    ls.include_neuroml2_file("Worm2D.net.nml", include_included=False)

    disp0 = "display0"
    ls.create_display(disp0, "States", "-15", "10", timeScale="1ms")

    disp1 = "display1"
    ls.create_display(disp1, "Outputs", "-.1", "1", timeScale="1ms")

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
        if "[" in cell_id:  # display all...
            ls.add_line_to_display(
                disp0,
                cell_id_val.replace("Pop", ""),
                cell_id_val + "/state",
                "1",
                colour,
                timeScale="1ms",
            )
            ls.add_column_to_output_file(
                of0, cell_id_val.replace("Pop", ""), cell_id_val + "/state"
            )

            ls.add_line_to_display(
                disp1,
                cell_id_val.replace("Pop", ""),
                cell_id_val + "/output",
                "1",
                colour,
                timeScale="1ms",
            )
            ls.add_column_to_output_file(
                of1, cell_id_val.replace("Pop", ""), cell_id_val + "/output"
            )

    # ls.add_column_to_output_file(of0, "v", "AllCells[0]/v")
    # ls.add_column_to_output_file(of0, "v", "PopDA[0]/v")

    ls.set_report_file("report.txt")

    print("Using information to generate LEMS: ")
    pp.pprint(ls.lems_info)
    print("\nLEMS: ")
    # print(ls.to_xml())

    ls.save_to_file()
    assert os.path.isfile("LEMS_%s.xml" % sim_id)

    """
    ############################################
    #  Create the LEMS file with helper method
    sim_id = "Simple"
    #neuroml_file = "test_data/simplenet.nml"
    neuroml_file = "testnet.nml"
    target = "simplenet"
    duration = 1000
    dt = 0.025
    lems_file_name = "LEMS_%s.xml" % sim_id
    target_dir = "."

    generate_lems_file_for_neuroml(
        sim_id,
        neuroml_file,
        target,
        duration,
        dt,
        lems_file_name,
        target_dir,
        include_extra_files=[],
        gen_plots_for_all_v=True,
        plot_all_segments=False,
        gen_plots_for_quantities={},  # Dict with displays vs lists of quantity paths
        gen_plots_for_only_populations=[],  # List of populations, all pops if = []
        gen_saves_for_all_v=True,
        save_all_segments=False,
        gen_saves_for_only_populations=[],  # List of populations, all pops if = []
        gen_saves_for_quantities={},  # Dict with file names vs lists of quantity paths
        gen_spike_saves_for_all_somas=True,
        report_file_name="report.txt",
        copy_neuroml=True,
        verbose=True,
    )"""

    if "-test" in sys.argv:
        neuroml_file = "test_data/HHCellNetwork.net.nml"
        lems_file_name = "LEMS_%s2.xml" % sim_id
        target = "HHCellNetwork"
        target_dir = "test_data/tmp"
        if not os.path.isdir(target_dir):
            os.mkdir(target_dir)

        generate_lems_file_for_neuroml(
            sim_id,
            neuroml_file,
            target,
            duration=10,
            dt=0.01,
            lems_file_name=lems_file_name,
            target_dir=target_dir,
            copy_neuroml=True,
            verbose=True,
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
    )
