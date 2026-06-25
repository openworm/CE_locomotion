import argparse
import copy
import html
import json
import math
import os
import random
import shutil
import sys
from collections import Counter
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


def get_worm2d_version():
    from . import __version__
    return __version__


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
                and environment.get("name", {}).get("value") == environment_name
            ):
                environment_key = key
                canonical_name = environment_name
                break
    if environment_key is None:
        raise KeyError("Environment {!r} does not exist".format(environment_name))

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
    name=None,
    sensor_n=2.0,
    sensor_m=2.0,
):
    """Return a copy of a worm JSON dictionary plus the new sensor name."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(environment_name, str) or not environment_name:
        raise ValueError("environment_name must be a non-empty string")
    if name is not None and (not isinstance(name, str) or not name.strip()):
        raise ValueError("name must be a non-empty string")

    for parameter_name, value in (
        ("sensor_n", sensor_n),
        ("sensor_m", sensor_m),
    ):
        if isinstance(value, bool) or not isinstance(value, (int, float)):
            raise TypeError("{} must be a number".format(parameter_name))
    if sensor_n <= 0 or sensor_m <= 0:
        raise ValueError("sensor_n and sensor_m must be positive")

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
    if name is None:
        sensor_index = 1
        while "sensor_{}".format(sensor_index) in sensors:
            sensor_index += 1
        sensor_name = "sensor_{}".format(sensor_index)
    else:
        sensor_name = name.strip()
        if sensor_name in sensors:
            raise ValueError("Sensor {!r} already exists".format(sensor_name))

    sensors[sensor_name] = {
        "environment": {"value": canonical_environment_name},
        "sensor_m": {"value": float(sensor_m)},
        "sensor_n": {"value": float(sensor_n)},
        "outputs": {
            "message": ("Available sensor output names for sensor-to-cell connections"),
            "value": [
                {
                    "name": "output_1",
                    "description": (
                        "Positive change in sensed concentration "
                        "(present average above past average)"
                    ),
                },
                {
                    "name": "output_2",
                    "description": (
                        "Negative change in sensed concentration "
                        "(past average above present average)"
                    ),
                },
            ],
        },
        "weights": {
            "message": "Weights from sensor outputs to Nervous System cells",
            "value": [],
        },
    }
    return result, sensor_name


def add_sensor_connection(
    json_data,
    sensor_name,
    output_name,
    cell_name,
    weight=1.0,
    make_evolvable=False,
    evotag_name=None,
):
    """Return a copy with a sensor-output connection added if absent."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(sensor_name, str) or not sensor_name:
        raise ValueError("sensor_name must be a non-empty string")
    output_numbers = {
        "output_1": 1,
        "output_2": 2,
        "ext_inp_1": 1,
        "ext_inp_2": 2,
    }
    if output_name not in output_numbers:
        raise ValueError("output_name must be 'output_1' or 'output_2'")
    output_number = output_numbers[output_name]
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

    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    cells = nervous_system.get("cells")
    if not isinstance(cells, dict) or cell_name not in cells:
        raise KeyError(
            "Cell {!r} was not found in nervous_system.cells".format(cell_name)
        )

    weights = sensor.setdefault(
        "weights",
        {
            "message": "Weights from sensor outputs to Nervous System cells",
            "value": [],
        },
    )
    if not isinstance(weights, dict):
        raise TypeError("Sensor 'weights' must be a dictionary")
    if weights.get("value") is None:
        weights["value"] = []
    if not isinstance(weights.get("value"), list):
        raise TypeError("Sensor 'weights.value' must be a list")
    weights.setdefault("message", "Weights from sensor outputs to Nervous System cells")

    for connection in weights["value"]:
        if not isinstance(connection, dict):
            raise TypeError("Each driving input connection must be a dictionary")
        if (
            connection.get("from_output") == output_number
            and connection.get("to_cell") == cell_name
        ):
            return result

    weights["value"].append(
        {
            "from_output": output_number,
            "to_cell": cell_name,
            "weight": {"value": float(weight)},
        }
    )
    if make_evolvable:
        result = add_evotag(
            result,
            [
                "sensors",
                sensor_name,
                "weights",
                "value",
                len(weights["value"]) - 1,
                "weight",
            ],
            evotag_name,
        )
    return result


