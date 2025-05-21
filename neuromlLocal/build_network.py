"""

Example to build a full network
through libNeuroML, save it as XML and validate it

"""

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

import utils
from pyneuroml.modelgraphs import generate_nmlgraph
from neuromllite.MatrixHandler import MatrixHandler
from neuroml.hdf5.NeuroMLXMLParser import NeuroMLXMLParser


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
}

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
}

#muscle_ids = [
#"MD1", "MD2", "MD3", "MD4", "MV1", "MV2", "MV3", "MV4"
#]


muscle_group_sizes = [4,3,3,3,3,4,4]
muscle_group_sizes = [1]*24

spacing = 0.2


def append_pop_properties(pop):
    pop.properties.append(Property("color", colors[pop.component]))
    pop.properties.append(Property("type", exc_inh_type[pop.component]))
    pop.type = "populationList"
    for i in range(pop.size):
        inst = Instance(i)
        o = origins[pop.component]
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

    chemical_weights = network_json_data["Nervous system"]["Chemical weights"]["value"]
    electrical_weights = network_json_data["Nervous system"]["Electrical weights"][
        "value"
    ]

    doMuscles = a.doMuscles
    if doMuscles:
        d_muscle_cell_names = []
        v_muscle_cell_names = []
        for muscle_group_size in muscle_group_sizes:
            for i in range(muscle_group_size):
                d_muscle_cell_names.append("MD" + str(i+1))
                v_muscle_cell_names.append("MV" + str(i+1))

        muscle_ids = sorted(list(set(d_muscle_cell_names + v_muscle_cell_names)))


        vNMJ_weights = network_json_data["Ventral NMJ"]["weights"]["value"]
        dNMJ_weights = network_json_data["Dorsal NMJ"]["weights"]["value"]
        vNMJ_cellnames = d_muscle_cell_names #network_json_data["Ventral NMJ"]["Cell name"]["value"]
        dNMJ_cellnames = v_muscle_cell_names #network_json_data["Dorsal NMJ"]["Cell name"]["value"]

        vNMJ_pop_cell_names = utils.getPopNamesCell(vNMJ_cellnames)
        dNMJ_pop_cell_names = utils.getPopNamesCell(dNMJ_cellnames)
        vNMJ_popSizes = utils.getPopSizes(vNMJ_cellnames, vNMJ_pop_cell_names)
        dNMJ_popSizes = utils.getPopSizes(dNMJ_cellnames, dNMJ_pop_cell_names)

    drop_self_connections = False
    if drop_self_connections:
        chemical_weights = utils.dropSelfConnections(chemical_weights)

    # pop_cell_names, cell_names = utils.getPopNamesCellNames(network_json_data)
    cell_names = utils.getCellNames(network_json_data)
    pop_cell_names = utils.getPopNames(network_json_data)
    popSizes = utils.getPopSizes(cell_names, pop_cell_names)

    cur_wkd_dir = os.getcwd()
    this_file_dir = os.path.dirname(
        os.path.realpath(__file__)
    )  # location of this file!
    cellX_filename = "cell_syn_X_cells.xml"
    utils.makeCellXml(network_json_data, cellX_filename)

    if doMuscles:
        muscX_filename = "musc_X_cells.xml"
        utils.makeMuscCellXml(network_json_data, muscX_filename)

    # copy from current working directory to neuromLocal and output folder
    if not output_folder_name == this_file_dir:
        shutil.copyfile(
            this_file_dir + "/cell_syn_X.xml", output_folder_name + "/cell_syn_X.xml"
        )
    if not cur_wkd_dir == this_file_dir:
        shutil.copyfile(this_file_dir + "/cell_syn_X.xml", "cell_syn_X.xml")

    if not output_folder_name == cur_wkd_dir:
        shutil.copyfile(cellX_filename, output_folder_name + "/" + cellX_filename)
        if doMuscles:
            shutil.copyfile(muscX_filename, output_folder_name + "/" + muscX_filename)

    nml_doc = NeuroMLDocument(id="Worm2D")
    # nml_doc.includes.append(IncludeType(href="cell_syn_X.xml"))
    # nml_doc.includes.append(IncludeType(href=cellX_filename))

    add_gapJunctions = True
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

        cell_num = network_json_data["Nervous system"]["size"]["value"]
        size0 = cell_num
        cell_comp = "GenericNeuronCellX"
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
                post_component="neuron_to_neuron_syn_x",
                weight=weight,
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
                weight=weight,
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

                if add_continuousProjections:
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

    pg_ext = PulseGenerator(
        id="extStim",
        delay="0s",
        duration="10000s",
        amplitude="1 pA",
    )

    nml_doc.pulse_generators.append(pg_ext)
    for pop in net.populations:
        if pop.id[3:] not in muscle_ids:
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

    nml_file = "Worm2D.net.nml"
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

    nml_level = 3
    nml_engine = "circo"
    generate_nmlgraph(
        nml_file, nml_level, nml_engine, view_on_render=False, include_ext_inputs=False
    )
    if not output_folder_name == cur_wkd_dir:
        shutil.copyfile("Worm2DNet.gv", output_folder_name + "/Worm2DNet.gv")
        shutil.copyfile("Worm2DNet.gv.png", output_folder_name + "/Worm2DNet.gv.png")
        shutil.copyfile("Worm2D.net.nml", output_folder_name + "/Worm2D.net.nml")

    handler = MatrixHandler(
        level=1,
        nl_network=None,
        show_already=False,
        save_figs_to_dir=output_folder_name,
    )
    currParser = NeuroMLXMLParser(handler)
    currParser.parse(nml_file)
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
