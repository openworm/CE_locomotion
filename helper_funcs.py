import argparse
import copy
import html
import json
import math
import os
import random
import shutil
import sys
from functools import partial

import numpy as np
from matplotlib import pyplot as plt

try:
    from scipy.stats import binned_statistic
except ImportError:
    binned_statistic = None

try:
    import ipywidgets as widgets
except ImportError:
    widgets = None


dir_name = None
file_prefix = None

title_font_size = 16
label_font_size = 14


DEFAULTS = {"modelName": None, "showPlot": True, "folderName": None, "verbose": False}


def short_repr(x, max_len=80):
    text = json.dumps(x, ensure_ascii=False)
    if len(text) > max_len:
        text = text[:max_len] + "..."
    return html.escape(text)


def make_json_tree(obj, title="root"):
    if widgets is None:
        raise ImportError("ipywidgets is required to display JSON trees")

    if isinstance(obj, dict):
        children = []
        titles = []

        for key, value in obj.items():
            children.append(make_json_tree(value, key))

            if isinstance(value, dict):
                titles.append(f"{key}  {{...}}")
            elif isinstance(value, list):
                titles.append(f"{key}  [...]")
            else:
                titles.append(f"{key}: {short_repr(value)}")

        acc = widgets.Accordion(children=children)
        for i, t in enumerate(titles):
            acc.set_title(i, t)

        return acc

    elif isinstance(obj, list):
        if all(is_simple(x) for x in obj):
            text = json.dumps(obj, indent=2, ensure_ascii=False)
            return widgets.HTML(f"<pre>{html.escape(text)}</pre>")

        children = []
        titles = []

        for value in obj:
            children.append(make_json_tree(value))

            if isinstance(value, dict):
                titles.append("{...}")
            elif isinstance(value, list):
                titles.append("[...]")
            else:
                titles.append(short_repr(value))

        acc = widgets.Accordion(children=children)
        for i, t in enumerate(titles):
            acc.set_title(i, t)

        return acc

    else:
        text = json.dumps(obj, indent=2, ensure_ascii=False)
        return widgets.HTML(f"<pre>{html.escape(text)}</pre>")


def is_simple(value):
    return isinstance(value, (str, int, float, bool)) or value is None


def json_widget(obj):
    if widgets is None:
        raise ImportError("ipywidgets is required to display JSON widgets")

    if isinstance(obj, dict):
        children = []
        titles = []

        for key, value in obj.items():
            children.append(json_widget(value))
            titles.append(str(key))

        acc = widgets.Accordion(children=children)
        for i, title in enumerate(titles):
            acc.set_title(i, title)

        return acc

    elif isinstance(obj, list):
        if all(is_simple(x) for x in obj):
            text = json.dumps(obj, indent=2)
            return widgets.HTML(f"<pre>{html.escape(text)}</pre>")

        else:
            children = [json_widget(value) for value in obj]

            acc = widgets.Accordion(children=children)

            # Hide the numeric index by using a generic or blank title
            for i in range(len(children)):
                acc.set_title(i, "")

            return acc

    else:
        text = json.dumps(obj, indent=2)
        return widgets.HTML(f"<pre>{html.escape(text)}</pre>")


def json_widget_2(obj, name="root"):
    """
    Recursively display JSON-like Python objects using ipywidgets.
    Supports dicts, lists, strings, numbers, booleans, and None.
    """
    if widgets is None:
        raise ImportError("ipywidgets is required to display JSON widgets")

    if isinstance(obj, dict):
        children = []
        titles = []

        for key, value in obj.items():
            children.append(json_widget(value, str(key)))
            titles.append(str(key))

        accordion = widgets.Accordion(children=children)
        for i, title in enumerate(titles):
            accordion.set_title(i, title)

        return accordion

    elif isinstance(obj, list):
        children = []
        titles = []

        for i, value in enumerate(obj):
            children.append(json_widget(value, f"[{i}]"))
            titles.append(f"[{i}]")

        accordion = widgets.Accordion(children=children)
        for i, title in enumerate(titles):
            accordion.set_title(i, title)

        return accordion

    else:
        return widgets.HTML(value=f"<pre>{repr(obj)}</pre>")


def get_worm_file():
    worm_file = rename_file("worm_data_worm.json")
    if not os.path.isfile(worm_file):
        worm_file = rename_file("worm_data_evo.json")
    if not os.path.isfile(worm_file):
        worm_file = rename_file("worm_data.json")

    return worm_file


def get_worm_json(folder_name):
    """Load worm_data_worm.json from a run directory."""
    filename = os.path.join(folder_name, "worm_data_worm.json")
    with open(filename, "r") as f:
        return json.load(f)


def write_worm_json(folder_name, json_data):
    """Write worm_data_worm.json to a run directory."""
    os.makedirs(folder_name, exist_ok=True)
    filename = os.path.join(folder_name, "worm_data_worm.json")
    with open(filename, "w") as f:
        json.dump(json_data, f)


def add_environment(
    json_data,
    name=None,
    x_center=0.0,
    y_center=0.0,
    grad_steep=0.5,
):
    """Return a copy of a worm JSON dictionary with a new environment."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")

    for parameter_name, value in (
        ("x_center", x_center),
        ("y_center", y_center),
        ("grad_steep", grad_steep),
    ):
        if isinstance(value, bool) or not isinstance(value, (int, float)):
            raise TypeError("{} must be a number".format(parameter_name))

    result = copy.deepcopy(json_data)
    environments = result.setdefault("environments", {})
    if not isinstance(environments, dict):
        raise TypeError("'environments' must be a dictionary")

    if name is None:
        index = 1
        while "environment_{}".format(index) in environments:
            index += 1
        name = "environment_{}".format(index)
    elif not isinstance(name, str) or not name.strip():
        raise ValueError("name must be a non-empty string")
    elif name in environments:
        raise ValueError("Environment {!r} already exists".format(name))

    environments[name] = {
        "name": {"value": name},
        "x_center": {"value": float(x_center)},
        "y_center": {"value": float(y_center)},
        "grad_steep": {"value": float(grad_steep)},
    }
    return result


def delete_environment(json_data, environment_name):
    """Return a copy with an unused environment removed."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(environment_name, str) or not environment_name:
        raise ValueError("environment_name must be a non-empty string")

    result = copy.deepcopy(json_data)
    environments = result.get("environments")
    if not isinstance(environments, dict):
        raise KeyError("JSON does not contain an 'environments' object")

    environment_key = None
    canonical_name = None
    if environment_name in environments:
        environment_key = environment_name
        environment = environments[environment_key]
        canonical_name = environment_name
        if isinstance(environment, dict):
            stored_name = environment.get("name", {}).get("value")
            if isinstance(stored_name, str) and stored_name:
                canonical_name = stored_name
    else:
        for key, environment in environments.items():
            if (
                isinstance(environment, dict)
                and environment.get("name", {}).get("value")
                == environment_name
            ):
                environment_key = key
                canonical_name = environment_name
                break
    if environment_key is None:
        raise KeyError(
            "Environment {!r} does not exist".format(environment_name)
        )

    sensors = result.get("sensors", {})
    if not isinstance(sensors, dict):
        raise TypeError("'sensors' must be a dictionary")

    users = []
    for sensor_name, sensor in sensors.items():
        if not isinstance(sensor, dict):
            raise TypeError("Sensor {!r} must be a dictionary".format(sensor_name))
        sensor_environment = sensor.get("environment", {}).get("value")
        if sensor_environment in {
            environment_name,
            environment_key,
            canonical_name,
        }:
            users.append(sensor_name)

    if users:
        raise ValueError(
            "Environment {!r} is used by: {}".format(
                canonical_name, ", ".join(sorted(users))
            )
        )

    del environments[environment_key]
    if not environments:
        result.pop("environments", None)
    return result