def add_cell_muscle_connection(
    json_data,
    cell_name,
    muscle_number,
    muscle_side,
    weight=1.0,
    make_evolvable=False,
    evotag_name=None,
):
    """Return a copy with a cell-to-muscle connection added if absent."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(cell_name, str) or not cell_name:
        raise ValueError("cell_name must be a non-empty string")
    if isinstance(muscle_number, bool) or not isinstance(muscle_number, int):
        raise TypeError("muscle_number must be an integer")
    if muscle_number < 1:
        raise ValueError("muscle_number must be at least 1")
    if not isinstance(muscle_side, str):
        raise TypeError("muscle_side must be a string")
    muscle_side = muscle_side.lower()
    if muscle_side not in {"dorsal", "ventral"}:
        raise ValueError("muscle_side must be 'dorsal' or 'ventral'")
    if isinstance(weight, bool) or not isinstance(weight, (int, float)):
        raise TypeError("weight must be a number")
    if not isinstance(make_evolvable, bool):
        raise TypeError("make_evolvable must be a boolean")
    if evotag_name is not None and (
        not isinstance(evotag_name, str) or not evotag_name
    ):
        raise ValueError("evotag_name must be a non-empty string or None")

    result = copy.deepcopy(json_data)
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    cells = nervous_system.get("cells")
    if not isinstance(cells, dict) or cell_name not in cells:
        raise KeyError(
            "Cell {!r} was not found in nervous_system.cells".format(cell_name)
        )

    muscle = result.get("Muscle")
    if not isinstance(muscle, dict):
        raise KeyError("JSON does not contain a 'Muscle' object")
    muscle_count_object = muscle.get("Nmuscles")
    if (
        not isinstance(muscle_count_object, dict)
        or isinstance(muscle_count_object.get("value"), bool)
        or not isinstance(muscle_count_object.get("value"), int)
    ):
        raise TypeError("'Muscle.Nmuscles.value' must be an integer")
    muscle_count = muscle_count_object["value"]
    if muscle_number > muscle_count:
        raise ValueError(
            "muscle_number must not exceed the muscle count ({})".format(muscle_count)
        )

    nmj_name = "{}_nmj".format(muscle_side)
    weights = result.setdefault(
        nmj_name,
        {
            "weights": {
                "message": "{} NMJ weights in sparse format".format(
                    muscle_side.capitalize()
                ),
                "value": [],
            }
        },
    )
    if not isinstance(weights, dict):
        raise TypeError("'{}' must be a dictionary".format(nmj_name))
    weights = weights.setdefault(
        "weights",
        {
            "message": "{} NMJ weights in sparse format".format(
                muscle_side.capitalize()
            ),
            "value": [],
        },
    )
    if not isinstance(weights, dict):
        raise TypeError("'{}.weights' must be a dictionary".format(nmj_name))
    if weights.get("value") is None:
        weights["value"] = []
    connections = weights.get("value")
    if not isinstance(connections, list):
        raise TypeError("'{}.weights.value' must be a list".format(nmj_name))

    for connection in connections:
        if not isinstance(connection, dict):
            raise TypeError("Each cell-to-muscle connection must be a dictionary")
        if (
            connection.get("from_cell") == cell_name
            and connection.get("to_musc") == muscle_number
        ):
            return result

    connections.append(
        {
            "from_cell": cell_name,
            "to_musc": muscle_number,
            "weight": {"value": float(weight)},
        }
    )
    if make_evolvable or evotag_name is not None:
        result = add_evotag(
            result,
            [
                nmj_name,
                "weights",
                "value",
                len(connections) - 1,
                "weight",
            ],
            evotag_name,
        )
    return result


def add_cell(json_data):
    """Return a copy with one default cell added, plus the new cell name."""
    result, cell_names = add_random_cell_network(json_data, 1, 0.0)
    return result, cell_names[0]


def get_cell_names_by_stem(json_data, stem):
    """Return cell names whose numeric suffix follows the requested stem."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(stem, str) or not stem:
        raise ValueError("stem must be a non-empty string")

    nervous_system = json_data.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    cell_names = nervous_system.get("cell_names", {}).get("value")
    if not isinstance(cell_names, list):
        raise TypeError("'nervous_system.cell_names.value' must be a list")

    prefix = stem + "_"
    matches = []
    for cell_name in cell_names:
        if not isinstance(cell_name, str):
            raise TypeError("Each cell name must be a string")
        if cell_name.startswith(prefix) and cell_name[len(prefix) :].isdigit():
            matches.append(cell_name)
    return matches


def set_input_switcher_input(
    json_data,
    input_index,
    input_nums,
    values,
):
    """Return a copy with an input-switcher pattern added or replaced."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if isinstance(input_index, bool) or not isinstance(input_index, int):
        raise TypeError("input_index must be an integer")
    if input_index < 0:
        raise ValueError("input_index must be non-negative")
    if not isinstance(input_nums, (list, tuple)) or not input_nums:
        raise ValueError("input_nums must be a non-empty list or tuple")
    if not isinstance(values, (list, tuple)) or not values:
        raise ValueError("values must be a non-empty list or tuple")
    if len(input_nums) != len(values):
        raise ValueError("input_nums and values must have the same length")

    pattern_values = []
    seen_input_nums = set()
    for input_num, value in zip(input_nums, values):
        if isinstance(input_num, bool) or not isinstance(input_num, int):
            raise TypeError("Each input number must be an integer")
        if input_num < 1:
            raise ValueError("Each input number must be at least 1")
        if input_num in seen_input_nums:
            raise ValueError("Input number {} occurs more than once".format(input_num))
        seen_input_nums.add(input_num)

        if isinstance(value, bool) or not isinstance(value, (int, float)):
            raise TypeError("Each input value must be a number")
        if not math.isfinite(value):
            raise ValueError("Each input value must be finite")
        pattern_values.append({"input_num": input_num, "value": float(value)})

    result = copy.deepcopy(json_data)
    input_switcher = result.setdefault("input_switcher", {})
    if not isinstance(input_switcher, dict):
        raise TypeError("'input_switcher' must be a dictionary")
    inputs_object = input_switcher.setdefault("inputs", {"value": []})
    if not isinstance(inputs_object, dict):
        raise TypeError("'input_switcher.inputs' must be a dictionary")
    if inputs_object.get("value") is None:
        inputs_object["value"] = []
    inputs = inputs_object.get("value")
    if not isinstance(inputs, list):
        raise TypeError("'input_switcher.inputs.value' must be a list")

    legacy_zero_based = False
    for pattern in inputs:
        if not isinstance(pattern, dict):
            raise TypeError("Each input-switcher pattern must be a dictionary")
        pattern_entries = pattern.get("value")
        if not isinstance(pattern_entries, list):
            raise TypeError("Each input-switcher pattern value must be a list")
        for entry in pattern_entries:
            if not isinstance(entry, dict):
                raise TypeError(
                    "Each input-switcher pattern entry must be a dictionary"
                )
            existing_input_num = entry.get("input_num")
            if (
                isinstance(existing_input_num, bool)
                or not isinstance(existing_input_num, int)
                or existing_input_num < 0
            ):
                raise TypeError(
                    "Each existing input number must be a non-negative integer"
                )
            if existing_input_num == 0:
                legacy_zero_based = True

    if legacy_zero_based:
        for pattern in inputs:
            for entry in pattern["value"]:
                entry["input_num"] += 1

    new_pattern = {
        "input_index": input_index,
        "value": pattern_values,
    }
    matching_positions = []
    for position, pattern in enumerate(inputs):
        if pattern.get("input_index") == input_index:
            matching_positions.append(position)

    if matching_positions:
        inputs[matching_positions[0]] = new_pattern
        for position in reversed(matching_positions[1:]):
            inputs.pop(position)
    else:
        inputs.append(new_pattern)

    size_object = input_switcher.setdefault("size", {"value": 0})
    if not isinstance(size_object, dict):
        raise TypeError("'input_switcher.size' must be a dictionary")
    current_size = size_object.get("value", 0)
    if (
        isinstance(current_size, bool)
        or not isinstance(current_size, int)
        or current_size < 0
    ):
        raise TypeError("'input_switcher.size.value' must be a non-negative integer")
    size_object["value"] = max(current_size, input_index + 1)
    return result


def set_input_switcher_schedule(
    json_data,
    time_periods,
    input_indices,
    time_offset=0.0,
    do_evolution=False,
):
    """Return a copy with a repeating input-switcher schedule set in seconds."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(time_periods, (list, tuple)) or not time_periods:
        raise ValueError("time_periods must be a non-empty list or tuple")
    if not isinstance(input_indices, (list, tuple)) or not input_indices:
        raise ValueError("input_indices must be a non-empty list or tuple")
    if len(time_periods) != len(input_indices):
        raise ValueError("time_periods and input_indices must have the same length")
    if isinstance(time_offset, bool) or not isinstance(time_offset, (int, float)):
        raise TypeError("time_offset must be a number")
    if not math.isfinite(time_offset) or time_offset < 0:
        raise ValueError("time_offset must be finite and non-negative")
    if not isinstance(do_evolution, bool):
        raise TypeError("do_evolution must be a boolean")

    result = copy.deepcopy(json_data)
    input_switcher = result.get("input_switcher")
    if not isinstance(input_switcher, dict):
        raise KeyError("JSON does not contain an 'input_switcher' object")
    size_object = input_switcher.get("size")
    if not isinstance(size_object, dict):
        raise KeyError("JSON does not contain 'input_switcher.size'")
    input_count = size_object.get("value")
    if (
        isinstance(input_count, bool)
        or not isinstance(input_count, int)
        or input_count < 1
    ):
        raise TypeError("'input_switcher.size.value' must be a positive integer")

    periods = []
    for period in time_periods:
        if isinstance(period, bool) or not isinstance(period, (int, float)):
            raise TypeError("Each time period must be a number")
        if not math.isfinite(period) or period <= 0:
            raise ValueError("Each time period must be finite and greater than zero")
        periods.append(float(period))

    indices = []
    for input_index in input_indices:
        if isinstance(input_index, bool) or not isinstance(input_index, int):
            raise TypeError("Each input index must be an integer")
        if input_index < 0 or input_index >= input_count:
            raise ValueError(
                "Input index {} is outside the configured range 0 to {}".format(
                    input_index, input_count - 1
                )
            )
        indices.append(input_index)

    input_switcher["time_periods"] = {"value": periods}
    input_switcher["input_indices"] = {"value": indices}
    input_switcher["time_offset"] = {"value": float(time_offset)}
    input_switcher["doEvolution"] = {"value": do_evolution}
    return result


