import argparse
import json
import os
import copy
import re
import math
# import helper_funcs as hf

try:
    from neuroml import (
        ElectricalProjection,
        ContinuousProjection,
        ContinuousConnectionInstanceW,
        ElectricalConnectionInstanceW,
    )
except ImportError:
    ElectricalProjection = None
    ContinuousProjection = None
    ContinuousConnectionInstanceW = None
    ElectricalConnectionInstanceW = None


NS_NEW = "nervous_system"
NS_OLD = "Nervous system"


def collapseReciprocalElectricalConnections(weights, warn=True):
    if weights is None:
        return None

    collapsed = []
    by_pair = {}

    for connection in weights:
        pre = connection["from"]
        post = connection["to"]

        if pre == post:
            collapsed.append(connection)
            continue

        pair = tuple(sorted((pre, post)))
        if pair not in by_pair:
            by_pair[pair] = len(collapsed)
            collapsed.append(copy.deepcopy(connection))
            continue

        existing = collapsed[by_pair[pair]]
        old_weight = existing["weight"]
        new_weight = connection["weight"]

        if not math.isclose(old_weight, new_weight, rel_tol=1e-9, abs_tol=1e-12):
            if warn:
                print(
                    "WARNING: reciprocal electrical connection weights differ "
                    "for %s<->%s: %s and %s; using their average for NML gap junction"
                    % (pre, post, old_weight, new_weight)
                )
            existing["weight"] = 0.5 * (old_weight + new_weight)

    return collapsed


def getNervousSystem(network_json_data):
    return network_json_data.get(NS_NEW, network_json_data.get(NS_OLD, {}))


def _value(obj, default=None):
    if isinstance(obj, dict) and "value" in obj:
        return obj["value"]
    if obj is None:
        return default
    return obj


def _strip_cell_suffix(cell_name):
    return re.sub(r"_\d+$", "", cell_name)


def getCellNamesFull(network_json_data):
    ns = getNervousSystem(network_json_data)
    if NS_NEW in network_json_data:
        return _value(ns.get("cell_names"), [])
    return _value(ns.get("Cell name"), [])


def getNervousSystemSize(network_json_data):
    ns = getNervousSystem(network_json_data)
    if NS_NEW in network_json_data:
        return len(getCellNamesFull(network_json_data))
    return _value(ns.get("size"), 0)


def _get_new_cell_values(network_json_data, value):
    ns = getNervousSystem(network_json_data)
    names = getCellNamesFull(network_json_data)
    field_map = {
        "biases": "bias",
        "taus": "tau",
        "gains": "gain",
        "states": "state",
        "Section name": "cell_class",
    }
    field = field_map.get(value, value)
    cells = ns.get("cells", {})
    if not cells:
        return None

    vals = []
    for name in names:
        cell = cells.get(name)
        if not cell or field not in cell:
            return None
        vals.append(_value(cell[field]))
    return vals


def getNervousSystemConnections(network_json_data, kind):
    ns = getNervousSystem(network_json_data)
    if NS_NEW in network_json_data:
        key = "chemical_conns" if kind == "chemical" else "electrical_conns"
        conns = _value(ns.get(key), [])
        names = getCellNamesFull(network_json_data)
        indices = {name: i + 1 for i, name in enumerate(names)}
        return [
            {
                "from": indices[conn["from"]],
                "to": indices[conn["to"]],
                "weight": _value(conn["weight"]),
            }
            for conn in conns
        ]

    key = "Chemical weights" if kind == "chemical" else "Electrical weights"
    return _value(ns.get(key))


def getNMJWeights(network_json_data, side):
    keys = {
        "ventral": ("ventral_nmj", "Ventral NMJ"),
        "dorsal": ("dorsal_nmj", "Dorsal NMJ"),
    }
    new_key, old_key = keys[side]

    if new_key in network_json_data:
        conns = _value(network_json_data[new_key].get("weights"), [])
        names = getCellNamesFull(network_json_data)
        indices = {name: i + 1 for i, name in enumerate(names)}
        return [
            {
                "from": indices[conn["from_cell"]],
                "to": conn["to_musc"],
                "weight": _value(conn["weight"]),
            }
            for conn in conns
        ]

    if old_key in network_json_data:
        return _value(network_json_data[old_key].get("weights"))

    return None


