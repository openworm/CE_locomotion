import argparse
import json

from neuroml import (
    ElectricalProjection,
    ContinuousProjection,
    ContinuousConnectionInstanceW,
    ElectricalConnectionInstanceW,
)

DEFAULTS = {}

def process_args():
    pass

def build_namespace(DEFAULTS={}, a=None, **kwargs):
    if a is None:
        a = argparse.Namespace()

    # Add arguments passed in by keyword.
    for key, value in kwargs.items():
        setattr(a, key, value)

    # Add defaults for arguments not provided.
    for key, value in DEFAULTS.items():
        if not hasattr(a, key):
            setattr(a, key, value)

    return a



cells_have_3d_locations = False

def get_projection_id(pre, post, synclass, syntype = None):
    proj_id = "NC_%s_%s_%s" % (pre, post, synclass)
    """
    if "GapJunction" in syntype:
       proj_id += '_GJ' """

    return proj_id



def get_cell_id_string_full(population_structure, pop_id, cell_id, cell_number):
    if population_structure == 'one population': 
       return get_cell_id_string(pop_id, cell_id, cell_number)
    if population_structure == 'cell specific populations':
       return get_cell_id_string(pop_id, cell_id, cell_number)
    if population_structure == 'individual populations':
       return get_cell_id_string(pop_id, cell_id, 0)


def get_cell_id_string(pop_id, cell_id, cell_number):
    if cells_have_3d_locations:
        return "../%s/%s/%s" % (pop_id, str(cell_number), cell_id)
    else:
        return "../%s[%s]" % (pop_id, str(cell_number))
        

def getPopRelativeCellIndices(cell_names, pop_names):
    rel_names = list(range(len(cell_names)))
    for pop_name in pop_names:
        indices = [i for i, val in enumerate(cell_names) if val == pop_name]
        for i, val in enumerate(indices):
            rel_names[val] = i
    return rel_names
             
def get_rel_index_list(population_structure, cell_names = None, pop_names = None):
    if population_structure == 'one population':
       return list(range(len(cell_names)))
    if population_structure == 'individual populations':
       return [0]
    if population_structure == 'cell specific populations':
        return list(set(getPopRelativeCellIndices(cell_names, pop_names)))

def getCellNames(network_json_data):
    return network_json_data["Nervous system"]["Cell name"]["value"]
    
def getPopNames(network_json_data):    
    cell_names = getCellNames(network_json_data)
    return list(set(cell_names))


def get_pop_id_list(population_structure, cell_names = None, pop_names = None):
    if population_structure == 'one population': 
        return ['AllCells']
    if population_structure == 'cell specific populations':
        return ['Pop' + name for name in pop_names]
    if population_structure == 'individual populations':
        rel_inds = getPopRelativeCellIndices(cell_names, pop_names)
        return ['Pop' + name + str(ind) for name, ind in zip(cell_names, rel_inds)]

def get_pop_id(population_structure, name = None, ind = None):
    if population_structure == 'one population': 
        return 'AllCells'
    if population_structure == 'cell specific populations':
        return 'Pop' + name
    if population_structure == 'individual populations':
        return 'Pop' + name + str(ind)


def getJsonFile(json_file):
    with open(json_file, 'r') as file:
         return json.load(file)

def dropSelfConnections(weights):
    new_weights = []
    for connection in weights:
        if connection["from"] != connection["to"]:
           new_weights.append(connection) 
    return new_weights