def set_funcable_schedule(
    json_data,
    function_index,
    time_intervals,
    condvals=None,
    time_offset=0.0,
    do_evolution=False,
    schedule_name=None,
):
    """Return a copy with a repeating schedule for one Funcable function."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if (
        isinstance(function_index, bool)
        or not isinstance(function_index, int)
        or function_index < 1
    ):
        raise ValueError("function_index must be a positive integer")
    if not isinstance(time_intervals, (list, tuple)) or not time_intervals:
        raise ValueError("time_intervals must be a non-empty list or tuple")
    if isinstance(time_offset, bool) or not isinstance(time_offset, (int, float)):
        raise TypeError("time_offset must be a number")
    if not math.isfinite(time_offset) or time_offset < 0:
        raise ValueError("time_offset must be finite and non-negative")
    if not isinstance(do_evolution, bool):
        raise TypeError("do_evolution must be a boolean")
    if schedule_name is not None and (
        not isinstance(schedule_name, str) or not schedule_name
    ):
        raise ValueError("schedule_name must be a non-empty string or None")

    intervals = []
    for interval in time_intervals:
        if isinstance(interval, bool) or not isinstance(interval, (int, float)):
            raise TypeError("Each time interval must be a number")
        if not math.isfinite(interval) or interval <= 0:
            raise ValueError("Each time interval must be finite and greater than zero")
        intervals.append(float(interval))

    condition_values = None
    if condvals is not None:
        if not isinstance(condvals, (list, tuple)):
            raise TypeError("condvals must be a list, tuple or None")
        if len(condvals) != len(intervals):
            raise ValueError("condvals and time_intervals must have the same length")
        condition_values = []
        for condval in condvals:
            if isinstance(condval, bool) or not isinstance(condval, int):
                raise TypeError("Each condval must be an integer")
            condition_values.append(condval)

    result = copy.deepcopy(json_data)
    funcable = result.setdefault("Funcable", {})
    if not isinstance(funcable, dict):
        raise TypeError("'Funcable' must be a dictionary")
    schedules = funcable.setdefault("schedules", {})
    if not isinstance(schedules, dict):
        raise TypeError("'Funcable.schedules' must be a dictionary")

    if schedule_name is None:
        schedule_name = "function_{}".format(function_index)
    schedule = {
        "function_index": {"value": function_index},
        "time_intervals": {"value": intervals},
        "time_offset": {"value": float(time_offset)},
        "doEvolution": {"value": do_evolution},
    }
    if condition_values is not None:
        schedule["condvals"] = {"value": condition_values}
    schedules[schedule_name] = schedule
    return result


def delete_all_evotags(json_data):
    """Return a copy without evotags or their registry fields."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")

    fields_to_remove = {"evotag", "evolvable_ranges", "evolved_used"}

    def remove_fields(value):
        if isinstance(value, dict):
            return {
                key: remove_fields(child)
                for key, child in value.items()
                if key not in fields_to_remove
            }
        if isinstance(value, list):
            return [remove_fields(child) for child in value]
        return copy.deepcopy(value)

    return remove_fields(json_data)