plot_formats = {}
plot_formats["RS18"] = {}
plot_formats["RS18"]["fig_titles"] = [
    "Stretch receptors",
    "Head Neurons",
    "Body Neurons",
    "Muscles",
]
plot_formats["RS18"]["data_sizes"] = [20, 4, 36, 48]
plot_formats["RS18"]["fig_labels"] = ["SR", "Neu", "Neu", "Mu"]
plot_formats["RS18"]["plot_cell_names"] = [
    "DB",
    "DD",
    "VBA",
    "VDA",
    "VBP",
    "VDP",
    "SMDD",
    "RMDD",
    "SMDV",
    "RMDV",
]
plot_formats["RS18"]["plot_col_divs"] = [6, 4]
plot_formats["RS18"]["plot_time"] = 20
plot_formats["RS18"]["worm_plot_time"] = 12
plot_formats["RS18"]["do_body_plot"] = True
plot_formats["RS18"]["do_curv_plot"] = True


plot_formats["Net21"] = {}
plot_formats["Net21"]["fig_titles"] = ["Neurons", "Muscles"]
plot_formats["Net21"]["data_sizes"] = [49, 48]
plot_formats["Net21"]["fig_labels"] = ["Neu", "Mu"]
plot_formats["Net21"]["plot_cell_names"] = ["AS", "DA", "DB", "DD", "VD", "VB", "VA"]
plot_formats["Net21"]["plot_col_divs"] = [4, 3]
plot_formats["Net21"]["plot_time"] = 10
plot_formats["Net21"]["worm_plot_time"] = 2
plot_formats["Net21"]["do_body_plot"] = True
plot_formats["Net21"]["do_curv_plot"] = True
plot_formats["Net21"]["plot_cell_unit"] = 1
plot_formats["Net21"]["AvgSpeed"] = 0.22

plot_formats["CE"] = {}
plot_formats["CE"]["fig_titles"] = ["Stretch receptors", "Neurons", "Muscles"]
plot_formats["CE"]["data_sizes"] = [40, 60, 48]
plot_formats["CE"]["fig_labels"] = ["SR", "Neu", "Mu"]
plot_formats["CE"]["plot_cell_names"] = ["DA", "DB", "DD", "VA", "VB", "VD"]
plot_formats["CE"]["plot_cell_unit"] = 4
plot_formats["CE"]["plot_col_divs"] = [3, 3]
plot_formats["CE"]["plot_time"] = 10
plot_formats["CE"]["worm_plot_time"] = 5
plot_formats["CE"]["do_body_plot"] = True
plot_formats["CE"]["do_curv_plot"] = True

plot_formats["CO"] = {}
plot_formats["CO"]["fig_titles"] = [
    "Inter neurons",
    "Kinesis neurons",
    "Head Motor neurons",
    "Sensory receptors",
]

plot_formats["CO"]["data_sizes"] = [6, 2, 2, 2]
plot_formats["CO"]["fig_labels"] = ["Neu", "Kin", "Mot", "Sen"]
plot_formats["CO"]["plot_cell_names"] = (
    ["I" + str(i) for i in range(6)]
    + ["K" + str(i) for i in range(2)]
    + ["H" + str(i) for i in range(2)]
    + ["S" + str(i) for i in range(2)]
)
plot_formats["CO"]["plot_col_divs"] = [6, 6]
plot_formats["CO"]["plot_time"] = 10
plot_formats["CO"]["worm_plot_time"] = 5
plot_formats["CO"]["do_body_plot"] = True
plot_formats["CO"]["do_curv_plot"] = False

CO18_size = 6
plot_formats["CO18"] = {}
plot_formats["CO18"]["fig_titles"] = ["Neurons", "Sensory"]
plot_formats["CO18"]["data_sizes"] = [CO18_size, 2]
plot_formats["CO18"]["fig_labels"] = ["Neu", "Sen"]
plot_formats["CO18"]["plot_cell_names"] = ["N" + str(i) for i in range(CO18_size)] + [
    "S" + str(i) for i in range(2)
]
plot_formats["CO18"]["plot_col_divs"] = [CO18_size, 2]
plot_formats["CO18"]["plot_time"] = 10
plot_formats["CO18"]["worm_plot_time"] = 5
plot_formats["CO18"]["do_body_plot"] = False
plot_formats["CO18"]["do_curv_plot"] = False

