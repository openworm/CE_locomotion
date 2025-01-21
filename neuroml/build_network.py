"""

Example to build a full network
through libNeuroML, save it as XML and validate it

"""
import json
from random import random


import neuroml.writers as writers
from neuroml import (
    ExplicitInput,
    ExpOneSynapse,
    Network,
    NeuroMLDocument,
    Population,
    PulseGenerator,
    ElectricalProjection,
    SynapticConnection,
    ContinuousProjection,
    ContinuousConnectionInstanceW,
    ElectricalConnectionInstanceW,
    IncludeType,
)

cells_have_3d_locations = False

def get_projection_id(pre, post, synclass, syntype = None):
    proj_id = "NC_%s_%s_%s" % (pre, post, synclass)
    """
    if "GapJunction" in syntype:
       proj_id += '_GJ' """

    return proj_id

def get_cell_id_string(pop_id, cell_id, cell_number):
    if cells_have_3d_locations:
        return "../%s/%s/%s" % (pop_id, str(cell_number), cell_id)
    else:
        return "../%s[%s]" % (pop_id, str(cell_number))
        

def getPopRelativeCellIndices(pop_names, cell_names):
    rel_names = list(range(len(cell_names)))
    for pop_name in pop_names:
        indices = [i for i, val in enumerate(cell_names) if val == pop_name]
        for i, val in enumerate(indices):
            rel_names[val] = i
    return rel_names         


with open('../exampleRun/worm_data.json', 'r') as file:
         network_json_data = json.load(file)

cell_num = network_json_data["Nervous system"]["size"]["value"]
chemical_weights = network_json_data["Nervous system"]["Chemical weights"]["value"]
electrical_weights = network_json_data["Nervous system"]["Electrical weights"]["value"]
cell_per_unit = network_json_data["Worm global parameters"]["N_neuronsperunit"]["value"]
num_unit = network_json_data["Worm global parameters"]["N_units"]["value"]
cell_names = network_json_data["Nervous system"]["Cell name"]["value"]
#pop_names = cell_names[:cell_per_unit]

#pop_cell_names = ['cellX_' + val for val in pop_names]
pop_cell_names = cell_names[:cell_per_unit]

cell_biases = network_json_data["Nervous system"]["biases"]["value"]
pop_biases = cell_biases[:cell_per_unit]
cell_gains = network_json_data["Nervous system"]["gains"]["value"]
pop_gains = cell_gains[:cell_per_unit]
cell_taus = network_json_data["Nervous system"]["taus"]["value"]
pop_taus = cell_taus[:cell_per_unit]
cell_states = network_json_data["Nervous system"]["states"]["value"]
pop_states = cell_states[:cell_per_unit]

cellX_strings = []
for ind, pop_cell_name in enumerate(pop_cell_names):
    output_string = '<cellX id="' + str(pop_cell_name) + '" bias="' + str(pop_biases[ind]) + \
    '" gain="' + str(pop_gains[ind]) + '" state0="' + str(pop_states[ind]) + '" tau="' \
    + str(pop_taus[ind]) +'ms"/>'  
    cellX_strings.append(output_string)   

cellX_filename = "cell_syn_X_cells.xml"
with open(cellX_filename, "w") as f:
    f.write('<Lems>\n')
    for val in cellX_strings:
        f.write(val)
        f.write("\n") 
    f.write('</Lems>')     



do_rel_indices = True
rel_indices = getPopRelativeCellIndices(pop_cell_names, cell_names)

nml_doc = NeuroMLDocument(id="Worm2D")
nml_doc.includes.append(IncludeType(href='cell_syn_X.xml'))
nml_doc.includes.append(IncludeType(href=cellX_filename))


net = Network(id="Worm2DNet")
nml_doc.networks.append(net)

population_structures = ['one population', 'individual populations', 'cell specific populations']
population_structure = population_structures[2]

if population_structure == 'one population':

    size0 = cell_num
    cell_comp = "GenericNeuronCellX"
    pop0 = Population(id="AllCells", component=cell_comp, size=size0)
    net.populations.append(pop0)

    proj0 = ContinuousProjection(id="ChemicalProj", \
                        presynaptic_population=pop0.id,
                        postsynaptic_population=pop0.id)
    net.continuous_projections.append(proj0)

    elProj0 = ElectricalProjection(
                        id="ElectricalProj",
                        presynaptic_population=pop0.id,
                        postsynaptic_population=pop0.id,
                    )
    net.electrical_projections.append(elProj0)