def find_evotag_occurrences(json_data, evotag_name):
    """Return matching evotag entries indexed by their JSON paths."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(evotag_name, str) or not evotag_name:
        raise ValueError("evotag_name must be a non-empty string")

    occurrences = {}

    def path_string(path):
        result = ""
        for part in path:
            if isinstance(part, int):
                result += "[{}]".format(part)
            else:
                if result:
                    result += "."
                result += str(part)
        return result

    def find_matches(value, path=()):
        if isinstance(value, dict):
            if value.get("evotag") == evotag_name:
                occurrences[path_string(path)] = copy.deepcopy(value)
            for key, child in value.items():
                find_matches(child, path + (key,))
        elif isinstance(value, list):
            for index, child in enumerate(value):
                find_matches(child, path + (index,))

    find_matches(json_data)
    return occurrences


def find_evotag_objects(json_data, evotag_name):
    """Return the JSON hierarchy containing objects with a matching evotag."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(evotag_name, str) or not evotag_name:
        raise ValueError("evotag_name must be a non-empty string")

    no_match = object()
    connection_context_keys = (
        "from",
        "to",
        "from_cell",
        "to_cell",
        "from_input",
        "input_num",
        "from_output",
        "to_output",
        "from_sr",
        "to_ns",
        "from_musc",
        "to_musc",
        "to_seg",
        "cell_ind",
    )

    def connection_context(value):
        return {
            key: copy.deepcopy(value[key])
            for key in connection_context_keys
            if key in value
        }

    def matching_hierarchy(value):
        if isinstance(value, dict):
            if value.get("evotag") == evotag_name:
                return copy.deepcopy(value)

            matches = {}
            for key, child in value.items():
                child_match = matching_hierarchy(child)
                if child_match is not no_match:
                    matches[key] = child_match
            if matches:
                matches = {**connection_context(value), **matches}
            return matches if matches else no_match

        if isinstance(value, list):
            matches = []
            for child in value:
                child_match = matching_hierarchy(child)
                if child_match is not no_match:
                    matches.append(child_match)
            return matches if matches else no_match

        return no_match

    result = matching_hierarchy(json_data)
    return {} if result is no_match else result


def rename_cell(json_data, old_name, new_name):
    """Return a copy with a nervous-system cell renamed everywhere."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    for argument_name, value in (
        ("old_name", old_name),
        ("new_name", new_name),
    ):
        if not isinstance(value, str) or not value:
            raise ValueError("{} must be a non-empty string".format(argument_name))

    nervous_system = json_data.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    cells = nervous_system.get("cells")
    if not isinstance(cells, dict):
        raise KeyError("JSON does not contain 'nervous_system.cells'")
    if old_name not in cells:
        raise KeyError(
            "Cell {!r} was not found in nervous_system.cells".format(old_name)
        )
    if new_name != old_name and new_name in cells:
        raise ValueError("Cell {!r} already exists".format(new_name))
    if new_name == old_name:
        return copy.deepcopy(json_data)

    cell_names = nervous_system.get("cell_names", {}).get("value")
    cell_index = None
    if isinstance(cell_names, list) and old_name in cell_names:
        cell_index = cell_names.index(old_name)

    def replace_name(value):
        if isinstance(value, dict):
            renamed = {}
            for key, child in value.items():
                renamed_key = new_name if key == old_name else key
                if renamed_key in renamed:
                    raise ValueError(
                        "Renaming {!r} to {!r} would overwrite a dictionary "
                        "entry".format(old_name, new_name)
                    )
                renamed[renamed_key] = replace_name(child)
            return renamed
        if isinstance(value, list):
            return [replace_name(child) for child in value]
        if value == old_name:
            return new_name
        return copy.deepcopy(value)

    result = replace_name(json_data)

    no_suffix_names = (
        result.get("nervous_system", {}).get("cell_names_no_suffix", {}).get("value")
    )
    if (
        cell_index is not None
        and isinstance(no_suffix_names, list)
        and cell_index < len(no_suffix_names)
    ):
        stem, separator, suffix = new_name.rpartition("_")
        no_suffix_names[cell_index] = (
            stem if separator and suffix.isdigit() else new_name
        )

    return result


def add_random_cell_network(
    json_data,
    number_of_cells,
    connection_probability,
    random_seed=None,
):
    """Return a copy plus names for an isolated random directed cell network."""
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

    no_suffix_object = nervous_system.setdefault("cell_names_no_suffix", {"value": []})
    if not isinstance(no_suffix_object, dict):
        raise TypeError("'nervous_system.cell_names_no_suffix' must be a dictionary")
    if no_suffix_object.get("value") is None:
        no_suffix_object["value"] = []
    no_suffix_names = no_suffix_object.get("value")
    if not isinstance(no_suffix_names, list):
        raise TypeError("'nervous_system.cell_names_no_suffix.value' must be a list")

    chemical_conns = nervous_system.setdefault("chemical_conns", {"value": []})
    if not isinstance(chemical_conns, dict):
        raise TypeError("'nervous_system.chemical_conns' must be a dictionary")
    if chemical_conns.get("value") is None:
        chemical_conns["value"] = []
    connections = chemical_conns.get("value")
    if not isinstance(connections, list):
        raise TypeError("'nervous_system.chemical_conns.value' must be a list")

    existing_names = set(cells) | set(cell_names)
    new_names = []
    name_index = 1
    while len(new_names) < number_of_cells:
        full_name = "Cell_{}".format(name_index)
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
        no_suffix_names.append(full_name)
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

    return result, new_names


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
                "Cell {!r} was not found in nervous_system.cells".format(cell_name)
            )

    chemical_conns = nervous_system.setdefault("chemical_conns", {"value": []})
    if not isinstance(chemical_conns, dict):
        raise TypeError("'nervous_system.chemical_conns' must be a dictionary")
    if chemical_conns.get("value") is None:
        chemical_conns["value"] = []
    connections = chemical_conns.get("value")
    if not isinstance(connections, list):
        raise TypeError("'nervous_system.chemical_conns.value' must be a list")

    for connection in connections:
        if not isinstance(connection, dict):
            raise TypeError("Each chemical connection must be a dictionary")
        if connection.get("from") == from_cell and connection.get("to") == to_cell:
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


def _get_cell_connection(
    json_data,
    from_cell,
    to_cell,
    connection_key,
    reciprocal=False,
):
    """Return a copy of a matching nervous-system connection, if present."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    for parameter_name, cell_name in (
        ("from_cell", from_cell),
        ("to_cell", to_cell),
    ):
        if not isinstance(cell_name, str) or not cell_name:
            raise ValueError("{} must be a non-empty string".format(parameter_name))

    nervous_system = json_data.get("nervous_system")
    if not isinstance(nervous_system, dict):
        return None
    connection_object = nervous_system.get(connection_key)
    if connection_object is None:
        return None
    if not isinstance(connection_object, dict):
        raise TypeError(
            "'nervous_system.{}' must be a dictionary".format(connection_key)
        )
    connections = connection_object.get("value")
    if connections is None:
        return None
    if not isinstance(connections, list):
        raise TypeError(
            "'nervous_system.{}.value' must be a list".format(connection_key)
        )

    for connection in connections:
        if not isinstance(connection, dict):
            raise TypeError("Each connection must be a dictionary")
        direct_match = (
            connection.get("from") == from_cell and connection.get("to") == to_cell
        )
        reverse_match = reciprocal and (
            connection.get("from") == to_cell and connection.get("to") == from_cell
        )
        if direct_match or reverse_match:
            return copy.deepcopy(connection)
    return None