plot_formats["CO18Full"] = plot_formats["CO18"]
plot_formats["W2Dosc"] = copy.deepcopy(plot_formats["Net21"])
plot_formats["W2Dosc"]["data_sizes"] = [48, 48]
plot_formats["W2Dosc"]["plot_cell_names"] = ["ND1", "NV1"]
plot_formats["W2Dosc"]["plot_col_divs"] = [1, 1]
plot_formats["W2Dosc"]["plot_cell_unit"] = 0
plot_formats["W2Dosc21"] = copy.deepcopy(plot_formats["W2Dosc"])
plot_formats["W2Dosc21"]["data_sizes"] = [14, 48]
plot_formats["W2Dosc21all"] = plot_formats["W2Dosc21"]
# plot_formats["W2DoscH"] = plot_formats["W2Dosc"]
# plot_formats["W2DoscH"]["data_sizes"] = [24, 48]
plot_formats["W2Dosc21Coup"] = plot_formats["W2Dosc21"]
plot_formats["W2Dosc21S"] = plot_formats["W2Dosc21"]
plot_formats["W2Dosc21CF"] = plot_formats["W2Dosc21"]
plot_formats["W2D21"] = plot_formats["Net21"]
plot_formats["W2DCE"] = copy.deepcopy(plot_formats["CE"])
plot_formats["W2DCE"]["plot_time"] = 20
plot_formats["W2D21R"] = plot_formats["Net21"]
plot_formats["W2DCESR"] = plot_formats["W2DCE"]
# plot_formats["W2DSR"] = plot_formats["W2DCE"]
plot_formats["W2D18"] = plot_formats["RS18"]
plot_formats["W2DCO"] = plot_formats["CO"]
plot_formats["COW2DSR"] = plot_formats["RS18"]


DEFAULTS = {"doMuscles": False, "folder": None, "popstruct": 2}


FORMAT_CONN_WEIGHTS = "%.8f"

muscle_group_sizes = [4, 3, 3, 3, 3, 4, 4]
muscle_group_sizes = [1] * 24

default_cells = {}
default_cells["Net21"] = {}
default_cells["CE"] = {}
default_cells["RS18"] = {}
default_cells["CO"] = {}

default_cells["Net21"]["add_PG"] = False
default_cells["CE"]["add_PG"] = True
default_cells["RS18"]["add_PG"] = False
default_cells["CO"]["add_PG"] = False


default_cells["Net21"]["names"] = ["AS", "DA", "DB", "DD", "VD", "VB", "VA"] * 7
default_cells["CE"]["names"] = ["DA", "DB", "DD", "VD", "VA", "VB"] * 10
default_cells["RS18"]["names"] = ["DB", "DD", "VBA", "VDA", "VBP", "VDP"] * 6 + [
    "SMDD",
    "RMDD",
    "SMDV",
    "RMDV",
]

default_cells["Net21"]["Section name"] = ["VNC"] * 49
default_cells["CE"]["Section name"] = ["VNC"] * 60
default_cells["RS18"]["Section name"] = ["VNC"] * 36 + ["head"] * 4

default_cells["CO"]["names"] = ["A", "B"]

default_cells["CO"]["Section name"] = ["head"] * 2


default_cells["Worm2Dosc"] = {}
# default_cells["Worm2Dosc"]["names"] = ["NV"]*24 + ["ND"]*24
default_cells["Worm2Dosc"]["names"] = ["NV" + str(i) for i in range(24)] + [
    "ND" + str(i) for i in range(24)
]
default_cells["Worm2Dosc"]["add_PG"] = False
default_cells["Worm2Dosc"]["add_ES"] = True
default_cells["Worm2Dosc"]["add_MH"] = False
default_cells["Worm2Dosc"]["XML cell file"] = ["cell_W2Dosc.xml", "syn_W2D.xml"]
default_cells["Worm2Dosc"]["XML cells file"] = "cell_W2Dosc_cells.xml"
default_cells["Worm2Dosc"]["default parameters"] = {
    "amp": 1,
    "freq": 1,
    "phase": 1,
    # "timestep": {"value": 1, "dim": "s"},
    "tau": {"value": 1, "dim": "s"},
    # "state0": 0,
}
default_cells["Worm2Dosc"]["default parameters"] = {
    "amp": 1,
    "freq": 1,
    "phase": 1,
    "timestep": 0.005,
    # "state0": 0,
}

default_cells["Worm2Dosc"]["XML cell name"] = "cellW2Dosc"


default_cells["Worm2Dosc21"] = copy.deepcopy(default_cells["Worm2Dosc"])
namelist = []
for i in range(7):
    namelist.append("ND" + str(i))
    namelist.append("NV" + str(i))

default_cells["Worm2Dosc21"]["names"] = namelist
default_cells["W2Dosc21all"] = default_cells["Worm2Dosc21"]
default_cells["W2Dosc21"] = default_cells["Worm2Dosc21"]
default_cells["W2Dosc"] = default_cells["Worm2Dosc"]