def add_sensor(
    json_data,
    environment_name,
    sensor_n=2.0,
    sensor_m=2.0,
    hs_stepsize=0.01,
    input_strength=0.0,
):
    """Return a copy of a worm JSON dictionary with a new sensor."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(environment_name, str) or not environment_name:
        raise ValueError("environment_name must be a non-empty string")

    for parameter_name, value in (
        ("sensor_n", sensor_n),
        ("sensor_m", sensor_m),
        ("hs_stepsize", hs_stepsize),
        ("input_strength", input_strength),
    ):
        if isinstance(value, bool) or not isinstance(value, (int, float)):
            raise TypeError("{} must be a number".format(parameter_name))
    if sensor_n <= 0 or sensor_m <= 0 or hs_stepsize <= 0:
        raise ValueError("sensor_n, sensor_m and hs_stepsize must be positive")

    result = copy.deepcopy(json_data)
    environments = result.get("environments")
    if not isinstance(environments, dict):
        raise KeyError("JSON does not contain an 'environments' object")

    canonical_environment_name = None
    if environment_name in environments:
        environment = environments[environment_name]
        canonical_environment_name = environment_name
        if isinstance(environment, dict):
            stored_name = environment.get("name", {}).get("value")
            if isinstance(stored_name, str) and stored_name:
                canonical_environment_name = stored_name
    else:
        for environment in environments.values():
            if (
                isinstance(environment, dict)
                and environment.get("name", {}).get("value") == environment_name
            ):
                canonical_environment_name = environment_name
                break
    if canonical_environment_name is None:
        raise KeyError("Environment {!r} does not exist".format(environment_name))

    sensors = result.setdefault("sensors", {})
    if not isinstance(sensors, dict):
        raise TypeError("'sensors' must be a dictionary")
    sensor_index = 1
    while "sensor_{}".format(sensor_index) in sensors:
        sensor_index += 1
    sensor_name = "sensor_{}".format(sensor_index)

    driving_inputs = result.setdefault("driving_inputs", {})
    if not isinstance(driving_inputs, dict):
        raise TypeError("'driving_inputs' must be a dictionary")

    inputs = driving_inputs.setdefault(
        "inputs",
        {
            "message": "Driving input strength to Nervous System",
            "value": [],
        },
    )
    if not isinstance(inputs, dict):
        raise TypeError("'driving_inputs.inputs' must be a dictionary")
    if inputs.get("value") is None:
        inputs["value"] = []
    if not isinstance(inputs.get("value"), list):
        raise TypeError("'driving_inputs.inputs.value' must be a list")
    inputs.setdefault("message", "Driving input strength to Nervous System")

    weights = driving_inputs.setdefault(
        "weights",
        {
            "message": (
                "Weights of driving inputs to Nervous System in sparse format"
            ),
            "value": [],
        },
    )
    if not isinstance(weights, dict):
        raise TypeError("'driving_inputs.weights' must be a dictionary")
    if weights.get("value") is None:
        weights["value"] = []
    if not isinstance(weights.get("value"), list):
        raise TypeError("'driving_inputs.weights.value' must be a list")
    weights.setdefault(
        "message", "Weights of driving inputs to Nervous System in sparse format"
    )

    used_input_numbers = []
    for input_entry in inputs["value"]:
        if not isinstance(input_entry, dict):
            raise TypeError("Each driving input must be a dictionary")
        input_number = input_entry.get("input_num")
        if (
            isinstance(input_number, bool)
            or not isinstance(input_number, int)
            or input_number < 1
        ):
            raise ValueError("Driving input numbers must be positive integers")
        used_input_numbers.append(input_number)

    first_input_number = max(used_input_numbers, default=0) + 1
    second_input_number = first_input_number + 1
    for input_number in (first_input_number, second_input_number):
        inputs["value"].append(
            {
                "input_num": input_number,
                "strength": {"value": float(input_strength)},
            }
        )

    sensors[sensor_name] = {
        "environment": {"value": canonical_environment_name},
        "ext_inp_1": {"value": first_input_number - 1},
        "ext_inp_2": {"value": second_input_number - 1},
        "hs_stepsize": {"value": float(hs_stepsize)},
        "sensor_m": {"value": float(sensor_m)},
        "sensor_n": {"value": float(sensor_n)},
    }
    return result


def add_sensor_connection(
    json_data,
    sensor_name,
    ext_inp_name,
    cell_name,
    weight=1.0,
):
    """Return a copy with a sensor driving-input connection added."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(sensor_name, str) or not sensor_name:
        raise ValueError("sensor_name must be a non-empty string")
    if ext_inp_name not in {"ext_inp_1", "ext_inp_2"}:
        raise ValueError("ext_inp_name must be 'ext_inp_1' or 'ext_inp_2'")
    if not isinstance(cell_name, str) or not cell_name:
        raise ValueError("cell_name must be a non-empty string")
    if isinstance(weight, bool) or not isinstance(weight, (int, float)):
        raise TypeError("weight must be a number")

    result = copy.deepcopy(json_data)

    sensors = result.get("sensors")
    if not isinstance(sensors, dict):
        raise KeyError("JSON does not contain a 'sensors' object")
    if sensor_name not in sensors:
        raise KeyError("Sensor {!r} does not exist".format(sensor_name))
    sensor = sensors[sensor_name]
    if not isinstance(sensor, dict):
        raise TypeError("Sensor {!r} must be a dictionary".format(sensor_name))

    input_index = sensor.get(ext_inp_name, {}).get("value")
    if (
        isinstance(input_index, bool)
        or not isinstance(input_index, int)
        or input_index < 0
    ):
        raise ValueError(
            "{}.{} must contain a non-negative integer".format(
                sensor_name, ext_inp_name
            )
        )
    input_number = input_index + 1

    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    cells = nervous_system.get("cells")
    if not isinstance(cells, dict) or cell_name not in cells:
        raise KeyError(
            "Cell {!r} was not found in nervous_system.cells".format(cell_name)
        )

    driving_inputs = result.get("driving_inputs")
    if not isinstance(driving_inputs, dict):
        raise KeyError("JSON does not contain a 'driving_inputs' object")
    inputs = driving_inputs.get("inputs", {}).get("value")
    if not isinstance(inputs, list):
        raise TypeError("'driving_inputs.inputs.value' must be a list")
    if not any(
        isinstance(entry, dict) and entry.get("input_num") == input_number
        for entry in inputs
    ):
        raise KeyError("Driving input {} does not exist".format(input_number))

    weights = driving_inputs.setdefault(
        "weights",
        {
            "message": (
                "Weights of driving inputs to Nervous System in sparse format"
            ),
            "value": [],
        },
    )
    if not isinstance(weights, dict):
        raise TypeError("'driving_inputs.weights' must be a dictionary")
    if weights.get("value") is None:
        weights["value"] = []
    if not isinstance(weights.get("value"), list):
        raise TypeError("'driving_inputs.weights.value' must be a list")
    weights.setdefault(
        "message", "Weights of driving inputs to Nervous System in sparse format"
    )

    for connection in weights["value"]:
        if not isinstance(connection, dict):
            raise TypeError("Each driving input connection must be a dictionary")
        if (
            connection.get("from_input") == input_number
            and connection.get("to_cell") == cell_name
        ):
            raise ValueError(
                "A connection from {}.{} to {!r} already exists".format(
                    sensor_name, ext_inp_name, cell_name
                )
            )

    weights["value"].append(
        {
            "from_input": input_number,
            "to_cell": cell_name,
            "weight": {"value": float(weight)},
        }
    )
    return result


