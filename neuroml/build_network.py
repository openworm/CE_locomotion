"""

Example to build a full spiking IaF network
through libNeuroML, save it as XML and validate it

"""
import json
from random import random


import neuroml.writers as writers
from neuroml import (
    ExplicitInput,
    ExpOneSynapse,
    IafCell,
    Network,
    NeuroMLDocument,
    Population,
    PulseGenerator,
    ElectricalProjection,
    SynapticConnection,
    Projection,
    ConnectionWD,
    ElectricalConnectionInstanceW,
)

def get_projection_id(pre, post, synclass, syntype):
    proj_id = "NC_%s_%s_%s" % (pre, post, synclass)
    """
    if "GapJunction" in syntype:
       proj_id += '_GJ' """

    return proj_id

def get_cell_id_string(pop_id, cell_id, cell_number):
    return "../%s/%s/%s" % (pop_id, str(cell_number), cell_id)
        

with open('../exampleRun/worm_data.json', 'r') as file:
         network_json_data = json.load(file)

cell_num = network_json_data["Nervous system"]["size"]["value"]
chemical_weights = network_json_data["Nervous system"]["Chemical weights"]["value"]
electrical_weights = network_json_data["Nervous system"]["Electrical weights"]["value"]

nml_doc = NeuroMLDocument(id="Worm2D")

IafCell0 = IafCell(
    id="iaf0",
    C="1.0 nF",
    thresh="-50mV",
    reset="-65mV",
    leak_conductance="10 nS",
    leak_reversal="-65mV",
)

nml_doc.iaf_cells.append(IafCell0)

syn0 = ExpOneSynapse(id="syn0", gbase="65nS", erev="0mV", tau_decay="3ms")
nml_doc.exp_one_synapses.append(syn0)


net = Network(id="IafNet")

nml_doc.networks.append(net)

size0 = cell_num
pop0 = Population(id="IafPop0", component=IafCell0.id, size=size0)

net.populations.append(pop0)

proj0 = Projection(id="ChemicalProj",presynaptic_population=pop0.id,postsynaptic_population=pop0.id,
                   synapse=syn0.id,
                )
elProj0 = ElectricalProjection(
                    id="ElectricalProj",
                    presynaptic_population=pop0.id,
                    postsynaptic_population=pop0.id,
                )

net.electrical_projections.append(elProj0)

net.projections.append(proj0)


add_PG = False
if add_PG:
    for pre in range(0, size0):
        pg = PulseGenerator(
            id="pulseGen_%i" % pre,
            delay="0ms",
            duration="100ms",
            amplitude="%f nA" % (0.1 * random()),
        )

        nml_doc.pulse_generators.append(pg)

        exp_input = ExplicitInput(target="%s[%i]" % (pop0.id, pre), input=pg.id)

        net.explicit_inputs.append(exp_input)


make_connections = True
if make_connections:
    for index, connection in enumerate(chemical_weights): 
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        weight = connection["weight"]

        pre_cell_id = get_cell_id_string(pop0.id, IafCell0.id, pre_index)
        post_cell_id = get_cell_id_string(pop0.id, IafCell0.id, post_index)

        conn0 = ConnectionWD(
                    id=str(index),
                    pre_cell_id=pre_cell_id,
                    post_cell_id=post_cell_id,
                    weight=weight,
                    delay="0ms",
                )

        proj0.connection_wds.append(conn0)

make_electric_connections = True
if make_electric_connections:
    for index, connection in enumerate(electrical_weights): 
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        weight = connection["weight"]

        pre_cell_id = get_cell_id_string(pop0.id, IafCell0.id, pre_index)
        post_cell_id = get_cell_id_string(pop0.id, IafCell0.id, post_index)

        
        conn0 = ElectricalConnectionInstanceW(
                    id=str(index),
                    pre_cell=pre_cell_id,
                    post_cell=post_cell_id,
                    #synapse=syn_new.id,
                    weight=weight,
                )

        elProj0.electrical_connection_instance_ws.append(conn0)
    






""" syn = SynapticConnection(
            from_="%s[%i]" % (pop0.id, pre),
            synapse=syn0.id,
            to="%s[%i]" % (pop0.id, post),
        )
net.synaptic_connections.append(syn) """

    
nml_file = "testnet.nml"
writers.NeuroMLWriter.write(nml_doc, nml_file)


print("Written network file to: " + nml_file)


###### Validate the NeuroML ######

from neuroml.utils import validate_neuroml2

validate_neuroml2(nml_file)