default_cells["W2DCE"] = default_cells["CE"]
default_cells["W2D18"] = default_cells["RS18"]
default_cells["W2D21"] = default_cells["Net21"]
default_cells["W2D21R"] = default_cells["Net21"]
default_cells["W2DCO"] = default_cells["CO"]


def move_value_to_front(reference_list, value, *other_lists):
    # find where the value is in the first list
    idx = reference_list.index(value)  # raises ValueError if not found

    def move_index_to_front(lst, i):
        item = lst.pop(i)
        lst.insert(0, item)

    # move in the reference list
    move_index_to_front(reference_list, idx)

    # move in all the other lists
    for lst in other_lists:
        if len(lst) <= idx:
            raise IndexError("One of the other lists is too short.")
        move_index_to_front(lst, idx)


def move_value(reference_list, value, *other_lists, to="front"):
    idx = reference_list.index(value)  # raises ValueError if not found

    def move_index(lst, i):
        item = lst.pop(i)
        if to == "front":
            lst.insert(0, item)
        elif to == "back":
            lst.append(item)
        else:
            raise ValueError("to must be 'front' or 'back'")

    move_index(reference_list, idx)

    for lst in other_lists:
        if len(lst) <= idx:
            raise IndexError("One of the other lists is too short.")
        move_index(lst, idx)


def process_args():
    parser = argparse.ArgumentParser(
        description=("A script for building a NML network")
    )

    parser.add_argument(
        "-m",
        "--doMuscles",
        action="store_true",
        # metavar="<include muscles>",
        default=DEFAULTS["doMuscles"],
        help=("Add Muscles to NML"),
    )

    parser.add_argument(
        "-f",
        "--folder",
        type=str,
        metavar="<folder name>",
        default=DEFAULTS["folder"],
        help=("Required name of folder with worm.json for generation of nml files\n"),
    )

    parser.add_argument(
        "-ps",
        "--popstruct",
        type=int,
        metavar="<popstruct>",
        default=DEFAULTS["popstruct"],
        help=(
            "Select population structure, (0) one population,"
            "(1) individual populations,"
            "(2) cell specific populations."
        ),
    )

    return parser.parse_args()


jsonToStringMap = {
    "head": "Head Neurons",
    "interneuron": "Interneurons",
    "VNC": "VNC Neurons",
    "vnc": "VNC Neurons",
}


def getPlotFormat(network_json_data):
    plot_format = {}
    plot_format["fig_titles"] = []
    plot_format["fig_labels"] = []
    plot_format["data_sizes"] = []

    if "Stretch receptor" in network_json_data:
        plot_format["fig_titles"].append("Stretch receptors")
        plot_format["fig_labels"].append("SR")
        plot_format["data_sizes"].append(
            network_json_data["Stretch receptor"]["plot size"]["value"]
        )

    if True:
        json_model_name = getModelName(network_json_data)
        section_names = getNSvalue(network_json_data, "Section name")
        if section_names is None:
            section_names = default_cells[json_model_name]["Section name"]
        oldval = section_names[0]
        # network_json_data["Nervous system"]["Section name"]["value"][0]
        ind = 1
        for val in section_names[1:]:
            if oldval != val:
                if oldval != "dummy":
                    oldval1 = oldval
                    if oldval in jsonToStringMap:
                        oldval1 = jsonToStringMap[oldval]
                    plot_format["fig_titles"].append(oldval1)
                    plot_format["fig_labels"].append("Neu")
                    plot_format["data_sizes"].append(ind)
                ind = 0
                oldval = val
            ind = ind + 1
        if oldval != "dummy":
            oldval1 = oldval
            if oldval in jsonToStringMap:
                oldval1 = jsonToStringMap[oldval]
            plot_format["fig_titles"].append(oldval1)
            plot_format["fig_labels"].append("Neu")
            plot_format["data_sizes"].append(ind)

        move_value(
            plot_format["fig_titles"],
            "VNC Neurons",
            plot_format["fig_labels"],
            plot_format["data_sizes"],
            to="back",
        )

        """ vncind = plot_format["fig_titles"].index("VNC")
        plot_format["fig_titles"].append(plot_format["fig_titles"].pop(vncind))
        plot_format["fig_labels"].append(plot_format["fig_labels"].pop(vncind))
        plot_format["data_sizes"].append(plot_format["data_sizes"].pop(vncind)) """

    if False:
        sects = network_json_data["Nervous system"]["section sizes"]

        if "head" in sects:
            plot_format["fig_titles"].append("Head neurons")
            plot_format["fig_labels"].append("Neu")
            plot_format["data_sizes"].append(sects["head"]["value"])

        if "interneurons" in sects:
            plot_format["fig_titles"].append("Interneurons")
            plot_format["fig_labels"].append("Neu")
            plot_format["data_sizes"].append(sects["interneurons"]["value"])

        if "VNC" in sects:
            plot_format["fig_titles"].append("VNC neurons")
            plot_format["fig_labels"].append("Neu")
            plot_format["data_sizes"].append(sects["VNC"]["value"])

    if "Muscle" in network_json_data:
        plot_format["fig_titles"].append("Muscles")
        plot_format["fig_labels"].append("Mu")
        plot_format["data_sizes"].append(
            network_json_data["Muscle"]["Nmuscles"]["value"] * 2
        )

    if "Driving input" in network_json_data:
        plot_format["fig_titles"].append("Sensory")
        plot_format["fig_labels"].append("Se")
        plot_format["data_sizes"].append(
            network_json_data["Driving input"]["size"]["value"]
        )

    plot_format["plot_time"] = 20
    plot_format["worm_plot_time"] = 12
    plot_format["do_body_plot"] = True
    plot_format["do_curv_plot"] = True

    # print(plot_format)
    # exit
    return plot_format


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