def add_random_cell_network(
    json_data,
    number_of_cells,
    connection_probability,
    random_seed=None,
):
    """Return a copy with an isolated random directed cell network added."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if (
        isinstance(number_of_cells, bool)
        or not isinstance(number_of_cells, int)
        or number_of_cells < 1
    ):
        raise ValueError("number_of_cells must be a positive integer")
    if (
        isinstance(connection_probability, bool)
        or not isinstance(connection_probability, (int, float))
        or not 0.0 <= connection_probability <= 1.0
    ):
        raise ValueError("connection_probability must be between 0 and 1")
    if random_seed is not None and (
        isinstance(random_seed, bool) or not isinstance(random_seed, int)
    ):
        raise TypeError("random_seed must be an integer or None")

    result = copy.deepcopy(json_data)
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")

    cells = nervous_system.setdefault("cells", {})
    if not isinstance(cells, dict):
        raise TypeError("'nervous_system.cells' must be a dictionary")

    cell_names_object = nervous_system.setdefault("cell_names", {"value": []})
    if not isinstance(cell_names_object, dict):
        raise TypeError("'nervous_system.cell_names' must be a dictionary")
    if cell_names_object.get("value") is None:
        cell_names_object["value"] = []
    cell_names = cell_names_object.get("value")
    if not isinstance(cell_names, list):
        raise TypeError("'nervous_system.cell_names.value' must be a list")

    no_suffix_object = nervous_system.setdefault(
        "cell_names_no_suffix", {"value": []}
    )
    if not isinstance(no_suffix_object, dict):
        raise TypeError(
            "'nervous_system.cell_names_no_suffix' must be a dictionary"
        )
    if no_suffix_object.get("value") is None:
        no_suffix_object["value"] = []
    no_suffix_names = no_suffix_object.get("value")
    if not isinstance(no_suffix_names, list):
        raise TypeError(
            "'nervous_system.cell_names_no_suffix.value' must be a list"
        )

    chemical_conns = nervous_system.setdefault(
        "chemical_conns", {"value": []}
    )
    if not isinstance(chemical_conns, dict):
        raise TypeError("'nervous_system.chemical_conns' must be a dictionary")
    if chemical_conns.get("value") is None:
        chemical_conns["value"] = []
    connections = chemical_conns.get("value")
    if not isinstance(connections, list):
        raise TypeError(
            "'nervous_system.chemical_conns.value' must be a list"
        )

    existing_names = set(cells) | set(cell_names)
    new_names = []
    name_index = 1
    while len(new_names) < number_of_cells:
        base_name = "RandomCell_{}".format(name_index)
        full_name = base_name + "_0"
        name_index += 1
        if full_name in existing_names:
            continue

        cells[full_name] = {
            "bias": {"value": 0.0},
            "cell_class": {"value": "interneuron"},
            "gain": {"value": 1.0},
            "state": {"value": 0.0},
            "tau": {"value": 1.0},
        }
        cell_names.append(full_name)
        no_suffix_names.append(base_name)
        existing_names.add(full_name)
        new_names.append(full_name)

    rng = random.Random(random_seed)
    for from_cell in new_names:
        for to_cell in new_names:
            if from_cell == to_cell:
                continue
            if rng.random() < connection_probability:
                connections.append(
                    {
                        "from": from_cell,
                        "to": to_cell,
                        "weight": {"value": rng.uniform(-1.0, 1.0)},
                    }
                )

    for section_name in ("worm", "Worm"):
        section = result.get(section_name)
        if not isinstance(section, dict):
            continue
        for size_key in ("N_size", "n_size"):
            size_object = section.get(size_key)
            if (
                isinstance(size_object, dict)
                and isinstance(size_object.get("value"), int)
                and not isinstance(size_object.get("value"), bool)
            ):
                size_object["value"] += number_of_cells

    return result


def add_cell_connection(
    json_data,
    from_cell,
    to_cell,
    weight=None,
    random_seed=None,
):
    """Return a copy with one directed chemical connection added if absent."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    for parameter_name, cell_name in (
        ("from_cell", from_cell),
        ("to_cell", to_cell),
    ):
        if not isinstance(cell_name, str) or not cell_name:
            raise ValueError("{} must be a non-empty string".format(parameter_name))
    if weight is not None and (
        isinstance(weight, bool) or not isinstance(weight, (int, float))
    ):
        raise TypeError("weight must be a number or None")
    if random_seed is not None and (
        isinstance(random_seed, bool) or not isinstance(random_seed, int)
    ):
        raise TypeError("random_seed must be an integer or None")

    result = copy.deepcopy(json_data)
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    cells = nervous_system.get("cells")
    if not isinstance(cells, dict):
        raise KeyError("JSON does not contain 'nervous_system.cells'")
    for cell_name in (from_cell, to_cell):
        if cell_name not in cells:
            raise KeyError(
                "Cell {!r} was not found in nervous_system.cells".format(
                    cell_name
                )
            )

    chemical_conns = nervous_system.setdefault(
        "chemical_conns", {"value": []}
    )
    if not isinstance(chemical_conns, dict):
        raise TypeError("'nervous_system.chemical_conns' must be a dictionary")
    if chemical_conns.get("value") is None:
        chemical_conns["value"] = []
    connections = chemical_conns.get("value")
    if not isinstance(connections, list):
        raise TypeError(
            "'nervous_system.chemical_conns.value' must be a list"
        )

    for connection in connections:
        if not isinstance(connection, dict):
            raise TypeError("Each chemical connection must be a dictionary")
        if (
            connection.get("from") == from_cell
            and connection.get("to") == to_cell
        ):
            return result

    if weight is None:
        weight = random.Random(random_seed).uniform(-1.0, 1.0)
    connections.append(
        {
            "from": from_cell,
            "to": to_cell,
            "weight": {"value": float(weight)},
        }
    )
    return result