def get_chemical_connection(json_data, from_cell, to_cell):
    """Return the directed chemical connection, or None if it is absent."""
    return _get_cell_connection(
        json_data,
        from_cell,
        to_cell,
        "chemical_conns",
    )


def get_electrical_connection(json_data, first_cell, second_cell):
    """Return the reciprocal electrical connection, or None if absent."""
    return _get_cell_connection(
        json_data,
        first_cell,
        second_cell,
        "electrical_conns",
        reciprocal=True,
    )


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
                "Cell {!r} was not found in nervous_system.cells".format(cell_name)
            )

    chemical_conns = nervous_system.get("chemical_conns")
    if not isinstance(chemical_conns, dict):
        raise KeyError("JSON does not contain 'nervous_system.chemical_conns'")
    connections = chemical_conns.get("value")
    if connections is None:
        return result
    if not isinstance(connections, list):
        raise TypeError("'nervous_system.chemical_conns.value' must be a list")

    removed_evotags = set()
    remaining_connections = []
    for connection in connections:
        if not isinstance(connection, dict):
            raise TypeError("Each chemical connection must be a dictionary")
        if connection.get("from") == from_cell and connection.get("to") == to_cell:
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

    unused_evotags = removed_evotags - collect_remaining_evotags(result, at_root=True)
    ranges = result.get("evolvable_ranges")
    if isinstance(ranges, dict):
        for evotag in unused_evotags:
            ranges.pop(str(evotag), None)

    evolved_used = result.get("evolved_used")
    if isinstance(evolved_used, dict) and isinstance(evolved_used.get("value"), list):
        evolved_used["value"] = [
            evotag for evotag in evolved_used["value"] if evotag not in unused_evotags
        ]
    return result