def getCellIdDicts():
    print("calling set_up_from_json")

    import json

    dir_path = os.path.dirname(os.path.realpath(__file__))
    cwd = os.getcwd()
    print("file directory ", dir_path, " current directory ", cwd)

    file_name = dir_path + "/cell_Ids.json"

    if os.path.isfile(file_name):
        with open(file_name) as f:
            cellIdDict = json.load(f)
    else:
        import sys

        print("cell_Ids.json not found")
        sys.exit()

    NSIds = cellIdDict["Nervous System"]
    if "Ventral Muscles" in cellIdDict:
        VMIds = cellIdDict["Ventral Muscles"]
    else:
        VMIds = None
    if "Dorsal Muscles" in cellIdDict:
        DMIds = cellIdDict["Dorsal Muscles"]
    else:
        DMIds = None
    return NSIds, VMIds, DMIds


cells_have_3d_locations = True


def get_projection_id(pre, post, synclass, syntype=None):
    proj_id = "NC_%s_%s_%s" % (pre, post, synclass)
    """
    if "GapJunction" in syntype:
       proj_id += '_GJ' """

    return proj_id


def get_cell_id_string_full(population_structure, pop_id, cell_id, cell_number):
    if population_structure == "one population":
        return get_cell_id_string(pop_id, cell_id, cell_number)
    if population_structure == "cell specific populations":
        return get_cell_id_string(pop_id, cell_id, cell_number)
    if population_structure == "individual populations":
        return get_cell_id_string(pop_id, cell_id, 0)


def get_cell_id_string(pop_id, cell_id, cell_number):
    if cells_have_3d_locations:
        return "../%s/%s/%s" % (pop_id, str(cell_number), cell_id)
    else:
        return "../%s[%s]" % (pop_id, str(cell_number))


def getPopSizes(cell_names, pop_names):
    sizes = []
    for pop_name in pop_names:
        sizes.append(len([i for i, val in enumerate(cell_names) if val == pop_name]))
    return sizes


def getPopRelativeCellIndices(cell_names, pop_names):
    rel_names = list(range(len(cell_names)))
    for pop_name in pop_names:
        indices = [i for i, val in enumerate(cell_names) if val == pop_name]
        for i, val in enumerate(indices):
            rel_names[val] = i
    return rel_names


def get_rel_index_list(population_structure, cell_names=None, pop_names=None):
    if population_structure == "one population":
        return list(range(len(cell_names)))
    if population_structure == "individual populations":
        return [0]
    if population_structure == "cell specific populations":
        return list(set(getPopRelativeCellIndices(cell_names, pop_names)))


def getModelName(network_json_data):
    ns = getNervousSystem(network_json_data)
    if "model_name" in ns:
        return _value(ns["model_name"])
    if "Model name" in ns:
        return _value(ns["Model name"])
    worm = network_json_data.get("worm", {})
    if "main_model_name" in worm:
        return _value(worm["main_model_name"])
    if "Main model name" in worm:
        return _value(worm["Main model name"])
    worm = network_json_data.get("Worm", {})
    if "Main model name" in worm:
        return _value(worm["Main model name"])
    return None