def delete_cell_connection(json_data, from_cell, to_cell):
    """Return a copy with one directed chemical connection removed if present."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    for parameter_name, cell_name in (
        ("from_cell", from_cell),
        ("to_cell", to_cell),
    ):
        if not isinstance(cell_name, str) or not cell_name:
            raise ValueError("{} must be a non-empty string".format(parameter_name))

    result = copy.deepcopy(json_data)
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    cells = nervous_system.get("cells")
    if not isinstance(cells, dict):
        raise KeyError("JSON does not contain 'nervous_system.cells'")
    for cell_name in (from_cell, to_cell):
        if cell_name not in cells:
            raise KeyError(
                "Cell {!r} was not found in nervous_system.cells".format(
                    cell_name
                )
            )

    chemical_conns = nervous_system.get("chemical_conns")
    if not isinstance(chemical_conns, dict):
        raise KeyError(
            "JSON does not contain 'nervous_system.chemical_conns'"
        )
    connections = chemical_conns.get("value")
    if connections is None:
        return result
    if not isinstance(connections, list):
        raise TypeError(
            "'nervous_system.chemical_conns.value' must be a list"
        )

    removed_evotags = set()
    remaining_connections = []
    for connection in connections:
        if not isinstance(connection, dict):
            raise TypeError("Each chemical connection must be a dictionary")
        if (
            connection.get("from") == from_cell
            and connection.get("to") == to_cell
        ):
            evotag = connection.get("weight", {}).get("evotag")
            if isinstance(evotag, (str, int)) and not isinstance(evotag, bool):
                removed_evotags.add(evotag)
            continue
        remaining_connections.append(connection)
    chemical_conns["value"] = remaining_connections

    if not removed_evotags:
        return result

    def collect_remaining_evotags(value, at_root=False, output=None):
        if output is None:
            output = set()
        if isinstance(value, dict):
            evotag = value.get("evotag")
            if isinstance(evotag, (str, int)) and not isinstance(evotag, bool):
                output.add(evotag)
            for key, child in value.items():
                if at_root and key in {
                    "evolvable_ranges",
                    "evolved_used",
                    "Evolvable",
                }:
                    continue
                collect_remaining_evotags(child, output=output)
        elif isinstance(value, list):
            for child in value:
                collect_remaining_evotags(child, output=output)
        return output

    unused_evotags = removed_evotags - collect_remaining_evotags(
        result, at_root=True
    )
    ranges = result.get("evolvable_ranges")
    if isinstance(ranges, dict):
        for evotag in unused_evotags:
            ranges.pop(str(evotag), None)

    evolved_used = result.get("evolved_used")
    if isinstance(evolved_used, dict) and isinstance(
        evolved_used.get("value"), list
    ):
        evolved_used["value"] = [
            evotag
            for evotag in evolved_used["value"]
            if evotag not in unused_evotags
        ]
    return result


def delete_sensor(json_data, sensor_name):
    """Return a copy with a sensor and its dedicated driving inputs removed."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(sensor_name, str) or not sensor_name:
        raise ValueError("sensor_name must be a non-empty string")

    result = copy.deepcopy(json_data)
    sensors = result.get("sensors")
    if not isinstance(sensors, dict):
        raise KeyError("JSON does not contain a 'sensors' object")
    if sensor_name not in sensors:
        raise KeyError("Sensor {!r} does not exist".format(sensor_name))

    sensor = sensors[sensor_name]
    if not isinstance(sensor, dict):
        raise TypeError("Sensor {!r} must be a dictionary".format(sensor_name))

    removed_evotags = set()

    def collect_evotags(value):
        if isinstance(value, dict):
            evotag = value.get("evotag")
            if isinstance(evotag, (str, int)) and not isinstance(evotag, bool):
                removed_evotags.add(evotag)
            for child in value.values():
                collect_evotags(child)
        elif isinstance(value, list):
            for child in value:
                collect_evotags(child)

    removed_input_numbers = set()
    for key in ("ext_inp_1", "ext_inp_2"):
        value = sensor.get(key, {}).get("value")
        if isinstance(value, bool) or not isinstance(value, int) or value < 0:
            raise ValueError(
                "{}.{} must contain a non-negative integer".format(
                    sensor_name, key
                )
            )
        removed_input_numbers.add(value + 1)

    for other_name, other_sensor in sensors.items():
        if other_name == sensor_name or not isinstance(other_sensor, dict):
            continue
        for key in ("ext_inp_1", "ext_inp_2"):
            value = other_sensor.get(key, {}).get("value")
            if isinstance(value, int) and not isinstance(value, bool):
                if value + 1 in removed_input_numbers:
                    raise ValueError(
                        "Driving input {} is also used by sensor {!r}".format(
                            value + 1, other_name
                        )
                    )

    collect_evotags(sensor)
    del sensors[sensor_name]

    driving_inputs = result.get("driving_inputs")
    if not isinstance(driving_inputs, dict):
        raise KeyError("JSON does not contain a 'driving_inputs' object")

    inputs = driving_inputs.get("inputs", {}).get("value")
    if not isinstance(inputs, list):
        raise TypeError("'driving_inputs.inputs.value' must be a list")

    remaining_inputs = []
    for entry in inputs:
        if not isinstance(entry, dict):
            raise TypeError("Each driving input must be a dictionary")
        input_number = entry.get("input_num")
        if (
            isinstance(input_number, bool)
            or not isinstance(input_number, int)
            or input_number < 1
        ):
            raise ValueError("Driving input numbers must be positive integers")
        if input_number not in removed_input_numbers:
            remaining_inputs.append(entry)

    remaining_inputs.sort(key=lambda entry: entry["input_num"])
    input_number_map = {
        entry["input_num"]: new_number
        for new_number, entry in enumerate(remaining_inputs, start=1)
    }
    for entry in remaining_inputs:
        entry["input_num"] = input_number_map[entry["input_num"]]
    driving_inputs["inputs"]["value"] = remaining_inputs

    weights = driving_inputs.get("weights", {}).get("value")
    if weights is None:
        weights = []
    if not isinstance(weights, list):
        raise TypeError("'driving_inputs.weights.value' must be a list")

    remaining_weights = []
    for connection in weights:
        if not isinstance(connection, dict):
            raise TypeError("Each driving input connection must be a dictionary")
        input_number = connection.get("from_input")
        if input_number in removed_input_numbers:
            collect_evotags(connection)
            continue
        if input_number not in input_number_map:
            raise ValueError(
                "Connection refers to missing driving input {}".format(
                    input_number
                )
            )
        connection["from_input"] = input_number_map[input_number]
        remaining_weights.append(connection)
    driving_inputs.setdefault("weights", {})["value"] = remaining_weights

    for remaining_sensor in sensors.values():
        if not isinstance(remaining_sensor, dict):
            raise TypeError("Each sensor must be a dictionary")
        for key in ("ext_inp_1", "ext_inp_2"):
            old_index = remaining_sensor.get(key, {}).get("value")
            old_input_number = old_index + 1
            if old_input_number not in input_number_map:
                raise ValueError(
                    "Sensor refers to missing driving input {}".format(
                        old_input_number
                    )
                )
            remaining_sensor[key]["value"] = (
                input_number_map[old_input_number] - 1
            )

    def sensor_number(name):
        prefix = "sensor_"
        if not name.startswith(prefix) or not name[len(prefix):].isdigit():
            raise ValueError(
                "Sensor names must use the form 'sensor_N': {!r}".format(name)
            )
        return int(name[len(prefix):])

    ordered_sensors = sorted(sensors.items(), key=lambda item: sensor_number(item[0]))
    sensors.clear()
    for index, (_, remaining_sensor) in enumerate(ordered_sensors, start=1):
        sensors["sensor_{}".format(index)] = remaining_sensor
    if not sensors:
        result.pop("sensors", None)

    def collect_remaining_evotags(value, at_root=False, output=None):
        if output is None:
            output = set()
        if isinstance(value, dict):
            evotag = value.get("evotag")
            if isinstance(evotag, (str, int)) and not isinstance(evotag, bool):
                output.add(evotag)
            for key, child in value.items():
                if at_root and key in {
                    "evolvable_ranges",
                    "evolved_used",
                    "Evolvable",
                }:
                    continue
                collect_remaining_evotags(child, output=output)
        elif isinstance(value, list):
            for child in value:
                collect_remaining_evotags(child, output=output)
        return output

    unused_evotags = removed_evotags - collect_remaining_evotags(
        result, at_root=True
    )
    ranges = result.get("evolvable_ranges")
    if isinstance(ranges, dict):
        for evotag in unused_evotags:
            ranges.pop(str(evotag), None)

    evolved_used = result.get("evolved_used")
    if isinstance(evolved_used, dict) and isinstance(
        evolved_used.get("value"), list
    ):
        evolved_used["value"] = [
            evotag
            for evotag in evolved_used["value"]
            if evotag not in unused_evotags
        ]

    if not remaining_inputs and not remaining_weights:
        result.pop("driving_inputs", None)
    return result