elif population_structure == 'cell specific populations':

    for ind, pop_cell_name in enumerate(pop_cell_names):
        cell_comp_loc=pop_cell_name
        size0 = num_unit
        pop0 = Population(id='Pop' + pop_cell_name, component=cell_comp_loc, size=size0)
        net.populations.append(pop0)

    synclass='silentSyn'
    chemProjNames = []
    for connection in chemical_weights:
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        pre_pop = 'Pop' + cell_names[pre_index]
        post_pop = 'Pop' + cell_names[post_index]
        chemProjName = get_projection_id(pre_pop, post_pop, synclass)

        if chemProjName not in chemProjNames:
            chemProjNames.append(chemProjName)
            proj0 = ContinuousProjection(id=chemProjName, presynaptic_population=pre_pop, 
                                        postsynaptic_population=post_pop,)
            net.continuous_projections.append(proj0)

    synclass='gapJunction0'
    elecProjNames = []
    for connection in electrical_weights:
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        pre_pop = 'Pop' +  cell_names[pre_index]
        post_pop = 'Pop' + cell_names[post_index]
        elecProjName = get_projection_id(pre_pop, post_pop, synclass)

        if elecProjName not in elecProjNames:
            elecProjNames.append(elecProjName)
            proj0 = ElectricalProjection(id=elecProjName,presynaptic_population=pre_pop, 
                                         postsynaptic_population=post_pop,)
            net.electrical_projections.append(proj0)


else:
    print("Not implemented yet")
    import sys
    sys.exit(0)




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
    synclass='silentSyn'
    for index, connection in enumerate(chemical_weights): 
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        weight = connection["weight"]
        
        if population_structure == 'one population':

            pre_cell_id = get_cell_id_string(pop0.id, pop0.component, pre_index)
            post_cell_id = get_cell_id_string(pop0.id, pop0.component, post_index)

        elif population_structure == 'cell specific populations':

            pre_cell = cell_names[pre_index]
            pre_pop = 'Pop' + pre_cell
            post_cell = cell_names[post_index]
            post_pop = 'Pop' + post_cell
            
            if do_rel_indices:
                pre_index = rel_indices[pre_index]
                post_index = rel_indices[post_index]
        
            pre_cell_id = get_cell_id_string(pre_pop, pre_cell, pre_index)
            post_cell_id = get_cell_id_string(post_pop, post_cell, post_index)


        conn0 = ContinuousConnectionInstanceW(
                    id=str(index),
                    pre_cell=pre_cell_id,
                    post_cell=post_cell_id,
                    pre_component='silentSyn',
                    post_component='neuron_to_neuron_syn_x',
                    weight=weight,)
        
        if population_structure == 'one population':
            proj0.continuous_connection_instance_ws.append(conn0)

        elif population_structure == 'cell specific populations':
            chemProjName = get_projection_id(pre_pop, post_pop, synclass)
            net.continuous_projections[chemProjNames.index(chemProjName)].continuous_connection_instance_ws.append(conn0)


make_electric_connections = True
if make_electric_connections:
    synclass='gapJunction0'
    for index, connection in enumerate(electrical_weights): 
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        weight = connection["weight"]

        if population_structure == 'one population':

            pre_cell_id = get_cell_id_string(pop0.id, pop0.component, pre_index)
            post_cell_id = get_cell_id_string(pop0.id, pop0.component, post_index)

        elif population_structure == 'cell specific populations':

          
            pre_cell = cell_names[pre_index]
            pre_pop = 'Pop' + pre_cell
            post_cell = cell_names[post_index]
            post_pop = 'Pop' + post_cell

            if do_rel_indices:
                pre_index = rel_indices[pre_index]
                post_index = rel_indices[post_index]
            
        
            pre_cell_id = get_cell_id_string(pre_pop, pre_cell, pre_index)
            post_cell_id = get_cell_id_string(post_pop, post_cell, post_index)
        
        conn0 = ElectricalConnectionInstanceW(
                    id=str(index),
                    pre_cell=pre_cell_id,
                    post_cell=post_cell_id,
                    synapse='gapJunction0',
                    weight=weight,)
        
        if population_structure == 'one population':
            elProj0.electrical_connection_instance_ws.append(conn0)

        elif population_structure == 'cell specific populations':
            elecProjName = get_projection_id(pre_pop, post_pop, synclass)
            #proj0.continuous_connection_instance_ws.append(conn0)
            net.electrical_projections[elecProjNames.index(elecProjName)].electrical_connection_instance_ws.append(conn0)



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

try:
    validate_neuroml2(nml_file)
except:
    print('Not valid, but this is expected as it contains a newly defined ComponentType (not part of the core NeuroML elements)')