def getModelName_old(network_json_data):
    return getModelName(network_json_data)


def getMainModelName(network_json_data):
    worm_new = network_json_data.get("worm", {})
    if "main_model_name" in worm_new:
        return _value(worm_new["main_model_name"])
    worm = network_json_data.get("Worm", {})
    if "Main model name" in worm:
        return _value(worm["Main model name"])
    return getModelName(network_json_data)


def getIndOfNthVal(val, vals_list, n=0):
    l1 = [i for i, val1 in enumerate(vals_list) if val1 == val]
    if len(l1) > n:
        return l1[n]
    return None


def getCellNames(network_json_data):
    ns = getNervousSystem(network_json_data)
    if NS_NEW in network_json_data:
        if "cell_names_no_suffix" in ns:
            return _value(ns["cell_names_no_suffix"])
        return [
            _strip_cell_suffix(name) for name in getCellNamesFull(network_json_data)
        ]
    if "Cell name" in ns:
        return _value(ns["Cell name"])
    return None


def getNSvalue(network_json_data, value):
    ns = getNervousSystem(network_json_data)
    if NS_NEW in network_json_data:
        if value == "Cell name":
            return getCellNames(network_json_data)
        if value == "size":
            return getNervousSystemSize(network_json_data)
        new_values = _get_new_cell_values(network_json_data, value)
        if new_values is not None:
            return new_values
        old_ns = network_json_data.get(NS_OLD, {})
        if value in old_ns:
            return _value(old_ns[value])
    if value in ns:
        return _value(ns[value])
    return None


def getPopNames(network_json_data):
    cell_names = getCellNames(network_json_data)
    return getPopNamesCell(cell_names)


def getPopNamesCell(cell_names):
    return sorted(list(set(cell_names)))


def get_pop_id_list(population_structure, cell_names=None, pop_names=None):
    if population_structure == "one population":
        return ["AllCells"]
    if population_structure == "cell specific populations":
        return ["Pop" + name for name in pop_names]
    if population_structure == "individual populations":
        rel_inds = getPopRelativeCellIndices(cell_names, pop_names)
        return ["Pop" + name + str(ind) for name, ind in zip(cell_names, rel_inds)]


def get_pop_id(population_structure, name=None, ind=None):
    if population_structure == "one population":
        return "AllCells"
    if population_structure == "cell specific populations":
        return "Pop" + name
    if population_structure == "individual populations":
        return "Pop" + name + str(ind)


def getJsonFile(json_file):
    if not os.path.isfile(json_file):
        return None
    with open(json_file, "r") as file:
        return json.load(file)


def dropSelfConnections(weights):
    new_weights = []
    for connection in weights:
        if connection["from"] != connection["to"]:
            new_weights.append(connection)
    return new_weights


def makeCellIdJson(
    population_structure, cell_names, file_name, json_ind_name, appendFile=False
):
    pop_cell_names = getPopNamesCell(cell_names)
    rel_indices = getPopRelativeCellIndices(cell_names, pop_cell_names)
    # cellIds = []
    cellIdsList = []
    for ind, cell in enumerate(cell_names):
        rel_index = rel_indices[ind]
        pop = get_pop_id(population_structure, name=cell, ind=rel_index)
        """  cellIds.append(get_cell_id_string_full(
            population_structure, pop, cell, rel_index
        )) """
        nrn_pop_name = "m_" + cell + "_Pop" + cell
        cellIdsList.append(
            {
                "CellId": get_cell_id_string_full(
                    population_structure, pop, cell, rel_index
                ),
                "Pop": pop,
                "Cell": cell,
                "Ind": rel_index,
                "NRN pop name": nrn_pop_name,
            }
        )

    if appendFile and os.path.isfile(file_name):
        with open(file_name) as f:
            cellIdDict = json.load(f)
    else:
        cellIdDict = {}

    cellIdDict[json_ind_name] = cellIdsList
    # cellIdDict[json_ind_name]["Ids"] = cellIds
    # cellIdDict[json_ind_name]["List"] = cellIdsList

    dir_path = os.path.dirname(os.path.realpath(__file__))
    cwd = os.getcwd()
    print("making cellId.json, file directory ", dir_path, " current directory ", cwd)

    with open(file_name, "w", encoding="utf-8") as f:
        json.dump(cellIdDict, f, ensure_ascii=False, indent=4)


