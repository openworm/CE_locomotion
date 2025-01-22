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

import utils

with open('../exampleRun/worm_data.json', 'r') as file:
         network_json_data = json.load(file)

cell_num = network_json_data["Nervous system"]["size"]["value"]
chemical_weights = network_json_data["Nervous system"]["Chemical weights"]["value"]
electrical_weights = network_json_data["Nervous system"]["Electrical weights"]["value"]
cell_per_unit = network_json_data["Worm global parameters"]["N_neuronsperunit"]["value"]
num_unit = network_json_data["Worm global parameters"]["N_units"]["value"]
cell_names = network_json_data["Nervous system"]["Cell name"]["value"]

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


nml_doc = NeuroMLDocument(id="Worm2D")
nml_doc.includes.append(IncludeType(href='cell_syn_X.xml'))
nml_doc.includes.append(IncludeType(href=cellX_filename))


net = Network(id="Worm2DNet")
nml_doc.networks.append(net)

population_structures = ['one population', 'individual populations', 'cell specific populations']
population_structure = population_structures[0]

if population_structure == 'one population': #all cells in a single population

    size0 = cell_num
    cell_comp = "GenericNeuronCellX"
    pop0 = Population(id="AllCells", component=cell_comp, size=size0)
    net.populations.append(pop0)

    projName, pre_pop, post_pop = utils.getProjectionName(population_structure, 'silentSyn')

    proj0 = ContinuousProjection(id=projName, \
                        presynaptic_population=pre_pop,
                        postsynaptic_population=post_pop)
    net.continuous_projections.append(proj0)

    projName, pre_pop, post_pop = utils.getProjectionName(population_structure, 'gapJunction0')

    elProj0 = ElectricalProjection(
                        id=projName,
                        presynaptic_population=pre_pop,
                        postsynaptic_population=post_pop,
                    )
    net.electrical_projections.append(elProj0)
    
    for index, connection in enumerate(chemical_weights): 
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        weight = connection["weight"]
        
        pre_cell_id = utils.get_cell_id_string(pop0.id, pop0.component, pre_index)
        post_cell_id = utils.get_cell_id_string(pop0.id, pop0.component, post_index)

        conn0 = ContinuousConnectionInstanceW(
                    id=str(index),
                    pre_cell=pre_cell_id,
                    post_cell=post_cell_id,
                    pre_component='silentSyn',
                    post_component='neuron_to_neuron_syn_x',
                    weight=weight,)
        
        proj0.continuous_connection_instance_ws.append(conn0)

    for index, connection in enumerate(electrical_weights): 
        pre_index = connection["from"] - 1 # zero indexing
        post_index = connection["to"] - 1 # zero indexing
        weight = connection["weight"]

        pre_cell_id = utils.get_cell_id_string(pop0.id, pop0.component, pre_index)
        post_cell_id = utils.get_cell_id_string(pop0.id, pop0.component, post_index)

        conn0 = ElectricalConnectionInstanceW(
                    id=str(index),
                    pre_cell=pre_cell_id,
                    post_cell=post_cell_id,
                    synapse='gapJunction0',
                    weight=weight,)
        
        elProj0.electrical_connection_instance_ws.append(conn0)


elif population_structure == 'cell specific populations': # cells divided into cell specific populations

    for ind, pop_cell_name in enumerate(pop_cell_names):
        cell_comp_loc=pop_cell_name
        size0 = num_unit
        pop0 = Population(id='Pop' + pop_cell_name, component=cell_comp_loc, size=size0)
        net.populations.append(pop0)


    utils.makeProjectionsConnections(net, chemical_weights,'silentSyn','continuous', 
                                     population_structure, pop_cell_names, cell_names)
    utils.makeProjectionsConnections(net, electrical_weights,'gapJunction0','electrical', 
                                     population_structure, pop_cell_names, cell_names)


elif population_structure == 'individual populations': #each cell its own population
     
    rel_indices = utils.getPopRelativeCellIndices(pop_cell_names, cell_names)

    for ind, cell_name in enumerate(cell_names):

        cell_rel_index = rel_indices[ind]
        size0 = 1
        pop0 = Population(id='Pop' + cell_name + str(cell_rel_index), component=cell_name, size=size0)
        net.populations.append(pop0)

    utils.makeProjectionsConnections(net, chemical_weights,'silentSyn','continuous', 
                                     population_structure, pop_cell_names, cell_names)
    utils.makeProjectionsConnections(net, electrical_weights,'gapJunction0','electrical', 
                                     population_structure, pop_cell_names, cell_names)
    

else:
    print("Not implemented yet")
    exit(0)


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



nml_file = "testnet.nml"
writers.NeuroMLWriter.write(nml_doc, nml_file)


print("Written network file to: " + nml_file)


###### Validate the NeuroML ######

from neuroml.utils import validate_neuroml2

try:
    validate_neuroml2(nml_file)
except:
    print('Not valid, but this is expected as it contains a newly defined ComponentType (not part of the core NeuroML elements)')
