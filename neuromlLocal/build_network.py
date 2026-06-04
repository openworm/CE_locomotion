"""

Example to build a full network
through libNeuroML, save it as XML and validate it

"""

import glob
import os
import shutil
import neuroml.writers as writers
from neuroml import (
    Network,
    NeuroMLDocument,
    Population,
    PulseGenerator,
    ElectricalProjection,
    ContinuousProjection,
    ContinuousConnectionInstanceW,
    ElectricalConnectionInstanceW,
    Property,
    Instance,
    Location,
    InputW,
    InputList,
)
# from neuroml import IncludeType

from . import utils
from pyneuroml.modelgraphs import generate_nmlgraph
from neuromllite.MatrixHandler import MatrixHandler
from neuroml.hdf5.NeuroMLXMLParser import NeuroMLXMLParser

import random


def getRandColor():
    col_str = ""
    for _ in range(3):
        col_str += str(random.uniform(0.0, 1.0)) + " "
    return col_str


colors = {
    "AS": ".80 .1 .30",
    "DA": ".82 .7 .43",
    "DB": ".43 .69 .67",
    "DD": ".24 .32 .62",
    "VA": ".52 .33 .17",
    "VB": ".17 .4 .37",
    "VD": ".65 .78 .9",
    "VDA": ".82 .7 .43",
    "VBA": ".52 .33 .17",
    "VBP": ".17 .4 .37",
    "VDP": ".65 .78 .9",
    "SMDD": ".82 .7 .43",
    "RMDD": ".43 .69 .67",
    "SMDV": ".24 .32 .62",
    "RMDV": ".52 .33 .17",
    "MD1": ".2 .8 .1",
    "MD2": ".2 .8 .1",
    "MD3": ".2 .8 .1",
    "MD4": ".2 .8 .1",
    "MV1": ".9 .4 .1",
    "MV2": ".9 .4 .1",
    "MV3": ".9 .4 .1",
    "MV4": ".9 .4 .1",
    "ND": ".80 .1 .30",
    "NV": ".82 .7 .43",
}


# RS18 E/I (V*) type does not seem to obey Dale's rule, here selected arbitrarily.
exc_inh_type = {
    "AS": "I",
    "DA": "E",
    "DB": "E",
    "DD": "I",
    "VA": "E",
    "VB": "E",
    "VD": "I",
    "VDA": "E",
    "VBA": "E",
    "VBP": "E",
    "VDP": "E",
    "SMDD": "E",
    "RMDD": "I",
    "SMDV": "E",
    "RMDV": "I",
    "MD1": "E",
    "MD2": "E",
    "MD3": "E",
    "MD4": "E",
    "MV1": "E",
    "MV2": "E",
    "MV3": "E",
    "MV4": "E",
    "ND": "E",
    "NV": "E",
}


def getRandOrigin():
    origin = []
    for _ in range(2):
        origin.append(random.uniform(0.0, 1.0))
    return origin


origins = {
    "AS": [0, 0],
    "DA": [1, -1],
    "DB": [1, 0],
    "DD": [1, 1],
    "VA": [-1, -1],
    "VB": [-1, 0],
    "VD": [-1, 1],
    "VDA": [1, -1],
    "VBA": [-1, -1],
    "VBP": [-1, 0],
    "VDP": [-1, 1],
    "SMDD": [1, 1],
    "RMDD": [1, -1],
    "SMDV": [-1, -1],
    "RMDV": [-1, 1],
    "MD1": [1, 1],
    "MD2": [1, -1],
    "MD3": [-1, -1],
    "MD4": [-1, 1],
    "MV1": [1, 1],
    "MV2": [1, -1],
    "MV3": [-1, -1],
    "MV4": [-1, 1],
    "ND": [1, 1],
    "NV": [-1, -1],
}

# muscle_ids = [
# "MD1", "MD2", "MD3", "MD4", "MV1", "MV2", "MV3", "MV4"
# ]