def _add_connection_evotag(
    json_data,
    from_cell,
    to_cell,
    connection_key,
    range_name,
    lower_limit,
    upper_limit,
    evotag_name=None,
    reciprocal=False,
):
    """Add an evotag to an existing nervous-system connection."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    for argument_name, value in (
        ("from_cell", from_cell),
        ("to_cell", to_cell),
    ):
        if not isinstance(value, str) or not value:
            raise ValueError("{} must be a non-empty string".format(argument_name))
    if evotag_name is not None and (
        not isinstance(evotag_name, str) or not evotag_name
    ):
        raise ValueError("evotag_name must be a non-empty string or None")

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
                "Cell {!r} was not found in nervous_system.cells".format(cell_name)
            )

    connection_object = nervous_system.get(connection_key)
    if not isinstance(connection_object, dict):
        raise KeyError(
            "JSON does not contain 'nervous_system.{}'".format(connection_key)
        )
    connections = connection_object.get("value")
    if not isinstance(connections, list):
        raise TypeError(
            "'nervous_system.{}.value' must be a list".format(connection_key)
        )

    matching_connections = []
    for connection in connections:
        if not isinstance(connection, dict):
            raise TypeError("Each connection must be a dictionary")
        direct_match = (
            connection.get("from") == from_cell and connection.get("to") == to_cell
        )
        reverse_match = reciprocal and (
            connection.get("from") == to_cell and connection.get("to") == from_cell
        )
        if direct_match or reverse_match:
            weight = connection.get("weight")
            if not isinstance(weight, dict) or "value" not in weight:
                raise TypeError("Connection weight must contain a value")
            matching_connections.append(connection)

    if not matching_connections:
        connection_type = "electrical" if reciprocal else "chemical"
        raise KeyError(
            "No {} connection exists between {!r} and {!r}".format(
                connection_type, from_cell, to_cell
            )
        )

    evolvable_ranges = result.setdefault("evolvable_ranges", {})
    if not isinstance(evolvable_ranges, dict):
        raise TypeError("'evolvable_ranges' must be a dictionary")

    if evotag_name is None:
        existing_evotag = matching_connections[0]["weight"].get("evotag")
        if isinstance(existing_evotag, str) and existing_evotag:
            evotag_name = existing_evotag

    if evotag_name is None:
        used_evotags = set(evolvable_ranges)

        def collect_evotags(value):
            if isinstance(value, dict):
                evotag = value.get("evotag")
                if isinstance(evotag, str) and evotag:
                    used_evotags.add(evotag)
                for child in value.values():
                    collect_evotags(child)
            elif isinstance(value, list):
                for child in value:
                    collect_evotags(child)

        collect_evotags(nervous_system)
        suffix = 0
        while "{}_{}".format(range_name, suffix) in used_evotags:
            suffix += 1
        evotag_name = "{}_{}".format(range_name, suffix)

    for connection in matching_connections:
        connection["weight"]["evotag"] = evotag_name

    if evotag_name not in evolvable_ranges:
        evolvable_ranges[evotag_name] = {
            "active": True,
            "lower_limit": lower_limit,
            "name": range_name,
            "upper_limit": upper_limit,
        }
    return result


def add_chemical_connection_evotag(
    json_data,
    from_cell,
    to_cell,
    evotag_name=None,
):
    """Return a copy with an evotag added to a chemical connection."""
    return _add_connection_evotag(
        json_data,
        from_cell,
        to_cell,
        "chemical_conns",
        "ns_chemcons",
        -15.0,
        15.0,
        evotag_name=evotag_name,
    )


def add_chemical_connection_stem_evotag(
    json_data,
    from_stem,
    to_stem,
    evotag_name=None,
):
    """Return updated JSON and the evotag assigned to same-index connections."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    for argument_name, value in (
        ("from_stem", from_stem),
        ("to_stem", to_stem),
    ):
        if not isinstance(value, str) or not value:
            raise ValueError("{} must be a non-empty string".format(argument_name))
    if evotag_name is not None and (
        not isinstance(evotag_name, str) or not evotag_name
    ):
        raise ValueError("evotag_name must be a non-empty string or None")

    result = copy.deepcopy(json_data)
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    connection_object = nervous_system.get("chemical_conns")
    if not isinstance(connection_object, dict):
        raise KeyError("JSON does not contain 'nervous_system.chemical_conns'")
    connections = connection_object.get("value")
    if not isinstance(connections, list):
        raise TypeError("'nervous_system.chemical_conns.value' must be a list")

    def matching_suffix(cell_name, stem):
        if not isinstance(cell_name, str):
            return None
        prefix = stem + "_"
        if not cell_name.startswith(prefix):
            return None
        suffix = cell_name[len(prefix) :]
        return suffix if suffix.isdigit() else None

    matching_connections = []
    for connection in connections:
        if not isinstance(connection, dict):
            raise TypeError("Each chemical connection must be a dictionary")
        from_suffix = matching_suffix(connection.get("from"), from_stem)
        to_suffix = matching_suffix(connection.get("to"), to_stem)
        if from_suffix is None or from_suffix != to_suffix:
            continue
        weight = connection.get("weight")
        if not isinstance(weight, dict) or "value" not in weight:
            raise TypeError("Connection weight must contain a value")
        matching_connections.append(connection)

    if not matching_connections:
        raise KeyError(
            "No same-index chemical connections exist from stem {!r} "
            "to stem {!r}".format(from_stem, to_stem)
        )

    evolvable_ranges = result.setdefault("evolvable_ranges", {})
    if not isinstance(evolvable_ranges, dict):
        raise TypeError("'evolvable_ranges' must be a dictionary")

    if evotag_name is None:
        used_evotags = set(evolvable_ranges)

        def collect_evotags(value):
            if isinstance(value, dict):
                evotag = value.get("evotag")
                if isinstance(evotag, str) and evotag:
                    used_evotags.add(evotag)
                for child in value.values():
                    collect_evotags(child)
            elif isinstance(value, list):
                for child in value:
                    collect_evotags(child)

        collect_evotags(result)
        suffix = 0
        while "ns_chemcons_{}".format(suffix) in used_evotags:
            suffix += 1
        evotag_name = "ns_chemcons_{}".format(suffix)

    for connection in matching_connections:
        connection["weight"]["evotag"] = evotag_name

    if evotag_name not in evolvable_ranges:
        evolvable_ranges[evotag_name] = {
            "active": True,
            "lower_limit": -15.0,
            "name": "ns_chemcons",
            "upper_limit": 15.0,
        }
    return result, evotag_name


def remove_chemical_connection_stem_mfunc(
    json_data,
    from_stem,
    to_stem,
):
    """Remove mfunc from same-index chemical connections between two stems."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    for argument_name, value in (
        ("from_stem", from_stem),
        ("to_stem", to_stem),
    ):
        if not isinstance(value, str) or not value:
            raise ValueError("{} must be a non-empty string".format(argument_name))

    result = copy.deepcopy(json_data)
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    connection_object = nervous_system.get("chemical_conns")
    if not isinstance(connection_object, dict):
        raise KeyError("JSON does not contain 'nervous_system.chemical_conns'")
    connections = connection_object.get("value")
    if not isinstance(connections, list):
        raise TypeError("'nervous_system.chemical_conns.value' must be a list")

    def matching_suffix(cell_name, stem):
        if not isinstance(cell_name, str):
            return None
        prefix = stem + "_"
        if not cell_name.startswith(prefix):
            return None
        suffix = cell_name[len(prefix) :]
        return suffix if suffix.isdigit() else None

    for connection in connections:
        if not isinstance(connection, dict):
            raise TypeError("Each chemical connection must be a dictionary")
        from_suffix = matching_suffix(connection.get("from"), from_stem)
        to_suffix = matching_suffix(connection.get("to"), to_stem)
        if from_suffix is None or from_suffix != to_suffix:
            continue
        weight = connection.get("weight")
        if not isinstance(weight, dict):
            raise TypeError("Connection weight must be a dictionary")
        weight.pop("mfunc", None)

    return result


def add_electrical_connection_evotag(
    json_data,
    first_cell,
    second_cell,
    evotag_name=None,
):
    """Return a copy with an evotag added to an electrical connection."""
    return _add_connection_evotag(
        json_data,
        first_cell,
        second_cell,
        "electrical_conns",
        "ns_eleccons",
        0.0,
        2.0,
        evotag_name=evotag_name,
        reciprocal=True,
    )


def add_cell_parameter_evotag(
    json_data,
    cell_name,
    parameter_name,
    evotag_name=None,
):
    """Return a copy with a cell parameter marked as evolvable."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    for argument_name, value in (
        ("cell_name", cell_name),
        ("parameter_name", parameter_name),
    ):
        if not isinstance(value, str) or not value:
            raise ValueError("{} must be a non-empty string".format(argument_name))
    if evotag_name is not None and (
        not isinstance(evotag_name, str) or not evotag_name
    ):
        raise ValueError("evotag_name must be a non-empty string or None")

    result = copy.deepcopy(json_data)
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    cells = nervous_system.get("cells")
    if not isinstance(cells, dict) or cell_name not in cells:
        raise KeyError(
            "Cell {!r} was not found in nervous_system.cells".format(cell_name)
        )
    cell = cells[cell_name]
    if not isinstance(cell, dict) or parameter_name not in cell:
        raise KeyError(
            "Parameter {!r} was not found for cell {!r}".format(
                parameter_name, cell_name
            )
        )
    parameter = cell[parameter_name]
    if not isinstance(parameter, dict) or "value" not in parameter:
        raise TypeError(
            "{}.{} must be an object containing 'value'".format(
                cell_name, parameter_name
            )
        )
    parameter_value = parameter["value"]
    if isinstance(parameter_value, bool) or not isinstance(
        parameter_value, (int, float)
    ):
        raise TypeError(
            "{}.{} must have a numeric value".format(cell_name, parameter_name)
        )

    evolvable_ranges = result.setdefault("evolvable_ranges", {})
    if not isinstance(evolvable_ranges, dict):
        raise TypeError("'evolvable_ranges' must be a dictionary")

    existing_evotag = parameter.get("evotag")
    if evotag_name is None and isinstance(existing_evotag, str):
        evotag_name = existing_evotag

    range_name = "ns_cells_{}".format(parameter_name)
    if evotag_name is None:
        suffix = 0
        while "{}_{}".format(range_name, suffix) in evolvable_ranges:
            suffix += 1
        evotag_name = "{}_{}".format(range_name, suffix)

    parameter["evotag"] = evotag_name

    if evotag_name not in evolvable_ranges:
        default_ranges = {
            "bias": (-15.0, 15.0),
            "tau": (0.1, 4.2),
            "gain": (0.0, 2.0),
            "state": (-1.0, 1.0),
        }
        if parameter_name in default_ranges:
            lower_limit, upper_limit = default_ranges[parameter_name]
        else:
            span = max(1.0, abs(float(parameter_value)))
            lower_limit = float(parameter_value) - span
            upper_limit = float(parameter_value) + span

        evolvable_ranges[evotag_name] = {
            "active": True,
            "lower_limit": lower_limit,
            "name": range_name,
            "upper_limit": upper_limit,
        }

    return result