def makeProjectionsConnections(
    net,
    weights,
    synclass,
    connection_type,
    population_structure,
    pop_cell_names,
    cell_names,
    post_pop_cell_names=None,
    post_cell_names=None,
    conn_indices=None,
    projNames=None,
):
    if ContinuousProjection is None:
        raise ImportError(
            "The neuroml package is required to generate NeuroML projections"
        )

    if not post_pop_cell_names:
        post_pop_cell_names = pop_cell_names
    if not post_cell_names:
        post_cell_names = cell_names

    if conn_indices is None:
        conn_indices = []
    if projNames is None:
        projNames = []

    rel_indices = getPopRelativeCellIndices(cell_names, pop_cell_names)
    post_rel_indices = getPopRelativeCellIndices(post_cell_names, post_pop_cell_names)

    for connection in weights:
        pre_index = connection["from"] - 1  # zero indexing
        post_index = connection["to"] - 1  # zero indexing
        weight = connection["weight"]

        # make projection name
        pre_cell = cell_names[pre_index]
        post_cell = post_cell_names[post_index]
        pre_rel_index = rel_indices[pre_index]
        post_rel_index = post_rel_indices[post_index]

        pre_pop = get_pop_id(population_structure, name=pre_cell, ind=pre_rel_index)
        post_pop = get_pop_id(population_structure, name=post_cell, ind=post_rel_index)

        projName = get_projection_id(pre_pop, post_pop, synclass)

        # add projection if new
        if projName not in projNames:
            projNames.append(projName)
            conn_indices.append(0)

            if connection_type == "continuous":
                proj0 = ContinuousProjection(
                    id=projName,
                    presynaptic_population=pre_pop,
                    postsynaptic_population=post_pop,
                )
                net.continuous_projections.append(proj0)

            elif connection_type == "electrical":
                proj0 = ElectricalProjection(
                    id=projName,
                    presynaptic_population=pre_pop,
                    postsynaptic_population=post_pop,
                )
                net.electrical_projections.append(proj0)

            else:
                print("Incorrect connection type")
                exit(0)

        cpn_index = projNames.index(projName)

        # make cell id and add connection

        pre_cell_id = get_cell_id_string_full(
            population_structure, pre_pop, pre_cell, pre_rel_index
        )
        post_cell_id = get_cell_id_string_full(
            population_structure, post_pop, post_cell, post_rel_index
        )

        if connection_type == "continuous":
            conn0 = ContinuousConnectionInstanceW(
                id=str(conn_indices[cpn_index]),
                pre_cell=pre_cell_id,
                post_cell=post_cell_id,
                pre_component="silentSyn",
                post_component="neuron_to_neuron_syn_w2d",
                weight=FORMAT_CONN_WEIGHTS % weight,
            )

            net.continuous_projections[
                cpn_index
            ].continuous_connection_instance_ws.append(conn0)

        elif connection_type == "electrical":
            conn0 = ElectricalConnectionInstanceW(
                id=str(conn_indices[cpn_index]),
                pre_cell=pre_cell_id,
                post_cell=post_cell_id,
                synapse="gapJunction0",
                weight=FORMAT_CONN_WEIGHTS % weight,
            )

            net.electrical_projections[
                cpn_index
            ].electrical_connection_instance_ws.append(conn0)

        conn_indices[cpn_index] += 1


def check_equal(list):
    return all(i == list[0] for i in list)


def getVals(
    pop_names, cell_names, vals, do_check_equal=True
):  # if check_equal false use average
    pop_vals = []
    for pop_name in pop_names:
        vals_list = [vals[i] for i, val in enumerate(cell_names) if val == pop_name]
        if do_check_equal:
            if check_equal(vals_list):
                pop_vals.append(vals_list[0])
            else:
                print("Not all equal")
                exit()
        else:
            pop_vals.append(sum(vals_list) / len(vals_list))
    return pop_vals


def makeCellXml(network_json_data, cellW2D_filename):
    print("generating CellXml")
    pop_names = getPopNames(network_json_data)
    cell_names = getCellNames(network_json_data)
    cell_biases = getNSvalue(network_json_data, "biases")
    cell_gains = getNSvalue(network_json_data, "gains")
    cell_taus = getNSvalue(network_json_data, "taus")
    cell_states = getNSvalue(network_json_data, "states")
    # print('biases')
    pop_biases = getVals(pop_names, cell_names, cell_biases)
    # print('gains')
    pop_gains = getVals(pop_names, cell_names, cell_gains)
    # print('taus')
    pop_taus = getVals(pop_names, cell_names, cell_taus)
    # print('states')
    pop_states = getVals(pop_names, cell_names, cell_states, do_check_equal=False)

    cellW2D_strings = []
    for ind, pop_cell_name in enumerate(pop_names):
        output_string = (
            '<cellW2D id="'
            + str(pop_cell_name)
            + '" bias="'
            + str(pop_biases[ind])
            + '" gain="'
            + str(pop_gains[ind])
            + '" state0="'
            + str(pop_states[ind])
            + '" tau="'
            + str(pop_taus[ind])
            + 's"/>'
        )
        cellW2D_strings.append(output_string)

    with open(cellW2D_filename, "w") as f:
        f.write("<Lems>\n")
        for val in cellW2D_strings:
            f.write(val)
            f.write("\n")
        f.write("</Lems>")