def makeProjectionsConnections(net, weights, synclass, connection_type, 
                               population_structure, pop_cell_names, cell_names):

        rel_indices = getPopRelativeCellIndices(cell_names, pop_cell_names)

        conn_indices = []
        projNames = []
        for connection in weights:
            pre_index = connection["from"] - 1 # zero indexing
            post_index = connection["to"] - 1 # zero indexing
            weight = connection["weight"]
            
            #make projection name
            pre_cell = cell_names[pre_index]
            post_cell = cell_names[post_index]
            pre_rel_index = rel_indices[pre_index]
            post_rel_index = rel_indices[post_index]


            pre_pop = get_pop_id(population_structure, name = pre_cell, ind = pre_rel_index)
            post_pop = get_pop_id(population_structure, name = post_cell, ind = post_rel_index)

            projName = get_projection_id(pre_pop, post_pop, synclass)

            #add projection if new
            if projName not in projNames:
                projNames.append(projName)
                conn_indices.append(0)

                if connection_type == 'continuous':
                    proj0 = ContinuousProjection(id=projName, presynaptic_population=pre_pop, 
                                                postsynaptic_population=post_pop,)
                    net.continuous_projections.append(proj0)

                elif connection_type == 'electrical':    
                    proj0 = ElectricalProjection(id=projName, presynaptic_population=pre_pop, 
                                         postsynaptic_population=post_pop,)
                    net.electrical_projections.append(proj0)

                else:
                    print('Incorrect connection type')
                    exit(0)    

            cpn_index = projNames.index(projName)
            
            #make cell id and add connection
            
            
            pre_cell_id = get_cell_id_string_full(population_structure, pre_pop, pre_cell, pre_rel_index)
            post_cell_id = get_cell_id_string_full(population_structure, post_pop, post_cell, post_rel_index)

            if connection_type == 'continuous':
                conn0 = ContinuousConnectionInstanceW(
                        id=str(conn_indices[cpn_index]),
                        pre_cell=pre_cell_id,
                        post_cell=post_cell_id,
                        pre_component='silentSyn',
                        post_component='neuron_to_neuron_syn_x',
                        weight=weight,)
            
                net.continuous_projections[cpn_index].continuous_connection_instance_ws.append(conn0)

            elif connection_type == 'electrical':   
                conn0 = ElectricalConnectionInstanceW(
                    id=str(conn_indices[cpn_index]),
                    pre_cell=pre_cell_id,
                    post_cell=post_cell_id,
                    synapse='gapJunction0',
                    weight=weight,)
        
                net.electrical_projections[cpn_index].electrical_connection_instance_ws.append(conn0)

            conn_indices[cpn_index] += 1

def check_equal(list):
    return all(i == list[0] for i in list)

def getVals(pop_names, cell_names, vals):
    
    pop_vals = []
    for pop_name in pop_names:
        vals_list = [vals[i] for i, val in enumerate(cell_names) if val == pop_name]
        if check_equal(vals_list):
           pop_vals.append(vals_list[0])
        else:
           print("Not all equal")
           exit()    
    return pop_vals

def makeCellXml(network_json_data, cellX_filename):
    
    pop_names = getPopNames(network_json_data)
    cell_names = network_json_data["Nervous system"]["Cell name"]["value"]
    cell_biases = network_json_data["Nervous system"]["biases"]["value"]
    cell_gains = network_json_data["Nervous system"]["gains"]["value"]
    cell_taus = network_json_data["Nervous system"]["taus"]["value"]
    cell_states = network_json_data["Nervous system"]["states"]["value"]
    pop_biases = getVals(pop_names, cell_names, cell_biases)
    pop_gains = getVals(pop_names, cell_names, cell_gains)
    pop_taus = getVals(pop_names, cell_names, cell_taus)
    pop_states = getVals(pop_names, cell_names, cell_states)

    cellX_strings = []
    for ind, pop_name in enumerate(pop_names):
        output_string = '<cellX id="' + str(pop_name) + '" bias="' + str(pop_biases[ind]) + \
        '" gain="' + str(pop_gains[ind]) + '" state0="' + str(pop_states[ind]) + '" tau="' \
        + str(pop_taus[ind]) +'ms"/>'  
        cellX_strings.append(output_string)   

    with open(cellX_filename, "w") as f:
        f.write('<Lems>\n')
        for val in cellX_strings:
            f.write(val)
            f.write("\n") 
        f.write('</Lems>')     