spacing = 0.2


def append_pop_properties(pop):
    if pop.component in colors:
        pop.properties.append(Property("color", colors[pop.component]))
    else:
        pop.properties.append(Property("color", getRandColor()))
    # pop.properties.append(Property("type", exc_inh_type[pop.component]))
    pop.type = "populationList"
    for i in range(pop.size):
        inst = Instance(i)
        if pop.component in origins:
            o = origins[pop.component]
        else:
            o = getRandOrigin()
        inst.location = Location(o[0] * 100, o[1] * 100, i * 100)
        pop.instances.append(inst)


def run_main(args=None):
    if args is None:
        args = utils.process_args()
    run(a=args)


def run(a=None, **kwargs):
    a = utils.build_namespace(utils.DEFAULTS, a, **kwargs)

    population_structure = a.population_structure

    network_json_data = utils.getJsonFile(a.json_file)
    output_folder_name = a.output_folder

    chemical_weights = utils.getNervousSystemConnections(network_json_data, "chemical")
    electrical_weights = utils.getNervousSystemConnections(
        network_json_data, "electrical"
    )
    electrical_weights = utils.collapseReciprocalElectricalConnections(
        electrical_weights
    )

    doMuscles = a.doMuscles
    if doMuscles:
        d_muscle_cell_names = []
        v_muscle_cell_names = []
        for muscle_group_size in utils.muscle_group_sizes:
            for i in range(muscle_group_size):
                d_muscle_cell_names.append("MD" + str(i + 1))
                v_muscle_cell_names.append("MV" + str(i + 1))

        muscle_ids = sorted(list(set(d_muscle_cell_names + v_muscle_cell_names)))

        vNMJ_weights = utils.getNMJWeights(network_json_data, "ventral")
        dNMJ_weights = utils.getNMJWeights(network_json_data, "dorsal")
        vNMJ_cellnames = v_muscle_cell_names  # network_json_data["Ventral NMJ"]["Cell name"]["value"]
        dNMJ_cellnames = (
            d_muscle_cell_names  # network_json_data["Dorsal NMJ"]["Cell name"]["value"]
        )

        vNMJ_pop_cell_names = utils.getPopNamesCell(vNMJ_cellnames)
        dNMJ_pop_cell_names = utils.getPopNamesCell(dNMJ_cellnames)
        vNMJ_popSizes = utils.getPopSizes(vNMJ_cellnames, vNMJ_pop_cell_names)
        dNMJ_popSizes = utils.getPopSizes(dNMJ_cellnames, dNMJ_pop_cell_names)

    drop_self_connections = False
    if drop_self_connections and (chemical_weights is not None):
        chemical_weights = utils.dropSelfConnections(chemical_weights)

    # pop_cell_names, cell_names = utils.getPopNamesCellNames(network_json_data)
    model_name = utils.getMainModelName(network_json_data)
    if model_name is not None:
        default_dict = utils.default_cells[model_name]
        cell_names = default_dict["names"]
    else:
        cell_names = utils.getCellNames(network_json_data)
        default_dict = None
    pop_cell_names = utils.getPopNamesCell(cell_names)
    # cell_names = utils.getCellNames(network_json_data)
    # pop_cell_names = utils.getPopNames(network_json_data)
    popSizes = utils.getPopSizes(cell_names, pop_cell_names)

    cur_wkd_dir = os.getcwd()
    this_file_dir = os.path.dirname(
        os.path.realpath(__file__)
    )  # location of this file!

    cell_Id_file_name = this_file_dir + "/cell_Ids.json"
    utils.makeCellIdJson(
        population_structure, cell_names, cell_Id_file_name, "Nervous System"
    )

    if doMuscles:
        utils.makeCellIdJson(
            population_structure,
            vNMJ_cellnames,
            cell_Id_file_name,
            "Ventral Muscles",
            appendFile=True,
        )
        utils.makeCellIdJson(
            population_structure,
            dNMJ_cellnames,
            cell_Id_file_name,
            "Dorsal Muscles",
            appendFile=True,
        )

    input_filenames_to_delete = []
    files_sub = [
        ".mod",
    ]
    files_pre = [
        "Worm2D",
        "LEMS",
    ]

    for file in files_sub:
        input_filenames_to_delete += glob.glob(this_file_dir + "/*" + file)
    for file in files_pre:
        input_filenames_to_delete += glob.glob(this_file_dir + "/" + file + "*")

    utils.deleteFiles(input_filenames_to_delete)
    if os.path.isdir(this_file_dir + "/x86_64"):
        shutil.rmtree(this_file_dir + "/x86_64")

    print(default_dict)
    if default_dict is not None and "XML cell file" in default_dict:
        xml_cell_filenames = default_dict["XML cell file"]
        cells_filename = default_dict["XML cells file"]
        cells_filepath = this_file_dir + "/" + cells_filename
        if "timestep" in default_dict["default parameters"]:
            if isinstance(default_dict["default parameters"]["timestep"], dict):
                default_dict["default parameters"]["timestep"]["value"] = (
                    network_json_data["Evolutionary Optimization Parameters"][
                        "StepSize"
                    ]["value"]
                )
            else:
                default_dict["default parameters"]["timestep"] = network_json_data[
                    "Evolutionary Optimization Parameters"
                ]["StepSize"]["value"]

        utils.makeCellXmlReq(
            network_json_data,
            cells_filepath,
            cell_names,
            default_dict["default parameters"],
            default_dict["XML cell name"],
        )

    else:
        xml_cell_filenames = ["cell_syn_W2D.xml"]
        cells_filename = "cell_syn_W2D_cells.xml"
        cells_filepath = this_file_dir + "/" + cells_filename
        utils.makeCellXml(network_json_data, cells_filepath)

    if doMuscles:
        muscX_filename = "musc_W2D_cells.xml"
        muscX_filepath = this_file_dir + "/" + muscX_filename
        utils.makeMuscCellXml(
            network_json_data, muscX_filepath, vNMJ_cellnames + dNMJ_cellnames
        )

    # copy from current working directory to neuromLocal and output folder
    if not output_folder_name == this_file_dir:
        shutil.copyfile(cell_Id_file_name, output_folder_name + "/cell_Ids.json")
        for xml_cell_filename in xml_cell_filenames:
            shutil.copyfile(
                this_file_dir + "/" + xml_cell_filename,
                output_folder_name + "/" + xml_cell_filename,
            )
        if doMuscles:
            shutil.copyfile(
                this_file_dir + "/musc_W2D.xml", output_folder_name + "/musc_W2D.xml"
            )
        shutil.copyfile(cells_filepath, output_folder_name + "/" + cells_filename)
        if doMuscles:
            shutil.copyfile(muscX_filepath, output_folder_name + "/" + muscX_filename)

    """ if not cur_wkd_dir == this_file_dir:
        shutil.copyfile(cell_Id_file_name, "cell_Ids.json")
        shutil.copyfile(this_file_dir + "/cell_syn_W2D.xml", "cell_syn_W2D.xml")
        if doMuscles:
            shutil.copyfile(this_file_dir + "/musc_X.xml", "musc_X.xml")

    if not output_folder_name == cur_wkd_dir:
        shutil.copyfile(cellW2D_filename, output_folder_name + "/" + cellW2D_filename)
        if doMuscles:
            shutil.copyfile(muscX_filename, output_folder_name + "/" + muscX_filename)
 """
    nml_doc = NeuroMLDocument(id="Worm2D")
    # nml_doc.includes.append(IncludeType(href="cell_syn_X.xml"))
    # nml_doc.includes.append(IncludeType(href=cellX_filename))

    add_gapJunctions = False
    if electrical_weights is not None:
        add_gapJunctions = True
    add_continuousProjections = False
    if chemical_weights is not None:
        add_continuousProjections = True

    net = Network(id="Worm2DNet")
    nml_doc.networks.append(net)

    conn_indices = []
    projNames = []

    if population_structure == "one population":  # all cells in a single population
        """ cell_num = network_json_data["Nervous system"]["size"]["value"]
        size0 = cell_num
        cell_comp = "GenericNeuronCellX"
        pop0 = Population(id=utils.get_pop_id(population_structure), component=cell_comp, size=size0)
        net.populations.append(pop0)

        utils.makeProjectionsConnections(net, chemical_weights,'silentSyn','continuous', 
                                        population_structure, pop_cell_names, cell_names)
        utils.makeProjectionsConnections(net, electrical_weights,'gapJunction0','electrical', 
                                        population_structure, pop_cell_names, cell_names) """

        cell_num = utils.getNervousSystemSize(network_json_data)
        size0 = cell_num
        cell_comp = "GenericNeuronCellW2D"
        pop0 = Population(
            id=utils.get_pop_id(population_structure), component=cell_comp, size=size0
        )

        append_pop_properties(pop0)

        net.populations.append(pop0)

        pre_pop = utils.get_pop_id(population_structure)
        post_pop = utils.get_pop_id(population_structure)
        projName = utils.get_projection_id(pre_pop, post_pop, "silentSyn")

        proj0 = ContinuousProjection(
            id=projName,
            presynaptic_population=pre_pop,
            postsynaptic_population=post_pop,
        )
        net.continuous_projections.append(proj0)

        projName = utils.get_projection_id(pre_pop, post_pop, "gapJunction0")

        elProj0 = ElectricalProjection(
            id=projName,
            presynaptic_population=pre_pop,
            postsynaptic_population=post_pop,
        )
        net.electrical_projections.append(elProj0)

        for index, connection in enumerate(chemical_weights):
            pre_index = connection["from"] - 1  # zero indexing
            post_index = connection["to"] - 1  # zero indexing
            weight = connection["weight"]

            pre_cell_id = utils.get_cell_id_string_full(
                population_structure, pop0.id, pop0.component, pre_index
            )
            post_cell_id = utils.get_cell_id_string_full(
                population_structure, pop0.id, pop0.component, post_index
            )

            conn0 = ContinuousConnectionInstanceW(
                id=str(index),
                pre_cell=pre_cell_id,
                post_cell=post_cell_id,
                pre_component="silentSyn",
                post_component="neuron_to_neuron_syn_w2d",
                weight=utils.FORMAT_CONN_WEIGHTS % weight,
            )

            proj0.continuous_connection_instance_ws.append(conn0)

        for index, connection in enumerate(electrical_weights):
            pre_index = connection["from"] - 1  # zero indexing
            post_index = connection["to"] - 1  # zero indexing
            weight = connection["weight"]

            pre_cell_id = utils.get_cell_id_string_full(
                population_structure, pop0.id, pop0.component, pre_index
            )
            post_cell_id = utils.get_cell_id_string_full(
                population_structure, pop0.id, pop0.component, post_index
            )

            conn0 = ElectricalConnectionInstanceW(
                id=str(index),
                pre_cell=pre_cell_id,
                post_cell=post_cell_id,
                synapse="gapJunction0",
                weight=utils.FORMAT_CONN_WEIGHTS % weight,
            )

            elProj0.electrical_connection_instance_ws.append(conn0)

    elif (
        population_structure == "cell specific populations"
    ):  # cells divided into cell specific populations
        # num_unit = network_json_data["Worm"]["N_units"]["value"]
        for ind, pop_cell_name in enumerate(pop_cell_names):
            cell_comp_loc = pop_cell_name
            # size0 = num_unit
            size0 = popSizes[ind]
            pop0 = Population(
                id=utils.get_pop_id(population_structure, pop_cell_name),
                component=cell_comp_loc,
                size=size0,
            )
            append_pop_properties(pop0)
            net.populations.append(pop0)

        if doMuscles:

            def addMuscles(
                loc_pop_cell_names, loc_popSizes, loc_weights, loc_cellnames
            ):
                for ind, pop_cell_name in enumerate(loc_pop_cell_names):
                    cell_comp_loc = pop_cell_name
                    # size0 = num_unit
                    size0 = loc_popSizes[ind]
                    pop0 = Population(
                        id=utils.get_pop_id(population_structure, pop_cell_name),
                        component=cell_comp_loc,
                        size=size0,
                    )
                    append_pop_properties(pop0)
                    net.populations.append(pop0)

                if loc_weights is not None:
                    utils.makeProjectionsConnections(
                        net,
                        loc_weights,
                        "silentSyn",
                        "continuous",
                        population_structure,
                        pop_cell_names,
                        cell_names,
                        post_pop_cell_names=loc_pop_cell_names,
                        post_cell_names=loc_cellnames,
                        conn_indices=conn_indices,
                        projNames=projNames,
                    )
                # exit()

            addMuscles(dNMJ_pop_cell_names, dNMJ_popSizes, dNMJ_weights, dNMJ_cellnames)
            addMuscles(vNMJ_pop_cell_names, vNMJ_popSizes, vNMJ_weights, vNMJ_cellnames)

        if add_continuousProjections:
            utils.makeProjectionsConnections(
                net,
                chemical_weights,
                "silentSyn",
                "continuous",
                population_structure,
                pop_cell_names,
                cell_names,
                conn_indices=conn_indices,
                projNames=projNames,
            )

        if add_gapJunctions:
            utils.makeProjectionsConnections(
                net,
                electrical_weights,
                "gapJunction0",
                "electrical",
                population_structure,
                pop_cell_names,
                cell_names,
                # conn_indices = conn_indices,
                # projNames = projNames
            )

    elif (
        population_structure == "individual populations"
    ):  # each cell its own population
        rel_indices = utils.getPopRelativeCellIndices(cell_names, pop_cell_names)

        for ind, cell_name in enumerate(cell_names):
            cell_rel_index = rel_indices[ind]
            size0 = 1
            pop0 = Population(
                id=utils.get_pop_id(population_structure, cell_name, cell_rel_index),
                component=cell_name,
                size=size0,
            )
            append_pop_properties(pop0)
            net.populations.append(pop0)

        if add_continuousProjections:
            utils.makeProjectionsConnections(
                net,
                chemical_weights,
                "silentSyn",
                "continuous",
                population_structure,
                pop_cell_names,
                cell_names,
                conn_indices=conn_indices,
                projNames=projNames,
            )
        if add_gapJunctions:
            utils.makeProjectionsConnections(
                net,
                electrical_weights,
                "gapJunction0",
                "electrical",
                population_structure,
                pop_cell_names,
                cell_names,
                conn_indices=conn_indices,
                projNames=projNames,
            )

    else:
        print("Not implemented yet")
        exit(0)

    """
    pop_id_list = utils.get_pop_id_list(
        population_structure, cell_names, pop_cell_names
    )"""
    pop_id = "PopDA"
    add_PG = True
    if (default_dict is not None) and ("add_PG" in default_dict):
        add_PG = default_dict["add_PG"]
    add_ES = True
    if (default_dict is not None) and ("add_ES" in default_dict):
        add_ES = default_dict["add_ES"]
    add_MH = True
    if (default_dict is not None) and ("add_MH" in default_dict):
        add_MH = default_dict["add_MH"]

    if add_PG:
        # pop_stim_ind = 0
        # pop0 = net.populations[pop_stim_ind]
        size0 = 1  # pop0.size
        for pre in range(0, size0):
            pg = PulseGenerator(
                id="pulseGen_%i" % pre,
                delay="20s",
                duration="10s",
                # amplitude="%f nA" % 1,
                amplitude="%f pA" % (10),
            )

            nml_doc.pulse_generators.append(pg)

            input_list = InputList(id="OneStim", component=pg.id, populations=pop_id)

            net.input_lists.append(input_list)

            input_w = InputW(
                id=0,
                target=utils.get_cell_id_string(pop_id, pop_id.replace("Pop", ""), pre),
                destination="synapses",
                weight=1,
            )

            input_list.input_ws.append(input_w)

    if add_ES:
        pg_ext = PulseGenerator(
            id="extStim",
            delay="0s",
            duration="10000s",
            amplitude="1 pA",
        )

        nml_doc.pulse_generators.append(pg_ext)
        for pop in net.populations:
            if not doMuscles or (pop.id[3:] not in muscle_ids):
                input_list = InputList(
                    id="ExtStim%s" % pop.id, component=pg_ext.id, populations=pop.id
                )

                net.input_lists.append(input_list)
                for i in range(pop.size):
                    input_w = InputW(
                        id=i,
                        target=utils.get_cell_id_string(pop.id, pop.component, i),
                        destination="synapses",
                        weight=0,
                    )

                    input_list.input_ws.append(input_w)

    nml_file = this_file_dir + "/Worm2D.net.nml"
    writers.NeuroMLWriter.write(nml_doc, nml_file)

    print("Written network file to: " + nml_file)

    ###### Validate the NeuroML ######

    from neuroml.utils import validate_neuroml2

    try:
        validate_neuroml2(nml_file)
    except Exception:
        print(
            "Not valid, but this is expected as it contains a newly defined ComponentType (not part of the core NeuroML elements)"
        )

    # utils.deleteFiles(["Worm2DNet.gv", "Worm2DNet.gv.png", "Worm2D.net.nml"])
    nml_level = 3
    nml_engine = "circo"
    nml_level = 2
    nml_engine = "dot"
    cwd_before_graph = os.getcwd()
    try:
        os.chdir(this_file_dir)
        generate_nmlgraph(
            "Worm2D.net.nml",
            nml_level,
            nml_engine,
            view_on_render=False,
            include_ext_inputs=False,
        )
    finally:
        os.chdir(cwd_before_graph)
    if not output_folder_name == cur_wkd_dir:
        shutil.copyfile(
            this_file_dir + "/Worm2DNet.gv", output_folder_name + "/Worm2DNet.gv"
        )
        shutil.copyfile(
            this_file_dir + "/Worm2DNet.gv.png",
            output_folder_name + "/Worm2DNet.gv.png",
        )
    if not output_folder_name == this_file_dir:
        shutil.copyfile(nml_file, output_folder_name + "/Worm2D.net.nml")

    if add_MH:
        handler = MatrixHandler(
            level=1,
            nl_network=None,
            show_already=False,
            save_figs_to_dir=output_folder_name,
        )
        currParser = NeuroMLXMLParser(handler)
        cwd_before_matrix = os.getcwd()
        try:
            os.chdir(this_file_dir)
            currParser.parse("Worm2D.net.nml")
        finally:
            os.chdir(cwd_before_matrix)
        handler.finalise_document()


if __name__ == "__main__":
    population_structures = [
        "one population",
        "individual populations",
        "cell specific populations",
    ]
    population_structure = population_structures[2]
    import os

    current = os.path.dirname(os.path.realpath(__file__))
    from pathlib import Path

    parent_dir = Path(__file__).parent
    run_main(
        population_structure=population_structure,
        # json_file="exampleRun21W2D/worm_data.json",
        # output_folder="exampleRun21W2D",
        json_file=parent_dir + "/exampleRun21W2D/worm_data.json",
        output_folder=parent_dir + "/exampleRun21W2D",
    )