def remove_nervous_system_cell(json_data, cell_name):
    """Return a copy of a modern worm JSON dictionary without one NS cell."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(cell_name, str) or not cell_name:
        raise ValueError("cell_name must be a non-empty string")

    result = copy.deepcopy(json_data)
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")

    cells = nervous_system.get("cells")
    if not isinstance(cells, dict) or cell_name not in cells:
        raise KeyError(
            "Cell {!r} was not found in nervous_system.cells".format(cell_name)
        )

    removed_evotags = set()

    def collect_evotags(value):
        if isinstance(value, dict):
            evotag = value.get("evotag")
            if isinstance(evotag, (str, int)) and not isinstance(evotag, bool):
                removed_evotags.add(evotag)
            for child in value.values():
                collect_evotags(child)
        elif isinstance(value, list):
            for child in value:
                collect_evotags(child)

    def references_cell(value):
        if not isinstance(value, dict):
            return False
        for key, child in value.items():
            key_lower = str(key).lower()
            if (
                child == cell_name
                and (
                    key_lower
                    in {
                        "from",
                        "to",
                        "from_cell",
                        "to_cell",
                        "cell",
                        "cell_name",
                    }
                    or key_lower.endswith("_cell")
                    or key_lower.endswith("_cell_name")
                )
            ):
                return True
        return False

    collect_evotags(cells[cell_name])
    del cells[cell_name]

    cell_names = nervous_system.get("cell_names", {}).get("value")
    removed_index = None
    if isinstance(cell_names, list) and cell_name in cell_names:
        removed_index = cell_names.index(cell_name)
        cell_names.pop(removed_index)

    no_suffix = nervous_system.get("cell_names_no_suffix", {}).get("value")
    if (
        removed_index is not None
        and isinstance(no_suffix, list)
        and removed_index < len(no_suffix)
    ):
        no_suffix.pop(removed_index)
    elif isinstance(no_suffix, list):
        base_name = cell_name.rsplit("_", 1)[0]
        if base_name in no_suffix:
            no_suffix.remove(base_name)

    evotag_registry_keys = {"evolvable_ranges", "evolved_used", "Evolvable"}

    def remove_references(value, at_root=False):
        if isinstance(value, dict):
            for key in list(value):
                child = value[key]
                if at_root and key in evotag_registry_keys:
                    continue
                if key == cell_name:
                    collect_evotags(child)
                    del value[key]
                    continue
                remove_references(child)
        elif isinstance(value, list):
            kept = []
            for child in value:
                remove = child == cell_name or references_cell(child)
                if remove:
                    collect_evotags(child)
                else:
                    remove_references(child)
                    kept.append(child)
            value[:] = kept

    remove_references(result, at_root=True)

    # The modern format stores the nervous-system size in the worm object.
    for section_name in ("worm", "Worm"):
        section = result.get(section_name)
        if not isinstance(section, dict):
            continue
        for size_key in ("N_size", "n_size"):
            size_obj = section.get(size_key)
            if (
                isinstance(size_obj, dict)
                and isinstance(size_obj.get("value"), int)
                and size_obj["value"] > 0
            ):
                size_obj["value"] -= 1

    def collect_remaining_evotags(value, at_root=False, output=None):
        if output is None:
            output = set()
        if isinstance(value, dict):
            evotag = value.get("evotag")
            if isinstance(evotag, (str, int)) and not isinstance(evotag, bool):
                output.add(evotag)
            for key, child in value.items():
                if at_root and key in evotag_registry_keys:
                    continue
                collect_remaining_evotags(child, output=output)
        elif isinstance(value, list):
            for child in value:
                collect_remaining_evotags(child, output=output)
        return output

    remaining_evotags = collect_remaining_evotags(result, at_root=True)
    unused_evotags = removed_evotags - remaining_evotags

    ranges = result.get("evolvable_ranges")
    if isinstance(ranges, dict):
        for evotag in unused_evotags:
            ranges.pop(str(evotag), None)

        entries = ranges.get("value")
        if isinstance(entries, list):
            kept = []
            for entry in entries:
                entry_tag = None
                if isinstance(entry, dict):
                    if "evotag" in entry:
                        entry_tag = entry["evotag"]
                    elif len(entry) == 1:
                        entry_tag = next(iter(entry))
                if entry_tag not in unused_evotags:
                    kept.append(entry)
            entries[:] = kept

    evolved_used = result.get("evolved_used")
    if isinstance(evolved_used, dict):
        used_values = evolved_used.get("value")
    else:
        used_values = evolved_used
    if isinstance(used_values, list):
        used_values[:] = [tag for tag in used_values if tag not in unused_evotags]

    legacy_ranges = result.get("Evolvable")
    if isinstance(legacy_ranges, dict) and isinstance(legacy_ranges.get("value"), list):
        legacy_ranges["value"][:] = [
            entry
            for entry in legacy_ranges["value"]
            if not isinstance(entry, dict) or entry.get("evotag") not in unused_evotags
        ]

    return result


def delete_directory(directory_path):
    """Recursively delete a directory, returning True if it existed."""
    path = os.path.abspath(directory_path)
    if path in (os.path.abspath(os.curdir), os.path.abspath(os.sep)):
        raise ValueError("Refusing to delete the current directory or filesystem root")
    if not os.path.isdir(path):
        return False
    shutil.rmtree(path)
    return True


def delete_notebook_directory(subfolder_name, subsubfolder_name):
    """Delete a direct child directory from a subfolder of the current directory."""
    for name in (subfolder_name, subsubfolder_name):
        if (
            not isinstance(name, str)
            or name in ("", ".", "..")
            or os.path.isabs(name)
            or os.path.basename(name) != name
            or os.sep in name
            or (os.altsep is not None and os.altsep in name)
        ):
            raise ValueError("Arguments must be single folder names, not paths")

    parent_path = os.path.abspath(os.path.join(os.curdir, subfolder_name))
    if not os.path.isdir(parent_path):
        return False
    if os.path.islink(parent_path):
        raise ValueError("Refusing to use a symbolic link as the parent folder")

    target_path = os.path.abspath(os.path.join(parent_path, subsubfolder_name))
    if os.path.dirname(target_path) != parent_path:
        raise ValueError("Target must be a direct child of the parent folder")
    if not os.path.isdir(target_path):
        return False
    if os.path.islink(target_path):
        raise ValueError("Refusing to delete a symbolic link")

    shutil.rmtree(target_path)
    return True


def checkDictName(dictval, namelist):
    dictval1 = dictval
    for val in namelist:
        # print(val)
        # if isinstance(val, int):
        if not isinstance(val, int) and val not in dictval1:
            return False
        dictval1 = dictval1[val]
    return True


def process_args():
    """Parse command-line arguments.

    :returns: None
    """
    parser = argparse.ArgumentParser(
        description=("A script for supplying arguments to execute Worm2D")
    )

    parser.add_argument(
        "-m",
        "--modelName",
        type=str,
        metavar="<model name>",
        default=DEFAULTS["modelName"],
        help=(
            "Name of model is required.\nOptions include: RS18, CE, Net21, CO"
            # "Default is: %s" % DEFAULTS["modelName"]
        ),
    )

    parser.add_argument(
        "-s",
        "--showPlot",
        action="store_true",
        # metavar="<run NML>",
        default=DEFAULTS["showPlot"],
        help=("Show plot."),
    )

    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        # metavar="<run NML>",
        default=DEFAULTS["verbose"],
        help=("Verbose."),
    )

    parser.add_argument(
        "-f",
        "--folderName",
        type=str,
        metavar="<folder name>",
        default=DEFAULTS["folderName"],
        help=("Required name of data folder."),
    )


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


def setFolder(a):
    if a.modelName is None:
        print("plot_format is required to make figure.")
        return

    if a.folderName is None:
        print("Folder name is required for data.")
        return

    global dir_name, file_prefix
    dir_name = a.folderName
    file_prefix = a.modelName + "_"
    # print(dir_name,   file_prefix)


def rename_file(file_name):
    if dir_name is None:
        if file_prefix is None:
            return file_name
        return file_prefix + file_name
    if file_prefix is None:
        return dir_name + "/" + file_name
    return dir_name + "/" + file_prefix + file_name


def get_path_list(outFolderBases):
    path_list = []
    # outFolderBases = ["varyEvolSeeds", "varyEvolSeeds1", "varyEvolSeeds2", "varyEvolSeeds3"]
    # outFolderBases = ["varyEvolSeedsNet21_4"]
    # outFolderBases = ["izq_runs_nets"]
    current = os.path.dirname(os.path.realpath(__file__))  # location of this file!
    for outFolderBase in outFolderBases:
        path = current + "/" + outFolderBase
        dir_list = sorted([x[0] for x in os.walk(path)])
        # dir_list = sorted(os.listdir(path))
        # dirs = [dir for dir in dir_list if os.path.isdir(dir)]
        # path_list += [path + "/" + dir for dir in dir_list]
        path_list += dir_list[1:]
    return path_list


def make_directory(directory_name, overwrite=False, str1="the contents"):
    try:
        os.mkdir(directory_name)
        print(f"Directory '{directory_name}' created successfully.")
        return True
    except FileExistsError:
        if overwrite:
            print(
                f"Directory '{directory_name}' already exists and "
                + str1
                + " will be overwritten."
            )
            return True
        else:
            print(
                f"Directory '{directory_name}' already exists and overwrite is false."
            )
            return False
    except PermissionError:
        print(f"Permission denied: Unable to create '{directory_name}'.")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)


def make_orients(body_data, **kwargs):
    tmax = body_data.shape[1]
    t_start = 0
    if "t_start_off" in kwargs:
        t_start = kwargs["t_start_off"]

    t_end = tmax
    if "t_end_off" in kwargs:
        t_end = tmax - kwargs["t_end_off"]

    trange = range(t_start, t_end)
    body_data_res = body_data[:, trange]

    w_head = 0
    # w_tail = 50
    body_diff = np.diff(body_data_res, axis=1)
    # print(body_diff.shape)
    trajectory = np.arctan2(body_diff[w_head * 3 + 2], body_diff[w_head * 3 + 1])
    body_data_res_mid = (body_data_res[:, 1:] + body_data_res[:, :-1]) / 2.0
    dir_to_origin_mid = np.arctan2(
        body_data_res_mid[w_head * 3 + 2] * -1, body_data_res_mid[w_head * 3 + 1] * -1
    )

    trajectory_diff_u = angle_diff(trajectory[1:], trajectory[:-1])
    # trajectory_diff = np.diff(trajectory)
    # trajectory_diff_u =  np.unwrap(trajectory_diff)
    bearing_mid = angle_diff(trajectory, dir_to_origin_mid)

    # bearing_mid = np.unwrap(trajectory - dir_to_origin_mid)

    return bearing_mid, trajectory_diff_u


def movingaverage(interval, window_size):
    window = np.ones(int(window_size)) / float(window_size)
    return np.convolve(interval, window, "same")


def plot_path(body_data, ax):
    tmax = body_data.shape[1]
    num = 60.0
    point_start = 0
    point_end = 50
    markersize = 3
    markersize_small = 0.4

    for t in range(1, tmax, int(tmax / num)):
        f = float(t) / tmax

        color = "#%02x%02x00" % (int(0xFF * (f)), int(0xFF * (1 - f) * 0.8))
        # color2 = "#%06x" % random.randint(0, 0xFFFFFF)
        for i in range(point_start, point_end):
            x = body_data[i * 3 + 1][t]
            y = body_data[i * 3 + 2][t]

            ax.plot(
                x,
                y,
                ".",
                color=color,
                markersize=markersize if t == 1 else markersize_small,
            )


def plot_orients(
    body_data,
    plot_list=[
        "body orientation",
        "direction to peak",
        "distance to peak",
        "bearing from peak direction",
    ],
):
    num_cols = 2
    num_rows = math.ceil(len(plot_list) / num_cols)
    fig_orient, ax_orient = plt.subplots(
        num_rows, num_cols, figsize=(num_cols * 4, num_rows * 4)
    )

    tmax = body_data.shape[1]
    trange = body_data[0, :]
    t_offset = 0
    t_start = t_offset
    t_end = tmax - t_offset
    trange_inds = (trange >= t_start) & (trange < t_end)
    trange = trange[trange_inds]
    body_data_res = body_data[:, trange_inds]

    w_head = 0
    w_tail = 50

    body_diff = np.diff(body_data_res, axis=1)

    trajectory = np.arctan2(body_diff[w_head * 3 + 2], body_diff[w_head * 3 + 1])

    body_data_res_mid = (body_data_res[:, 1:] + body_data_res[:, :-1]) / 2.0

    dir_to_origin_mid = np.arctan2(
        body_data_res_mid[w_head * 3 + 2] * -1, body_data_res_mid[w_head * 3 + 1] * -1
    )

    dir_to_origin = np.arctan2(
        body_data_res[w_head * 3 + 2] * -1, body_data_res[w_head * 3 + 1] * -1
    )

    trajectory_diff_u = angle_diff(trajectory[1:], trajectory[:-1])

    bearing_mid = angle_diff(trajectory, dir_to_origin_mid)

    orientation = np.arctan2(
        body_data_res[w_head * 3 + 2] - body_data_res[w_tail * 3 + 2],
        body_data_res[w_head * 3 + 1] - body_data_res[w_tail * 3 + 1],
    )

    dOrientation = angle_diff(orientation[1:], orientation[:-1])

    distToOrigin = np.sqrt(
        np.multiply(body_data_res[w_head * 3 + 2], body_data_res[w_head * 3 + 2])
        + np.multiply(body_data_res[w_head * 3 + 1], body_data_res[w_head * 3 + 1])
    )

    bearing = angle_diff(trajectory, dir_to_origin[1:])

    plottables = {
        "bearing from peak direction": bearing_mid,
        "distance to peak": distToOrigin,
        "orientation variation": dOrientation,
        "body orientation": orientation,
        "direction to peak": dir_to_origin,
        "head trajectory variation": trajectory_diff_u,
        "head trajectory": trajectory,
    }

    for key, val in plottables.items():
        newval = {}
        newval["value"] = val
        if key == "distance to peak":
            newval["y_label"] = "distance (cm)"
        else:
            newval["y_label"] = "angle (rad)"
        plottables[key] = newval

    # print(plottables)
    sys.exit

    mark_size = 0.2
    tav_window = 1
    plot_func = partial(movingaverage, window_size=tav_window)

    for ind, val in enumerate(plot_list):
        col_num = ind % 2
        row_num = math.floor(ind / 2)
        r_diff = len(trange) - len(plottables[val]["value"])
        t_start_ind = 0
        t_end_ind = len(trange)
        if r_diff > 0:
            t_end_ind = -1
        if r_diff > 1:
            t_start_ind = 1
        # print(val, t_start_ind, t_end_ind, r_diff)
        ax_orient[row_num, col_num].plot(
            plot_func(trange[t_start_ind:t_end_ind]),
            plot_func(plottables[val]["value"]),
            "o",
            markersize=mark_size,
        )
        ax_orient[row_num, col_num].set_title(val, fontsize=title_font_size)
        ax_orient[row_num, col_num].set_ylabel(
            plottables[val]["y_label"], fontsize=label_font_size
        )
        if row_num == num_rows - 1:
            ax_orient[row_num, col_num].set_xlabel("Time (s)", fontsize=label_font_size)

    if False:
        # trange_av = movingaverage(trange, tav_window)
        ax_orient[0, 0].plot(
            trange, orientation, "o", markersize=mark_size
        )  # body orientation
        ax_orient[0, 0].set_title("body orientation", fontsize=title_font_size)
        ax_orient[1, 0].plot(trange, distToOrigin, "o", markersize=mark_size)
        ax_orient[1, 0].set_title("distance to peak", fontsize=title_font_size)
        ax_orient[2, 0].plot(trange, dir_to_origin, "o", markersize=mark_size)
        ax_orient[2, 0].set_title("direction to origin", fontsize=title_font_size)
        ax_orient[3, 0].plot(
            movingaverage(trange[:-1], tav_window),
            movingaverage(dOrientation, tav_window),
            "o",
            markersize=mark_size,
        )
        ax_orient[3, 0].set_title("orientation variation", fontsize=title_font_size)
        ax_orient[4, 0].plot(
            movingaverage(trange[1:-1], tav_window),
            movingaverage(trajectory_diff_u, tav_window),
            "o",
            markersize=mark_size,
        )
        ax_orient[4, 0].set_title("head trajectory variation", fontsize=title_font_size)

        ax_orient[0, 1].plot(
            movingaverage(trange[:-1], tav_window),
            movingaverage(bearing_mid, tav_window),
            "o",
            markersize=mark_size,
        )
        ax_orient[0, 1].set_title(
            "bearing from origin direction", fontsize=title_font_size
        )
        ax_orient[1, 1].plot(
            movingaverage(trange[:-1], tav_window),
            movingaverage(trajectory, tav_window),
            "o",
            markersize=mark_size,
        )
        ax_orient[1, 1].set_title("head trajectory", fontsize=title_font_size)

        # ax_orient[4,0].plot(trange[1:-1], trajectory_diff_1)
        ax_orient[2, 1].scatter(bearing[:-1], trajectory_diff_u * 10, s=mark_size)

        # heatmap, xedges, yedges = np.histogram2d(bearing[:-1], trajectory_diff_u*10.0, bins=50)
        # extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
        # ax_orient[1,1].imshow(heatmap.T, extent=extent, origin='lower')

        ax_orient[3, 1].scatter(bearing_mid[:-1], trajectory_diff_u * 10, s=mark_size)

        heatmap, xedges, yedges = np.histogram2d(
            bearing_mid[:-1], trajectory_diff_u * 10.0, bins=50
        )
        extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
        ax_orient[4, 1].imshow(heatmap.T, extent=extent, origin="lower")

        # ax_orient[1,1].scatter(dir_to_origin[:-1], dOrientation, s=mark_size)

        # heatmap, xedges, yedges = np.histogram2d(bearing_mid[:-1], trajectory_diff_u*10.0, bins=50)
        # extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]

        # plt.clf()

        # ax_orient[3,1].scatter(dir_to_origin[1:-1], trajectory_diff, s=mark_size)
        # ax_orient[4,1].scatter(dir_to_origin[1:-1], trajectory_diff_1, s=mark_size)

    fig_orient.tight_layout()
    filename = rename_file("Orient.png")
    # fig_orient.show()
    fig_orient.savefig(filename, bbox_inches="tight", dpi=300)
    plt.close(fig_orient)
    # fig_orient.close()


def angle_diff(a, b):
    """Return the signed smallest difference between two angles (in radians)."""
    d = (a - b + math.pi) % (2 * math.pi) - math.pi
    # Optional: map -pi to +pi for symmetry
    # if d == -math.pi:
    #    return math.pi
    return d


def plotHist(ax, x, y):
    if binned_statistic is None:
        raise ImportError("scipy is required to plot binned histogram statistics")

    bins = 40
    # mean
    # y_mean, bin_edges, _ = binned_statistic(x, y, statistic='mean', bins=bins)
    # standard deviation
    # y_std, _, _ = binned_statistic(x, y, statistic='std', bins=bins)

    mean_stats = binned_statistic(x, y, statistic="mean", bins=bins)
    bin_means = mean_stats.statistic

    bin_edges = mean_stats.bin_edges
    bin_centers = 0.5 * (bin_edges[1:] + bin_edges[:-1])

    count_stats = binned_statistic(x, y, statistic="count", bins=bins)
    bin_counts = count_stats.statistic

    std_stats = binned_statistic(x, y, statistic="std", bins=bins)
    bin_stds = std_stats.statistic

    # Calculate the Standard Error of the Mean (SEM) for each bin: SEM = SD / sqrt(count)
    bin_sems = bin_stds / np.sqrt(bin_counts)

    # bin_centers = 0.5 * (bin_edges[1:] + bin_edges[:-1])

    ax.errorbar(bin_centers, bin_means, yerr=bin_sems, fmt="o")
    # ax.xlabel('x')
    # ax.ylabel('Average y')
    # plt.show()


def load_nonragged_arrays(filename, dtype=float, delimiter=None, skip_empty=True):
    arrays = []
    current_rows = []
    current_len = None

    with open(filename, "r") as f:
        for line_num, line in enumerate(f, start=1):
            line = line.strip()

            if skip_empty and not line:
                continue

            parts = line.split(delimiter) if delimiter is not None else line.split()
            row = [dtype(x) for x in parts]
            row_len = len(row)

            if current_len is None:
                current_len = row_len
                current_rows.append(row)
            elif row_len == current_len:
                current_rows.append(row)
            else:
                arrays.append(np.array(current_rows, dtype=dtype))
                current_rows = [row]
                current_len = row_len

    if current_rows:
        arrays.append(np.array(current_rows, dtype=dtype))

    return arrays


def clean_ragged_numeric_file(input_path, output_path=None):
    """
    Read a ragged numeric text file, replace NaN/Inf/-Inf with 0,
    and save it preserving the original row structure.

    Assumes each row contains whitespace-separated numeric values.
    Blank lines are preserved.
    """
    if output_path is None:
        output_path = input_path

    cleaned_lines = []

    with open(input_path, "r") as f:
        for line_num, line in enumerate(f, start=1):
            stripped = line.strip()

            # Preserve blank lines
            if not stripped:
                cleaned_lines.append("\n")
                continue

            parts = stripped.split()
            cleaned_parts = []

            for col_num, part in enumerate(parts, start=1):
                try:
                    x = float(part)
                    if math.isnan(x) or math.isinf(x):
                        x = 0.0
                    cleaned_parts.append(str(x))
                except ValueError:
                    raise ValueError(
                        f"Non-numeric value {part!r} at line {line_num}, column {col_num}"
                    )

            cleaned_lines.append(" ".join(cleaned_parts) + "\n")

    with open(output_path, "w") as f:
        f.writelines(cleaned_lines)