def set_json_value(json_data, keys, new_value):
    """Return a copy with the value at an existing JSON path replaced."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(keys, (list, tuple)) or not keys:
        raise ValueError("keys must be a non-empty list or tuple")

    result = copy.deepcopy(json_data)
    current = result
    for depth, key in enumerate(keys[:-1]):
        if isinstance(current, dict):
            if key not in current:
                raise KeyError(
                    "JSON path does not contain {!r} at position {}".format(key, depth)
                )
            current = current[key]
        elif isinstance(current, list):
            if isinstance(key, bool) or not isinstance(key, int):
                raise TypeError(
                    "List path component at position {} must be an integer".format(
                        depth
                    )
                )
            try:
                current = current[key]
            except IndexError:
                raise IndexError(
                    "List index {} is out of range at path position {}".format(
                        key, depth
                    )
                )
        else:
            raise TypeError(
                "JSON path reaches a non-container at position {}".format(depth)
            )

    final_position = len(keys) - 1
    final_key = keys[-1]
    if isinstance(current, dict):
        if final_key not in current:
            raise KeyError(
                "JSON path does not contain {!r} at position {}".format(
                    final_key, final_position
                )
            )
        current[final_key] = copy.deepcopy(new_value)
    elif isinstance(current, list):
        if isinstance(final_key, bool) or not isinstance(final_key, int):
            raise TypeError(
                "List path component at position {} must be an integer".format(
                    final_position
                )
            )
        try:
            current[final_key] = copy.deepcopy(new_value)
        except IndexError:
            raise IndexError(
                "List index {} is out of range at path position {}".format(
                    final_key, final_position
                )
            )
    else:
        raise TypeError(
            "JSON path reaches a non-container at position {}".format(final_position)
        )

    return result


def add_evotag(json_data, keys, evotag_name=None):
    """Return a copy with the numeric value at a JSON path made evolvable."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(keys, (list, tuple)) or not keys:
        raise ValueError("keys must be a non-empty list or tuple")
    if evotag_name is not None and (
        not isinstance(evotag_name, str) or not evotag_name
    ):
        raise ValueError("evotag_name must be a non-empty string or None")

    result = copy.deepcopy(json_data)
    current = result
    for depth, key in enumerate(keys):
        if isinstance(current, dict):
            if key not in current:
                raise KeyError(
                    "JSON path does not contain {!r} at position {}".format(key, depth)
                )
            current = current[key]
        elif isinstance(current, list):
            if isinstance(key, bool) or not isinstance(key, int):
                raise TypeError(
                    "List path component at position {} must be an integer".format(
                        depth
                    )
                )
            try:
                current = current[key]
            except IndexError:
                raise IndexError(
                    "List index {} is out of range at path position {}".format(
                        key, depth
                    )
                )
        else:
            raise TypeError(
                "JSON path reaches a non-container at position {}".format(depth)
            )

    if keys[-1] == "value":
        parameter = result
        for key in keys[:-1]:
            if isinstance(parameter, dict):
                parameter = parameter[key]
            else:
                parameter = parameter[key]
    else:
        parameter = current

    if not isinstance(parameter, dict) or "value" not in parameter:
        raise TypeError("The JSON path must identify an object containing 'value'")
    parameter_value = parameter["value"]
    if isinstance(parameter_value, bool) or not isinstance(
        parameter_value, (int, float)
    ):
        raise TypeError("The selected value must be numeric")

    name_parts = []
    replacements = {
        "nervous_system": "ns",
        "chemical_conns": "chemcons",
        "electrical_conns": "eleccons",
        "stretch_receptor": "sr",
    }
    previous_key = None
    for key in keys:
        if isinstance(key, int) or key == "value":
            continue
        if previous_key == "cells":
            previous_key = key
            continue
        part = replacements.get(str(key), str(key))
        if (
            part == "weight"
            and name_parts
            and name_parts[-1] in {"chemcons", "eleccons"}
        ):
            continue
        name_parts.append(part)
        previous_key = key
    if len(name_parts) >= 3 and name_parts[-1] == "weight" and "weights" in name_parts:
        name_parts.remove("weights")
    range_name = "_".join(name_parts)
    if not range_name:
        raise ValueError("The JSON path does not produce a valid evotag name")
    terminal_name = next(
        (
            str(key)
            for key in reversed(keys)
            if not isinstance(key, int) and key != "value"
        ),
        "",
    )

    evolvable_ranges = result.setdefault("evolvable_ranges", {})
    if not isinstance(evolvable_ranges, dict):
        raise TypeError("'evolvable_ranges' must be a dictionary")

    existing_evotag = parameter.get("evotag")
    if evotag_name is None and isinstance(existing_evotag, str):
        evotag_name = existing_evotag

    if evotag_name is None:
        used_evotags = set(evolvable_ranges)

        def collect_evotags(value):
            if isinstance(value, dict):
                evotag = value.get("evotag")
                if isinstance(evotag, str) and evotag:
                    used_evotags.add(evotag)
                for child in value.values():
                    collect_evotags(child)
            elif isinstance(value, list):
                for child in value:
                    collect_evotags(child)

        collect_evotags(result)
        suffix = 0
        while "{}_{}".format(range_name, suffix) in used_evotags:
            suffix += 1
        evotag_name = "{}_{}".format(range_name, suffix)

    parameter["evotag"] = evotag_name

    if evotag_name not in evolvable_ranges:
        matching_limits = Counter()
        for range_entry in evolvable_ranges.values():
            if (
                isinstance(range_entry, dict)
                and range_entry.get("name") == range_name
                and isinstance(range_entry.get("lower_limit"), (int, float))
                and isinstance(range_entry.get("upper_limit"), (int, float))
            ):
                matching_limits[
                    (
                        range_entry["lower_limit"],
                        range_entry["upper_limit"],
                    )
                ] += 1

        if matching_limits:
            lower_limit, upper_limit = matching_limits.most_common(1)[0][0]
        elif "chemical_conns" in keys:
            lower_limit, upper_limit = -15.0, 15.0
        elif "electrical_conns" in keys:
            lower_limit, upper_limit = 0.0, 2.0
        elif "driving_inputs" in keys and terminal_name == "weight":
            lower_limit, upper_limit = -1500.0, 1500.0
        elif "sensors" in keys and terminal_name == "weight":
            lower_limit, upper_limit = -1500.0, 1500.0
        elif terminal_name in {"sensor_n", "sensor_m", "tau"}:
            lower_limit, upper_limit = 0.1, 4.2
        elif terminal_name == "bias":
            lower_limit, upper_limit = -15.0, 15.0
        elif terminal_name == "gain":
            lower_limit, upper_limit = 0.0, 2.0
        elif terminal_name == "state":
            lower_limit, upper_limit = -1.0, 1.0
        else:
            span = max(1.0, abs(float(parameter_value)))
            lower_limit = float(parameter_value) - span
            upper_limit = float(parameter_value) + span

        evolvable_ranges[evotag_name] = {
            "active": True,
            "lower_limit": lower_limit,
            "name": range_name,
            "upper_limit": upper_limit,
        }

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

    if "ext_inp_1" not in sensor and "ext_inp_2" not in sensor:
        collect_evotags(sensor)
        del sensors[sensor_name]

        def sensor_number(name):
            prefix = "sensor_"
            if not name.startswith(prefix) or not name[len(prefix) :].isdigit():
                raise ValueError(
                    "Sensor names must use the form 'sensor_N': {!r}".format(name)
                )
            return int(name[len(prefix) :])

        ordered_sensors = sorted(
            sensors.items(), key=lambda item: sensor_number(item[0])
        )
        sensors.clear()
        for index, (_, remaining_sensor) in enumerate(ordered_sensors, start=1):
            sensors["sensor_{}".format(index)] = remaining_sensor
        if not sensors:
            result.pop("sensors", None)
            for section_name in ("worm", "Worm"):
                section = result.get(section_name)
                if isinstance(section, dict):
                    section.pop("sensorM", None)
                    section.pop("sensorN", None)

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

    removed_input_numbers = set()
    for key in ("ext_inp_1", "ext_inp_2"):
        value = sensor.get(key, {}).get("value")
        if isinstance(value, bool) or not isinstance(value, int) or value < 0:
            raise ValueError(
                "{}.{} must contain a non-negative integer".format(sensor_name, key)
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
                "Connection refers to missing driving input {}".format(input_number)
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
                    "Sensor refers to missing driving input {}".format(old_input_number)
                )
            remaining_sensor[key]["value"] = input_number_map[old_input_number] - 1

    def sensor_number(name):
        prefix = "sensor_"
        if not name.startswith(prefix) or not name[len(prefix) :].isdigit():
            raise ValueError(
                "Sensor names must use the form 'sensor_N': {!r}".format(name)
            )
        return int(name[len(prefix) :])

    ordered_sensors = sorted(sensors.items(), key=lambda item: sensor_number(item[0]))
    sensors.clear()
    for index, (_, remaining_sensor) in enumerate(ordered_sensors, start=1):
        sensors["sensor_{}".format(index)] = remaining_sensor
    if not sensors:
        result.pop("sensors", None)
        for section_name in ("worm", "Worm"):
            section = result.get(section_name)
            if isinstance(section, dict):
                section.pop("sensorM", None)
                section.pop("sensorN", None)

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

    unused_evotags = removed_evotags - collect_remaining_evotags(result, at_root=True)
    ranges = result.get("evolvable_ranges")
    if isinstance(ranges, dict):
        for evotag in unused_evotags:
            ranges.pop(str(evotag), None)

    evolved_used = result.get("evolved_used")
    if isinstance(evolved_used, dict) and isinstance(evolved_used.get("value"), list):
        evolved_used["value"] = [
            evotag for evotag in evolved_used["value"] if evotag not in unused_evotags
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
            if child == cell_name and (
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


def delete_subfolder_directory(subfolder_name, subsubfolder_name):
    return delete_notebook_directory(subfolder_name, subsubfolder_name)


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