def deleteFiles(files_to_delete):
    for file_to_delete in files_to_delete:
        deleteFile(file_to_delete)


def deleteFile(file_to_delete):
    if os.path.exists(file_to_delete):
        os.remove(file_to_delete)


def makeCellXmlGen(network_json_data, filename, cell_names):
    print("generating CellXml")
    pop_names = getPopNamesCell(cell_names)
    vals = {}
    ns = getNervousSystem(network_json_data)
    if NS_NEW in network_json_data:
        for key in ["biases", "gains", "taus", "states"]:
            cell_vals = getNSvalue(network_json_data, key)
            if cell_vals is not None:
                vals[key] = getVals(pop_names, cell_names, cell_vals)
    else:
        for key in ns:
            if "cell_val" in ns[key] and ns[key]["cell_val"] == 1:
                cell_vals = _value(ns[key])
                vals[key] = getVals(pop_names, cell_names, cell_vals)
    cell_strings = []
    for ind, pop_cell_name in enumerate(pop_names):
        output_string = '<cellW2D id="' + str(pop_cell_name)
        for key in vals:
            output_string += f'" {key}="'
            +str(vals[key][ind])
        output_string += 's"/>'
        cell_strings.append(output_string)
    with open(filename, "w") as f:
        f.write("<Lems>\n")
        for val in cell_strings:
            f.write(val)
            f.write("\n")
        f.write("</Lems>")


def makeCellXmlReq(
    network_json_data, filename, cell_names, par_name_default, xml_cell_name
):
    print("generating CellXml")
    pop_names = getPopNamesCell(cell_names)
    vals = {}
    for key in par_name_default:
        cell_vals = getNSvalue(network_json_data, key)
        if cell_vals is not None:
            vals[key] = getVals(pop_names, cell_names, cell_vals)
        elif key == "timestep":
            sim = network_json_data.get("Simulation", {})
            step_size = _value(sim.get("StepSize"), par_name_default[key])
            vals[key] = [step_size] * len(cell_names)
        else:
            if isinstance(par_name_default[key], dict):
                vals[key] = [par_name_default[key]["value"]] * len(cell_names)
            else:
                vals[key] = [par_name_default[key]] * len(cell_names)
    cell_strings = []
    for ind, pop_cell_name in enumerate(pop_names):
        output_string = f'<{xml_cell_name} id="' + str(pop_cell_name)
        for key in vals:
            output_string += f'" {key}="' + str(vals[key][ind])
            if isinstance(par_name_default[key], dict):
                output_string += f"{par_name_default[key]['dim']}"
        output_string += '"/>'
        cell_strings.append(output_string)
    with open(filename, "w") as f:
        f.write("<Lems>\n")
        for val in cell_strings:
            f.write(val)
            f.write("\n")
        f.write("</Lems>")


def makeMuscCellXml(network_json_data, cellX_filename, cell_names):
    pop_names = getPopNamesCell(cell_names)

    print("generating MuscCellXml")

    worm = network_json_data.get("worm", network_json_data.get("Worm", {}))
    cell_taus = [worm["T_muscle"]["value"]] * len(cell_names)
    cell_states = [0] * len(cell_names)
    # print('taus')
    pop_taus = getVals(pop_names, cell_names, cell_taus)
    # print('states')
    pop_states = getVals(pop_names, cell_names, cell_states)

    cellX_strings = []
    for ind, pop_cell_name in enumerate(pop_names):
        output_string = (
            '<muscW2D id="'
            + str(pop_cell_name)
            + '" state0="'
            + str(pop_states[ind])
            + '" tau="'
            + str(pop_taus[ind])
            + 's"/>'
        )
        cellX_strings.append(output_string)

    with open(cellX_filename, "w") as f:
        f.write("<Lems>\n")
        for val in cellX_strings:
            f.write(val)
            f.write("\n")
        f.write("</Lems>")
