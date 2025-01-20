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
    ContinuousProjection,
    ContinuousConnectionInstanceW,
    ElectricalConnectionInstanceW,
    IncludeType,
)

def get_projection_id(pre, post, synclass):
    proj_id = "NC_%s_%s_%s" % (pre, post, synclass)
    """
    if "GapJunction" in syntype:
       proj_id += '_GJ' """

    return proj_id


cells_have_3d_locations = False

def get_cell_id_string(pop_id, cell_id, cell_number):
    if cells_have_3d_locations:
        return "../%s/%s/%s" % (pop_id, str(cell_number), cell_id)
    else:
        return "../%s[%s]" % (pop_id, str(cell_number))


        

with open('../exampleRun/worm_data.json', 'r') as file:
         network_json_data = json.load(file)

cell_num = network_json_data["Nervous system"]["size"]["value"]
chemical_weights = network_json_data["Nervous system"]["Chemical weights"]["value"]
electrical_weights = network_json_data["Nervous system"]["Electrical weights"]["value"]
cell_per_unit = network_json_data["Worm global parameters"]["N_neuronsperunit"]["value"]
num_unit = network_json_data["Worm global parameters"]["N_units"]["value"]
cell_names = network_json_data["Nervous system"]["Cell name"]["value"]
pop_names = cell_names[:cell_per_unit]


nml_doc = NeuroMLDocument(id="Worm2D")
nml_doc.includes.append(IncludeType(href='cell_syn_X.xml'))


syn0 = ExpOneSynapse(id="syn0", gbase="65nS", erev="0mV", tau_decay="3ms")
nml_doc.exp_one_synapses.append(syn0)


net = Network(id="Worm2DNet")
nml_doc.networks.append(net)

#make populations, one for each cell type


cell_comp = "GenericNeuronCellX"
#pop0 = Population(id="AllCells", component=cell_comp, size=size0)

for pop_name in pop_names:
    size0 = num_unit
    pop0 = Population(id=pop_name, component=cell_comp, size=size0)
    net.populations.append(pop0)


synclass='silentSyn'
chemProjNames = []
for connection in chemical_weights:
    pre_index = connection["from"] - 1 # zero indexing
    post_index = connection["to"] - 1 # zero indexing
    pre_pop = cell_names[pre_index]
    post_pop = cell_names[post_index]
    chemProjName = get_projection_id(pre_pop, post_pop, synclass)
    if chemProjName not in chemProjNames:
        chemProjNames.append(chemProjName)
        proj0 = ContinuousProjection(id=chemProjName,presynaptic_population=pre_pop, 
                                     postsynaptic_population=post_pop,)
        net.continuous_projections.append(proj0)


synclass='gapJunction0'
elecProjNames = []
for connection in electrical_weights:
    pre_index = connection["from"] - 1 # zero indexing
    post_index = connection["to"] - 1 # zero indexing
    pre_pop = cell_names[pre_index]
    post_pop = cell_names[post_index]
    elecProjName = get_projection_id(pre_pop, post_pop, synclass)
    if elecProjName not in elecProjNames:
        elecProjNames.append(elecProjName)
        proj0 = ElectricalProjection(id=elecProjName,presynaptic_population=pre_pop, postsynaptic_population=post_pop,)
        net.electrical_projections.append(proj0)



""" elProj0 = ElectricalProjection(
                    id="ElectricalProj",
                    presynaptic_population=pop0.id,
                    postsynaptic_population=pop0.id,
                )

net.electrical_projections.append(elProj0)
 """

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

synclass='silentSyn'
make_connections = True
if make_connections:
    for index, connection in enumerate(chemical_weights): 
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        weight = connection["weight"]
        pre_pop = cell_names[pre_index]
        post_pop = cell_names[post_index]
        pre_cell_id = get_cell_id_string(pre_pop, cell_comp, pre_index)
        post_cell_id = get_cell_id_string(post_pop, cell_comp, post_index)

        conn0 = ContinuousConnectionInstanceW(
                    id=str(index),
                    pre_cell=pre_cell_id,
                    post_cell=post_cell_id,
                    pre_component='silentSyn',
                    post_component='neuron_to_neuron_syn_x',
                    weight=weight,)
        chemProjName = get_projection_id(pre_pop, post_pop, synclass)
        #proj0.continuous_connection_instance_ws.append(conn0)chemProjNames
        net.continuous_projections[chemProjNames.index(chemProjName)].continuous_connection_instance_ws.append(conn0)


""" if make_connections:
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

        proj0.connection_wds.append(conn0) """

synclass='gapJunction0'
make_electric_connections = True
if make_electric_connections:
    for index, connection in enumerate(electrical_weights): 
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        weight = connection["weight"]
        pre_pop = cell_names[pre_index]
        post_pop = cell_names[post_index]
        pre_cell_id = get_cell_id_string(pre_pop, cell_comp, pre_index)
        post_cell_id = get_cell_id_string(post_pop, cell_comp, post_index)

        
        conn0 = ElectricalConnectionInstanceW(
                    id=str(index),
                    pre_cell=pre_cell_id,
                    post_cell=post_cell_id,
                    synapse='gapJunction0',
                    weight=weight,
                )
        elecProjName = get_projection_id(pre_pop, post_pop, synclass)
        #proj0.continuous_connection_instance_ws.append(conn0)
        net.electrical_projections[elecProjNames.index(elecProjName)].electrical_connection_instance_ws.append(conn0)


""" make_electric_connections = True
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

        elProj0.electrical_connection_instance_ws.append(conn0) """
    






""" syn = SynapticConnection(
            from_="%s[%i]" % (pop0.id, pre),
            synapse=syn0.id,
            to="%s[%i]" % (pop0.id, post),
        )
net.synaptic_connections.append(syn) """

    
nml_file = "testnet_pop.nml"
writers.NeuroMLWriter.write(nml_doc, nml_file)


print("Written network file to: " + nml_file)


###### Validate the NeuroML ######

from neuroml.utils import validate_neuroml2

try:
    validate_neuroml2(nml_file)
except:
    print('Not valid, but this is expected as it contains a newly defined ComponentType (not part of the core NeuroML elements)')

