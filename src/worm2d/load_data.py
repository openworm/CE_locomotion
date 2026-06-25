#####
# Script to load generated data on worm motion/cell activity & generate graphical output
#####


import numpy as np
from matplotlib import pyplot as plt
import sys
import random
from datetime import datetime

# import argparse
import os
from .neuromlLocal import utils

# from matplotlib.ticker import MaxNLocator
import math
from . import helper_funcs as hf

# import neuromlLocal.utils as utils
import matplotlib as mpl
from cycler import cycler
import itertools
from itertools import cycle
import matplotlib.colors as mcolors


def get_evolvable_ranges(network_json_data):
    if "evolvable_ranges" in network_json_data:
        return network_json_data["evolvable_ranges"]
    return network_json_data.get("Evolvable")


def get_evolved_used_order(network_json_data):
    evolved_used = network_json_data.get("evolved_used", {})
    if isinstance(evolved_used, dict):
        evolved_used = evolved_used.get("value", [])
    if isinstance(evolved_used, list):
        return [str(val) for val in evolved_used]
    return []


def _json_value(obj, default=None):
    if isinstance(obj, dict) and "value" in obj:
        return obj["value"]
    if obj is None:
        return default
    return obj


def _normalise_cell_class_name(name):
    cleaned = str(name).strip().lower().replace("_", " ")
    for suffix in (" neurons", " neuron", " cells", " cell", " class"):
        if cleaned.endswith(suffix):
            cleaned = cleaned[: -len(suffix)]
    cleaned = cleaned.strip().replace(" ", "_")
    aliases = {
        "body": "vnc",
        "vnc_neurons": "vnc",
        "ventral_nerve_cord": "vnc",
        "inter": "interneuron",
        "inter_neurons": "interneuron",
        "interneurons": "interneuron",
    }
    return aliases.get(cleaned, cleaned)


def _cell_names_for_class(network_json_data, cell_class):
    nervous_system = network_json_data.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")

    cell_names = _json_value(nervous_system.get("cell_names"), [])
    if not isinstance(cell_names, list) or not cell_names:
        raise ValueError("'nervous_system.cell_names.value' must be a non-empty list")

    cells = nervous_system.get("cells", {})
    if not isinstance(cells, dict):
        cells = {}

    wanted_class = _normalise_cell_class_name(cell_class)
    selected_cells = []
    for cell_name in cell_names:
        cell = cells.get(cell_name, {})
        class_name = _json_value(cell.get("cell_class"))
        if (
            class_name is not None
            and _normalise_cell_class_name(class_name) == wanted_class
        ):
            selected_cells.append(cell_name)
    return selected_cells


def get_activity_cell_names_by_numbers(
    output_folder,
    cell_class,
    cell_numbers,
    one_based=False,
):
    """Map ExampleActivity imshow row numbers for one cell class to cell names."""
    worm_file = os.path.join(output_folder, "worm_data_worm.json")
    if not os.path.isfile(worm_file):
        raise FileNotFoundError(
            "Could not find worm_data_worm.json in {}".format(output_folder)
        )
    if isinstance(cell_numbers, (int, np.integer)):
        cell_numbers = [int(cell_numbers)]
    if not isinstance(cell_numbers, (list, tuple)) or not cell_numbers:
        raise ValueError("cell_numbers must be a non-empty list or tuple")

    network_json_data = utils.getJsonFile(worm_file)
    class_cells = _cell_names_for_class(network_json_data, cell_class)
    if not class_cells:
        raise ValueError("No cells found for cell class {!r}".format(cell_class))

    selected_cells = []
    for cell_number in cell_numbers:
        if isinstance(cell_number, bool) or not isinstance(
            cell_number, (int, np.integer)
        ):
            raise TypeError("Each cell number must be an integer")
        index = int(cell_number) - 1 if one_based else int(cell_number)
        if index < 0 or index >= len(class_cells):
            raise IndexError(
                "Cell number {} is outside the valid range {} to {}".format(
                    cell_number,
                    1 if one_based else 0,
                    len(class_cells) if one_based else len(class_cells) - 1,
                )
            )
        selected_cells.append(class_cells[index])
    return selected_cells


def plot_selected_activity(
    output_folder,
    cells_or_class,
    time_interval=None,
    save_png=False,
    filename="SelectedActivity.png",
):
    """Return a two-panel activity figure for selected cells or one cell class."""
    act_file = os.path.join(output_folder, "act.dat")
    worm_file = os.path.join(output_folder, "worm_data_worm.json")
    if not os.path.isfile(act_file):
        raise FileNotFoundError("Could not find act.dat in {}".format(output_folder))
    if not os.path.isfile(worm_file):
        raise FileNotFoundError(
            "Could not find worm_data_worm.json in {}".format(output_folder)
        )

    network_json_data = utils.getJsonFile(worm_file)
    nervous_system = network_json_data.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")

    cell_names = _json_value(nervous_system.get("cell_names"), [])
    if not isinstance(cell_names, list) or not cell_names:
        raise ValueError("'nervous_system.cell_names.value' must be a non-empty list")

    cells = nervous_system.get("cells", {})
    if not isinstance(cells, dict):
        cells = {}

    if isinstance(cells_or_class, str):
        selected_cells = _cell_names_for_class(network_json_data, cells_or_class)
        if not selected_cells:
            raise ValueError(
                "No cells found for cell class {!r}".format(cells_or_class)
            )
    elif isinstance(cells_or_class, (list, tuple)):
        selected_cells = list(cells_or_class)
        if not selected_cells:
            raise ValueError("cells_or_class must contain at least one cell name")
    else:
        raise TypeError("cells_or_class must be a cell-class string or a list of cells")

    missing_cells = [cell for cell in selected_cells if cell not in cell_names]
    if missing_cells:
        raise ValueError("Unknown cell name(s): {}".format(", ".join(missing_cells)))

    act_data = np.loadtxt(act_file).T
    if act_data.ndim != 2 or act_data.shape[0] < len(cell_names) + 1:
        raise ValueError("act.dat does not contain the expected activity columns")

    t_data = act_data[0]
    if time_interval is None:
        t_start = t_data[0]
        t_end = t_data[-1]
    else:
        if not isinstance(time_interval, (list, tuple)) or len(time_interval) != 2:
            raise ValueError("time_interval must be None or a (start, end) pair")
        t_start, t_end = time_interval
        if t_start is None:
            t_start = t_data[0]
        if t_end is None:
            t_end = t_data[-1]
        if t_end < t_start:
            raise ValueError("time_interval end must not be less than start")

    data_seg = (t_data >= t_start) & (t_data <= t_end)
    if not np.any(data_seg):
        raise ValueError("time_interval does not overlap the act.dat time range")

    selected_indices = [cell_names.index(cell) + 1 for cell in selected_cells]
    selected_data = act_data[selected_indices][:, data_seg]
    t_plot = t_data[data_seg]

    fig_height = max(4.0, 1.1 + 0.22 * len(selected_cells))
    fig, axs = plt.subplots(2, 1, figsize=(10, fig_height), sharex=True)

    for cell_name, row in zip(selected_cells, selected_data):
        axs[0].plot(t_plot, row, linewidth=0.8, label=cell_name)
    axs[0].set_ylabel("Activity")
    axs[0].legend(
        loc="upper right",
        fontsize="small",
        ncol=max(1, min(4, len(selected_cells))),
    )

    extent = [t_plot[0], t_plot[-1], 0, len(selected_cells)]
    axs[1].imshow(
        selected_data,
        aspect="auto",
        interpolation="nearest",
        extent=extent,
    )
    axs[1].set_yticks(np.arange(len(selected_cells)) + 0.5)
    axs[1].set_yticklabels(selected_cells)
    axs[1].set_xlabel("Time (s)")
    axs[1].set_ylabel("Cell")

    title = (
        cells_or_class
        if isinstance(cells_or_class, str)
        else "{} selected cells".format(len(selected_cells))
    )
    axs[0].set_title("Activity: {}".format(title))
    fig.tight_layout()

    if save_png:
        fig.savefig(os.path.join(output_folder, filename), bbox_inches="tight", dpi=300)

    return fig


def plot_cell_connections(
    output_folder,
    cell_names,
    save_png=False,
    filename="CellConnections.png",
    hide_self_connections=True,
    primary_rotation=0.0,
    secondary_rotation=None,
    external_rotation=None,
    symbol_text_scale=1.0,
    node_scale=None,
    text_scale=None,
):
    """Return a figure showing selected cells and connected model objects."""
    from matplotlib.lines import Line2D
    from matplotlib.patches import FancyArrowPatch

    worm_file = os.path.join(output_folder, "worm_data_worm.json")
    if not os.path.isfile(worm_file):
        raise FileNotFoundError(
            "Could not find worm_data_worm.json in {}".format(output_folder)
        )
    if isinstance(cell_names, str):
        raise TypeError("cell_names must be a list or tuple of cell names")
    if not isinstance(cell_names, (list, tuple)) or not cell_names:
        raise ValueError("cell_names must be a non-empty list or tuple")

    selected_cells = [str(cell_name) for cell_name in cell_names]
    if len(set(selected_cells)) != len(selected_cells):
        raise ValueError("cell_names contains duplicate entries")
    if (
        isinstance(symbol_text_scale, bool)
        or not isinstance(symbol_text_scale, (int, float))
        or symbol_text_scale <= 0
    ):
        raise ValueError("symbol_text_scale must be a positive number")
    if node_scale is None:
        node_scale = symbol_text_scale
    if text_scale is None:
        text_scale = symbol_text_scale
    for scale_name, scale_value in (
        ("node_scale", node_scale),
        ("text_scale", text_scale),
    ):
        if (
            isinstance(scale_value, bool)
            or not isinstance(scale_value, (int, float))
            or scale_value <= 0
        ):
            raise ValueError("{} must be a positive number".format(scale_name))

    network_json_data = utils.getJsonFile(worm_file)
    nervous_system = network_json_data.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")

    json_cell_names = _json_value(nervous_system.get("cell_names"), [])
    if not isinstance(json_cell_names, list):
        raise ValueError("'nervous_system.cell_names.value' must be a list")
    missing_cells = [
        cell_name for cell_name in selected_cells if cell_name not in json_cell_names
    ]
    if missing_cells:
        raise ValueError("Unknown cell name(s): {}".format(", ".join(missing_cells)))

    selected_set = set(selected_cells)

    def connection_weight(connection):
        weight = _json_value(connection.get("weight"), 1.0)
        if isinstance(weight, bool) or not isinstance(weight, (int, float)):
            return 1.0
        return float(weight)

    def connection_list(connection_key):
        connection_object = nervous_system.get(connection_key, {})
        if connection_object is None:
            return []
        if not isinstance(connection_object, dict):
            raise TypeError(
                "'nervous_system.{}' must be a dictionary".format(connection_key)
            )
        connections = connection_object.get("value", [])
        if connections is None:
            return []
        if not isinstance(connections, list):
            raise TypeError(
                "'nervous_system.{}.value' must be a list".format(connection_key)
            )
        return connections

    nodes = {cell_name: {"type": "cell"} for cell_name in selected_cells}
    edges = []

    def add_node(node_name, node_type):
        nodes.setdefault(str(node_name), {"type": node_type})

    def add_edge(from_node, to_node, weight, edge_type, bidirectional=False):
        if hide_self_connections and str(from_node) == str(to_node):
            return
        edges.append(
            {
                "from": str(from_node),
                "to": str(to_node),
                "weight": float(weight),
                "type": edge_type,
                "bidirectional": bidirectional,
            }
        )

    cell_name_set = set(json_cell_names)
    secondary_cells = []

    def add_secondary_cell(cell_name):
        if cell_name in selected_set or cell_name not in cell_name_set:
            return
        if cell_name not in nodes:
            nodes[cell_name] = {"type": "secondary cell"}
            secondary_cells.append(cell_name)

    for connection in connection_list("chemical_conns"):
        if (
            isinstance(connection, dict)
            and connection.get("from") in cell_name_set
            and connection.get("to") in cell_name_set
            and (
                connection.get("from") in selected_set
                or connection.get("to") in selected_set
            )
        ):
            add_secondary_cell(connection["from"])
            add_secondary_cell(connection["to"])
            add_edge(
                connection["from"],
                connection["to"],
                connection_weight(connection),
                "chemical",
            )

    for connection in connection_list("electrical_conns"):
        if (
            isinstance(connection, dict)
            and connection.get("from") in cell_name_set
            and connection.get("to") in cell_name_set
            and (
                connection.get("from") in selected_set
                or connection.get("to") in selected_set
            )
        ):
            add_secondary_cell(connection["from"])
            add_secondary_cell(connection["to"])
            add_edge(
                connection["from"],
                connection["to"],
                connection_weight(connection),
                "electrical",
                bidirectional=True,
            )

    stretch_receptor = network_json_data.get("stretch_receptor")
    if isinstance(stretch_receptor, dict):
        for weights_key, prefix in (("ns_d_weights", "D_SR"), ("ns_v_weights", "V_SR")):
            weights = stretch_receptor.get(weights_key, {}).get("value", [])
            if weights is None:
                continue
            if not isinstance(weights, list):
                raise TypeError(
                    "'stretch_receptor.{}.value' must be a list".format(weights_key)
                )
            for connection in weights:
                if not isinstance(connection, dict):
                    continue
                to_cell = connection.get("to_ns")
                if to_cell not in selected_set:
                    continue
                from_node = "{}_{}".format(prefix, connection.get("from_sr"))
                add_node(from_node, "stretch receptor")
                add_edge(
                    from_node,
                    to_cell,
                    connection_weight(connection),
                    "stretch receptor",
                )

    sensors = network_json_data.get("sensors")
    if isinstance(sensors, dict):
        for sensor_name, sensor in sensors.items():
            if not isinstance(sensor, dict):
                continue
            weights = sensor.get("weights", {}).get("value", [])
            if weights is None:
                continue
            if not isinstance(weights, list):
                raise TypeError(
                    "'sensors.{}.weights.value' must be a list".format(sensor_name)
                )
            for connection in weights:
                if not isinstance(connection, dict):
                    continue
                to_cell = connection.get("to_cell")
                if to_cell not in selected_set:
                    continue
                from_output = connection.get(
                    "from_output", connection.get("from_input")
                )
                from_node = "{}_output_{}".format(sensor_name, from_output)
                add_node(from_node, "sensor")
                add_edge(
                    from_node,
                    to_cell,
                    connection_weight(connection),
                    "sensor",
                )

    driving_inputs = network_json_data.get("driving_inputs")
    if isinstance(driving_inputs, dict):
        weights = driving_inputs.get("weights", {}).get("value", [])
        if weights is not None:
            if not isinstance(weights, list):
                raise TypeError("'driving_inputs.weights.value' must be a list")
            for connection in weights:
                if not isinstance(connection, dict):
                    continue
                to_cell = connection.get("to_cell")
                if to_cell not in selected_set:
                    continue
                from_node = "input_{}".format(connection.get("from_input"))
                add_node(from_node, "driving input")
                add_edge(
                    from_node,
                    to_cell,
                    connection_weight(connection),
                    "driving input",
                )

    for nmj_key, prefix in (("dorsal_nmj", "D_musc"), ("ventral_nmj", "V_musc")):
        nmj = network_json_data.get(nmj_key)
        if not isinstance(nmj, dict):
            continue
        weights = nmj.get("weights", {}).get("value", [])
        if weights is None:
            continue
        if not isinstance(weights, list):
            raise TypeError("'{}.weights.value' must be a list".format(nmj_key))
        for connection in weights:
            if not isinstance(connection, dict):
                continue
            from_cell = connection.get("from_cell")
            if from_cell not in selected_set:
                continue
            to_node = "{}_{}".format(prefix, connection.get("to_musc"))
            add_node(to_node, "muscle")
            add_edge(
                from_cell,
                to_node,
                connection_weight(connection),
                "muscle",
            )

    displayed_weights = [abs(edge["weight"]) for edge in edges]
    max_abs_weight = max(displayed_weights) if displayed_weights else 1.0
    if max_abs_weight <= 0:
        max_abs_weight = 1.0

    def connection_width(edge):
        scaled = abs(edge["weight"]) / max_abs_weight
        return 0.6 + 3.4 * scaled

    fig_size = max(6.0, 0.35 * len(nodes) + 3.5)
    fig, ax = plt.subplots(figsize=(fig_size, fig_size))
    ax.set_aspect("equal")
    ax.axis("off")

    primary_rotation_rad = np.deg2rad(primary_rotation)
    angles = (
        np.linspace(0, 2 * np.pi, len(selected_cells), endpoint=False)
        + primary_rotation_rad
    )
    positions = {
        cell_name: np.array([np.cos(angle), np.sin(angle)])
        for cell_name, angle in zip(selected_cells, angles)
    }
    if secondary_cells:
        if secondary_rotation is None:
            secondary_rotation_rad = primary_rotation_rad + np.pi / len(selected_cells)
        else:
            secondary_rotation_rad = np.deg2rad(secondary_rotation)
        secondary_angles = (
            np.linspace(0, 2 * np.pi, len(secondary_cells), endpoint=False)
            + secondary_rotation_rad
        )
        for cell_name, angle in zip(secondary_cells, secondary_angles):
            positions[cell_name] = np.array(
                [1.35 * np.cos(angle), 1.35 * np.sin(angle)]
            )
    external_nodes = [
        node_name
        for node_name in nodes
        if nodes[node_name]["type"] not in ("cell", "secondary cell")
    ]
    if external_nodes:
        if external_rotation is None:
            external_rotation_rad = primary_rotation_rad
        else:
            external_rotation_rad = np.deg2rad(external_rotation)
        external_angles = (
            np.linspace(0, 2 * np.pi, len(external_nodes), endpoint=False)
            + external_rotation_rad
        )
        for node_name, angle in zip(external_nodes, external_angles):
            positions[node_name] = np.array(
                [1.85 * np.cos(angle), 1.85 * np.sin(angle)]
            )

    marker_area_scale = float(node_scale) ** 2
    arrow_mutation_scale = 12 * max(1.0, min(float(node_scale), 1.6))
    node_styles = {
        "cell": {
            "marker": "o",
            "size": 850,
            "facecolor": "white",
            "edgecolor": "black",
        },
        "secondary cell": {
            "marker": "o",
            "size": 720,
            "facecolor": "#E8E8E8",
            "edgecolor": "#555555",
        },
        "stretch receptor": {
            "marker": "s",
            "size": 650,
            "facecolor": "#F1E5A6",
            "edgecolor": "#806000",
        },
        "sensor": {
            "marker": "D",
            "size": 650,
            "facecolor": "#B6E3C6",
            "edgecolor": "#26733D",
        },
        "driving input": {
            "marker": "h",
            "size": 700,
            "facecolor": "#D9C6F2",
            "edgecolor": "#634197",
        },
        "muscle": {
            "marker": "^",
            "size": 700,
            "facecolor": "#F4B6B6",
            "edgecolor": "#A33A3A",
        },
    }

    def node_shrink_points(node_name):
        node_type = nodes[node_name]["type"]
        style = node_styles.get(node_type, node_styles["cell"])
        marker_diameter = math.sqrt(style["size"] * marker_area_scale)
        shape_margin = 0.58 if style["marker"] in ("D", "s", "^", "h") else 0.46
        return marker_diameter * shape_margin + 1.5 * max(1.0, float(node_scale))

    def edge_color(edge):
        if edge["type"] == "electrical":
            return "#4C78A8" if edge["weight"] >= 0 else "#7570B3"
        return "#D95F02" if edge["weight"] >= 0 else "#7570B3"

    def draw_edge(edge, rad, alpha=0.85):
        from_cell = edge["from"]
        to_cell = edge["to"]
        linewidth = connection_width(edge)
        color = edge_color(edge)
        arrowstyle = "<->" if edge["bidirectional"] else "-|>"
        linestyle = "--" if edge["type"] == "electrical" else "-"
        if from_cell == to_cell:
            center = positions[from_cell]
            loop_radius = 0.13 * max(1.0, float(node_scale))
            loop = FancyArrowPatch(
                center + np.array([0.0, loop_radius]),
                center + np.array([loop_radius, 0.0]),
                arrowstyle=arrowstyle,
                mutation_scale=arrow_mutation_scale,
                connectionstyle="arc3,rad=1.2",
                linewidth=linewidth,
                linestyle=linestyle,
                color=color,
                alpha=alpha,
                shrinkA=node_shrink_points(from_cell),
                shrinkB=node_shrink_points(to_cell),
            )
            ax.add_patch(loop)
            return

        arrow = FancyArrowPatch(
            positions[from_cell],
            positions[to_cell],
            arrowstyle=arrowstyle,
            mutation_scale=arrow_mutation_scale,
            connectionstyle="arc3,rad={}".format(rad),
            linewidth=linewidth,
            linestyle=linestyle,
            color=color,
            alpha=alpha,
            shrinkA=node_shrink_points(from_cell),
            shrinkB=node_shrink_points(to_cell),
        )
        ax.add_patch(arrow)

    for index, edge in enumerate(edges):
        rad = 0.09 if index % 2 == 0 else -0.09
        if edge["type"] == "electrical":
            rad = -0.08
        draw_edge(edge, rad=rad, alpha=0.78 if edge["type"] == "electrical" else 0.85)

    def display_node_name(node_name, node_type):
        if node_type == "sensor":
            return str(node_name).replace("_output_", "_")
        return node_name

    for node_name, position in positions.items():
        node_type = nodes[node_name]["type"]
        style = node_styles.get(node_type, node_styles["cell"])
        ax.scatter(
            [position[0]],
            [position[1]],
            s=style["size"] * marker_area_scale,
            marker=style["marker"],
            facecolors=style["facecolor"],
            edgecolors=style["edgecolor"],
            linewidths=1.2,
            zorder=3,
        )
        ax.text(
            position[0],
            position[1],
            display_node_name(node_name, node_type),
            ha="center",
            va="center",
            fontsize=(8 if len(nodes) <= 16 else 6) * text_scale,
            zorder=4,
        )

    edge_legend = [
        Line2D([0], [0], color="#D95F02", linewidth=1.8, label="Positive"),
        Line2D([0], [0], color="#7570B3", linewidth=1.8, label="Negative"),
        Line2D(
            [0],
            [0],
            color="#4C78A8",
            linewidth=1.8,
            linestyle="--",
            label="Electrical",
        ),
        Line2D([0], [0], color="0.25", linewidth=0.8, label="Weak"),
        Line2D([0], [0], color="0.25", linewidth=4.0, label="Strong"),
    ]
    node_legend = [
        Line2D(
            [0],
            [0],
            marker=style["marker"],
            color="none",
            markerfacecolor=style["facecolor"],
            markeredgecolor=style["edgecolor"],
            markersize=8 * node_scale,
            label=node_type.title(),
        )
        for node_type, style in node_styles.items()
        if any(nodes[node]["type"] == node_type for node in nodes)
    ]
    ax.legend(
        handles=edge_legend + node_legend,
        loc="upper left",
        bbox_to_anchor=(1.02, 1.0),
        frameon=False,
        fontsize=8 * text_scale,
        ncol=2 if len(nodes) > 12 else 1,
    )
    ax.set_title(
        "{} selected cells, {} displayed connections".format(
            len(selected_cells),
            len(edges),
        ),
        fontsize=12 * text_scale,
    )
    ax.set_xlim(-2.2, 2.2)
    ax.set_ylim(-2.2, 2.2)
    fig.tight_layout()

    if save_png:
        fig.savefig(os.path.join(output_folder, filename), bbox_inches="tight", dpi=300)

    return fig


def plot_json_structure(
    output_folder,
    save_png=False,
    filename="JsonStructure.png",
):
    """Return a schematic figure of the main connected JSON model objects."""
    from matplotlib.patches import FancyArrowPatch, FancyBboxPatch

    worm_file = os.path.join(output_folder, "worm_data_worm.json")
    if not os.path.isfile(worm_file):
        raise FileNotFoundError(
            "Could not find worm_data_worm.json in {}".format(output_folder)
        )

    network_json_data = utils.getJsonFile(worm_file)
    nervous_system = network_json_data.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")

    cell_names = _json_value(nervous_system.get("cell_names"), [])
    if not isinstance(cell_names, list):
        cell_names = []

    def values_list(section, key="value"):
        if not isinstance(section, dict):
            return []
        values = section.get(key, [])
        if values is None:
            return []
        if isinstance(values, dict) and "value" in values:
            values = values["value"]
        return values if isinstance(values, list) else []

    def weight_count(section, key="weights"):
        if not isinstance(section, dict):
            return 0
        return len(values_list(section.get(key, {})))

    def wrap_cell_names(names, columns=4):
        if not names:
            return "No cell names found"
        rows = int(math.ceil(len(names) / columns))
        lines = []
        for row in range(rows):
            row_names = names[row::rows]
            lines.append("   ".join(row_names))
        return "\n".join(lines)

    def class_label(class_name):
        class_name = str(class_name)
        normalised = _normalise_cell_class_name(class_name)
        labels = {
            "vnc": "VNC",
            "head": "Head neurons",
            "interneuron": "Interneurons",
        }
        return labels.get(normalised, class_name.replace("_", " ").title())

    def box_text(title, body_lines):
        if isinstance(body_lines, str):
            body = body_lines
        else:
            body = "\n".join(str(line) for line in body_lines if line is not None)
        return "{}\n{}".format(title, body).strip()

    sr = network_json_data.get("stretch_receptor")
    sensors = network_json_data.get("sensors")
    # environments = network_json_data.get("environments")
    dorsal_nmj = network_json_data.get("dorsal_nmj")
    ventral_nmj = network_json_data.get("ventral_nmj")
    driving_inputs = network_json_data.get("driving_inputs")

    chemical_count = len(values_list(nervous_system.get("chemical_conns", {})))
    electrical_count = len(values_list(nervous_system.get("electrical_conns", {})))
    cells = nervous_system.get("cells", {})
    if not isinstance(cells, dict):
        cells = {}
    cell_class_groups = []
    for cell_name in cell_names:
        cell = cells.get(cell_name, {})
        cell_class = _json_value(cell.get("cell_class"), "uncategorized")
        if not cell_class_groups or cell_class_groups[-1]["class"] != cell_class:
            cell_class_groups.append({"class": cell_class, "cells": []})
        cell_class_groups[-1]["cells"].append(cell_name)

    ns_text = box_text(
        "nervous_system",
        [
            "{} cells".format(len(cell_names)),
            "{} chemical, {} electrical conns".format(chemical_count, electrical_count),
        ],
    )

    boxes = {
        "nervous_system": {
            "xy": (0.27, 0.08),
            "wh": (0.40, 0.56),
            "text": ns_text,
            "facecolor": "#F7F7F7",
            "cell_class_groups": cell_class_groups,
        }
    }

    if isinstance(dorsal_nmj, dict):
        boxes["dorsal_muscles"] = {
            "xy": (0.72, 0.43),
            "wh": (0.16, 0.14),
            "text": box_text(
                "dorsal_nmj\nDorsal muscles",
                ["{} weights".format(weight_count(dorsal_nmj))],
            ),
            "facecolor": "#F4B6B6",
        }
    if isinstance(ventral_nmj, dict):
        boxes["ventral_muscles"] = {
            "xy": (0.72, 0.15),
            "wh": (0.16, 0.14),
            "text": box_text(
                "ventral_nmj\nVentral muscles",
                ["{} weights".format(weight_count(ventral_nmj))],
            ),
            "facecolor": "#F4C7B6",
        }
    if isinstance(sr, dict):
        ns_d = len(values_list(sr.get("ns_d_weights", {})))
        ns_v = len(values_list(sr.get("ns_v_weights", {})))
        d_body = len(values_list(sr.get("d_weights", {})))
        v_body = len(values_list(sr.get("v_weights", {})))
        boxes["stretch_receptor"] = {
            "xy": (0.04, 0.62),
            "wh": (0.18, 0.18),
            "text": box_text(
                "stretch_receptor",
                [
                    "{} dorsal-to-NS weights".format(ns_d),
                    "{} ventral-to-NS weights".format(ns_v),
                ],
            ),
            "facecolor": "#F1E5A6",
        }
        if d_body or v_body:
            boxes["body_segments"] = {
                "xy": (0.58, 0.69),
                "wh": (0.16, 0.11),
                "text": box_text(
                    "body segments",
                    [
                        "{} dorsal SR weights".format(d_body),
                        "{} ventral SR weights".format(v_body),
                    ],
                ),
                "facecolor": "#E7D8B9",
            }
    if isinstance(sensors, dict) and sensors:
        sensors_by_environment = {}
        for sensor_name, sensor in sensors.items():
            if not isinstance(sensor, dict):
                continue
            environment_name = _json_value(sensor.get("environment"))
            if environment_name is not None:
                environment_name = str(environment_name)
            else:
                environment_name = "unassigned"
            sensors_by_environment.setdefault(environment_name, []).append(
                (sensor_name, weight_count(sensor))
            )
        sensor_entries = []
        for environment_name in sorted(sensors_by_environment):
            sensor_summaries = []
            for sensor_name, _sensor_weights in sorted(
                sensors_by_environment[environment_name]
            ):
                sensor = sensors.get(sensor_name, {})
                output_count = len(values_list(sensor.get("outputs", {})))
                sensor_summaries.append(
                    "{}: {} outputs".format(sensor_name, output_count)
                )
            sensor_entries.append(
                {
                    "id": environment_name,
                    "title": environment_name,
                    "lines": sensor_summaries,
                }
            )
        sensor_box_h = (
            min(0.26, max(0.17, 0.07 + 0.075 * len(sensor_entries)))
            if len(sensor_entries) > 1
            else 0.17
        )
        boxes["sensors"] = {
            "xy": (0.04, 0.25),
            "wh": (0.18, sensor_box_h),
            "text": box_text(
                "environments",
                "" if len(sensor_entries) > 1 else sensor_entries[0]["lines"],
            ),
            "facecolor": "#B6E3C6",
            "sub_boxes": sensor_entries if len(sensor_entries) > 1 else [],
        }
    if isinstance(driving_inputs, dict) and driving_inputs:
        boxes["driving_inputs"] = {
            "xy": (0.28, 0.04),
            "wh": (0.18, 0.10),
            "text": box_text(
                "driving_inputs",
                ["{} weights".format(weight_count(driving_inputs))],
            ),
            "facecolor": "#D9C6F2",
        }

    fig, ax = plt.subplots(figsize=(13, 8))
    min_x = min(spec["xy"][0] for spec in boxes.values())
    max_x = max(spec["xy"][0] + spec["wh"][0] for spec in boxes.values())
    min_y = min(spec["xy"][1] for spec in boxes.values())
    max_y = max(spec["xy"][1] + spec["wh"][1] for spec in boxes.values())
    pad = 0.025
    ax.set_xlim(min_x - pad, max_x + pad)
    ax.set_ylim(min_y - pad, max_y + pad)
    ax.axis("off")

    def center(name):
        x, y = boxes[name]["xy"]
        w, h = boxes[name]["wh"]
        return np.array([x + w / 2.0, y + h / 2.0])

    sub_box_centers = {}

    def add_box(name):
        spec = boxes[name]
        x, y = spec["xy"]
        w, h = spec["wh"]
        patch = FancyBboxPatch(
            (x, y),
            w,
            h,
            boxstyle="round,pad=0.012",
            facecolor=spec["facecolor"],
            edgecolor="#333333",
            linewidth=1.2,
        )
        ax.add_patch(patch)
        if name != "nervous_system":
            sub_boxes = spec.get("sub_boxes", [])
            if sub_boxes:
                title_position = spec.get("sub_box_title_position", "top")
                title_y = y + 0.018 if title_position == "bottom" else y + h - 0.018
                ax.text(
                    x + w / 2.0,
                    title_y,
                    spec["text"],
                    ha="center",
                    va="bottom" if title_position == "bottom" else "top",
                    fontsize=12,
                    fontweight="bold",
                )
                inner_x = x + 0.015
                inner_w = w - 0.03
                inner_top = y + h - (0.018 if title_position == "bottom" else 0.060)
                inner_bottom = y + (0.060 if title_position == "bottom" else 0.018)
                gap = 0.010
                sub_h = (inner_top - inner_bottom - gap * (len(sub_boxes) - 1)) / len(
                    sub_boxes
                )
                sub_colors = ["#EAF5D8", "#EEF6E8", "#F5F1D8", "#E7F1E4"]
                for sub_index, sub_box in enumerate(sub_boxes):
                    sy = inner_top - (sub_index + 1) * sub_h - sub_index * gap
                    sub_patch = FancyBboxPatch(
                        (inner_x, sy),
                        inner_w,
                        sub_h,
                        boxstyle="round,pad=0.004",
                        facecolor=sub_colors[sub_index % len(sub_colors)],
                        edgecolor="#777777",
                        linewidth=0.8,
                    )
                    ax.add_patch(sub_patch)
                    sub_id = sub_box.get("id", sub_box.get("title", sub_index))
                    sub_box_centers[(name, sub_id)] = np.array(
                        [inner_x + inner_w / 2.0, sy + sub_h / 2.0]
                    )
                    sub_box_centers[(name, sub_id, "left")] = np.array(
                        [inner_x + inner_w * 0.25, sy + sub_h / 2.0]
                    )
                    sub_box_centers[(name, sub_id, "right")] = np.array(
                        [inner_x + inner_w * 0.75, sy + sub_h / 2.0]
                    )
                    sub_box_centers[(name, sub_id, "top_left")] = np.array(
                        [inner_x + inner_w * 0.32, sy + sub_h * 0.92]
                    )
                    sub_box_centers[(name, sub_id, "top_right")] = np.array(
                        [inner_x + inner_w * 0.68, sy + sub_h * 0.92]
                    )
                    sub_box_centers[(name, sub_id, "bottom_left")] = np.array(
                        [inner_x + inner_w * 0.32, sy + sub_h * 0.08]
                    )
                    sub_box_centers[(name, sub_id, "bottom_right")] = np.array(
                        [inner_x + inner_w * 0.68, sy + sub_h * 0.08]
                    )
                    lines = sub_box.get("lines", [])
                    text = str(sub_box.get("title", ""))
                    if lines:
                        text += "\n" + "\n".join(str(line) for line in lines)
                    ax.text(
                        inner_x + inner_w / 2.0,
                        sy + sub_h / 2.0,
                        text,
                        ha="center",
                        va="center",
                        fontsize=9.5 if len(lines) > 2 else 11,
                    )
                return
            ax.text(
                x + w / 2.0,
                y + h / 2.0,
                spec["text"],
                ha="center",
                va="center",
                fontsize=13,
            )
            return

        ax.text(
            x + w / 2.0,
            y + h - 0.025,
            spec["text"],
            ha="center",
            va="top",
            fontsize=12,
            fontweight="bold",
        )
        groups = spec.get("cell_class_groups", [])
        if not groups:
            return

        inner_x = x + 0.025
        inner_w = w - 0.05
        inner_top = y + h - 0.105
        inner_bottom = y + 0.025
        gap = 0.012
        class_h = (inner_top - inner_bottom - gap * (len(groups) - 1)) / len(groups)
        class_colors = ["#EAF2FA", "#EEF6E8", "#F9EFE4", "#F2EAF7", "#F4F4E6"]
        for group_index, group in enumerate(groups):
            gy = inner_top - (group_index + 1) * class_h - group_index * gap
            class_patch = FancyBboxPatch(
                (inner_x, gy),
                inner_w,
                class_h,
                boxstyle="round,pad=0.006",
                facecolor=class_colors[group_index % len(class_colors)],
                edgecolor="#777777",
                linewidth=0.8,
            )
            ax.add_patch(class_patch)
            group_cells = group["cells"]
            columns = 6 if len(group_cells) > 12 else 3
            text = "{} ({})\n{}".format(
                class_label(group["class"]),
                len(group_cells),
                wrap_cell_names(group_cells, columns=columns),
            )
            ax.text(
                inner_x + inner_w / 2.0,
                gy + class_h / 2.0,
                text,
                ha="center",
                va="center",
                fontsize=10.5 if len(group_cells) > 16 else 12,
            )

    def add_arrow_between_points(
        start,
        end,
        label=None,
        start_trim=0.07,
        end_trim=0.07,
        rad=0.05,
    ):
        direction = end - start
        distance = np.linalg.norm(direction)
        if distance == 0:
            return
        unit = direction / distance
        start = start + start_trim * unit
        end = end - end_trim * unit
        arrow = FancyArrowPatch(
            start,
            end,
            arrowstyle="-|>",
            mutation_scale=14,
            linewidth=1.4,
            color="#333333",
            connectionstyle="arc3,rad={}".format(rad),
        )
        ax.add_patch(arrow)
        if label:
            midpoint = (start + end) / 2.0
            ax.text(
                midpoint[0],
                midpoint[1],
                label,
                ha="center",
                va="center",
                fontsize=10.5,
                bbox={"facecolor": "white", "edgecolor": "none", "alpha": 0.75},
            )

    def add_arrow(from_name, to_name, label=None):
        if from_name not in boxes or to_name not in boxes:
            return
        add_arrow_between_points(
            center(from_name),
            center(to_name),
            label=label,
            start_trim=0.15 if from_name == "nervous_system" else 0.07,
            end_trim=0.15 if to_name == "nervous_system" else 0.07,
        )

    for name in boxes:
        add_box(name)

    add_arrow("nervous_system", "dorsal_muscles", "dorsal_nmj")
    add_arrow("nervous_system", "ventral_muscles", "ventral_nmj")
    add_arrow("dorsal_muscles", "body_segments", "body force")
    add_arrow("ventral_muscles", "body_segments", "body force")
    add_arrow("body_segments", "stretch_receptor", "body-to-SR")
    add_arrow("stretch_receptor", "nervous_system", "SR-to-NS")
    add_arrow("sensors", "nervous_system", "sensor outputs")
    add_arrow("driving_inputs", "nervous_system", "inputs")

    fig.subplots_adjust(left=0, right=1, bottom=0, top=1)

    if save_png:
        fig.savefig(
            os.path.join(output_folder, filename),
            bbox_inches="tight",
            pad_inches=0.02,
            dpi=300,
        )

    return fig


def normalize_evolvable_range_entries(evolvable_ranges, evolved_used_order=None):
    entries = []
    if evolvable_ranges is None:
        return entries
    if evolved_used_order is None:
        evolved_used_order = []

    def evotag_number(evotag):
        if isinstance(evotag, int):
            return evotag
        if isinstance(evotag, str) and evotag.startswith("evotag_"):
            return int(evotag.replace("evotag_", "", 1))
        return evotag

    for entry in evolvable_ranges.get("value", []):
        if "evotag" in entry:
            entry = dict(entry)
            evotag_key = entry["evotag"]
            entry["evotag"] = evotag_number(entry["evotag"])
            entry["evotag_key"] = str(evotag_key)
            entries.append(entry)
            continue

        if len(entry) != 1:
            continue

        evotag_key, attrs = next(iter(entry.items()))

        attrs = dict(attrs)
        attrs["evotag"] = evotag_number(evotag_key)
        attrs["evotag_key"] = str(evotag_key)
        entries.append(attrs)

    for evotag_key, attrs in evolvable_ranges.items():
        if evotag_key == "value" or not isinstance(attrs, dict):
            continue

        attrs = dict(attrs)
        attrs["evotag"] = evotag_number(evotag_key)
        attrs["evotag_key"] = str(evotag_key)
        entries.append(attrs)

    if evolved_used_order:
        order = {tag: ind for ind, tag in enumerate(evolved_used_order)}
        entries.sort(key=lambda entry: order.get(entry.get("evotag_key"), len(order)))

    return entries


sys.path.append("..")

# import random


def distinct_30_colors():
    # Take colours from qualitative colormaps
    c1 = plt.get_cmap("tab20").colors  # 20 colours
    c2 = plt.get_cmap("tab20b").colors  # 20 colours
    c3 = plt.get_cmap("tab20c").colors  # 20 colours
    c4 = plt.get_cmap("tab10").colors

    # Pick a varied subset
    colors = list(c1[:20]) + list(c2[:5]) + list(c3[:5])

    # Reorder to spread related shades apart
    order = [
        0,
        20,
        5,
        25,
        10,
        1,
        15,
        21,
        6,
        26,
        11,
        2,
        16,
        22,
        7,
        27,
        12,
        3,
        17,
        23,
        8,
        28,
        13,
        4,
        18,
        24,
        9,
        29,
        14,
        19,
    ]

    return list(c4) + [colors[i] for i in order]


def distinct_colors_100(n):
    hues = np.linspace(0, 1, n, endpoint=False)
    vals = [0.85, 0.65]  # alternate brightness
    sats = [0.9, 0.75]  # alternate saturation

    colors = []
    for i, h in enumerate(hues):
        colors.append(mcolors.hsv_to_rgb((h, sats[i % 2], vals[i % 2])))
    return colors


def distinct_ordered_hsv_colors(n, step=37, saturation=0.85, value=0.9):
    """
    Generate n colours with hues evenly spaced around the colour wheel,
    then reorder them so neighbouring colours are well separated.

    step must be coprime to n.
    """
    if np.gcd(step, n) != 1:
        raise ValueError("step must be coprime to n")

    # Evenly spaced hues
    hues = np.linspace(0, 1, n, endpoint=False)

    # Reorder indices so neighbours are far apart in hue
    order = [(i * step) % n for i in range(n)]

    return [mcolors.hsv_to_rgb((hues[i], saturation, value)) for i in order]


colors30 = distinct_colors_100(100)
colors30 = distinct_ordered_hsv_colors(100, step=37)
colors30 = distinct_30_colors()

linestyles = ["-", "--", "-.", ":"]
style_cycle = cycler(color=colors30) + cycler(
    linestyle=list(itertools.islice(itertools.cycle(linestyles), len(colors30)))
)
random.seed(1234)

N = 120
colors = colors30 * 3
# colors = [colors30[i % len(colors30)] for i in range(N)]
linestyles = [random.choice(linestyles) for _ in range(N)]

# ax.set_prop_cycle(cycler(color=colors) + cycler(linestyle=linestyles))
style_cycle = cycler(color=colors) + cycler(linestyle=linestyles)

plt.rcParams["axes.prop_cycle"] = style_cycle


def run_main(args=None):
    if args is None:
        args = hf.process_args()
    reload_single_run(a=args)


title_font_size = hf.title_font_size
label_font_size = hf.label_font_size


def getRowsCols(plot_num, plot_cols):
    return int(plot_num / plot_cols), plot_num % plot_cols


def sign(val):
    return (val > 0) * 2.0 - 1.0


def signed_log(val):
    val = np.asarray(val)
    out = np.zeros_like(val, dtype=float)
    mask = np.isfinite(val) & (val != 0)
    out[mask] = np.sign(val[mask]) * np.log(np.abs(val[mask]))
    return out


def safe_ratio_to_initial(evol_data):
    evol_data = np.asarray(evol_data, dtype=float)
    initial = evol_data[0]
    out = np.zeros_like(evol_data, dtype=float)
    np.divide(
        evol_data,
        initial,
        out=out,
        where=np.isfinite(initial) & (initial != 0),
    )
    out[~np.isfinite(out)] = 0.0
    return out


short_phen_names = {
    "Nervous system": "NS",
    "Chemical weights": "ChemWei",
    "Electrical weights": "ElecWei",
    "Stretch receptor": "SR",
    "D inds": "D",
    "V inds": "V",
    "Sensors_Sensor_": "Sen",
    "Driving input": "Dri",
    "NMJ gain map D": "GMapD",
    "NMJ gain map V": "GMapV",
}


def getEvolTrans(evol_data):
    evol_data_diff_1 = safe_ratio_to_initial(evol_data)
    # evol_data_diff_1 = (evol_data - evol_data[0]) / evol_data[0]
    evol_data_diff_11 = signed_log(evol_data_diff_1)
    evol_data_diff_13 = evol_data - evol_data[0]
    evol_data_diff_131 = signed_log(evol_data_diff_13[1:])

    return [evol_data_diff_13, evol_data_diff_131, evol_data_diff_1, evol_data_diff_11]


def plot_phenonames(
    plot_list=["rel_var", "var", ["initial_log", "final_log"], ["initial", "final"]],
    a=None,
):
    file = hf.rename_file("genhistory.dat")
    if not os.path.isfile(file):
        return
    evol_data_all = hf.load_nonragged_arrays(file)
    evol_data_1 = evol_data_all[len(evol_data_all) - 1]  # use only the last array

    # evol_data_1 = np.loadtxt(hf.rename_file("genhistory.dat"))

    worm_file = hf.get_worm_file()

    network_json_data = utils.getJsonFile(worm_file)
    vectsize = network_json_data["Evolutionary Optimization Parameters"]["VectSize"][
        "value"
    ]

    evolvables = normalize_evolvable_range_entries(
        get_evolvable_ranges(network_json_data),
        get_evolved_used_order(network_json_data),
    )
    if evolvables and "name" in evolvables[0]:
        phen_names = []
        phen_tags = []
        phen_nums = []
        for val in evolvables:
            if not (("active" in val) & (not val["active"])):
                name = val["name"]
                for key, val2 in short_phen_names.items():
                    name = name.replace(key, val2)
                phen_names.append(name)
                phen_tags.append(val.get("evotag_key", str(val["evotag"])))
                phen_nums.append(val["evotag"])

    elif "PhenoNames" in network_json_data:
        phen_names = network_json_data["PhenoNames"]["value"]
        phen_tags = phen_names
        phen_nums = network_json_data["PhenoNamesNums"]["value"]
    else:
        print("PhenoNames needed for pheno plot")
        return

    # print("checkDict")
    # print(phen_names)

    if a.modelName == "CO18" or a.modelName == "CO18Full":
        network_json_data_RS18 = utils.getJsonFile(hf.dir_name + "/RS18_worm_data.json")
        phen_names += network_json_data_RS18["PhenoNames"]["value"]
        phen_tags += network_json_data_RS18["PhenoNames"]["value"]
        phen_nums += network_json_data_RS18["PhenoNamesNums"]["value"]

    phen_offset = vectsize * 2
    # phen_size = vectsize

    # avlentop = 1
    # evol_data = getAvData_1(evol_data_1, avlentop=avlentop)
    evol_data = evol_data_1

    # gen_index_orig = evol_data[:, 0]

    # generation number, phenotype number (first is gen index)

    evol_data = evol_data[:, 1 + phen_offset :]

    # evol_data_full_diff = (evol_data[-1] - evol_data[0]) / evol_data[0]

    evol_data_full_diff0 = safe_ratio_to_initial(evol_data)
    evol_data_full_diff = signed_log(evol_data_full_diff0)

    avlentop = 1
    if hasattr(a, "evoAvLen"):
        avlentop = a.evoAvLen

    evol_data_full_diff0 = getAvData_1(evol_data_full_diff0, avlentop=avlentop)
    evol_data_full_diff = getAvData_1(evol_data_full_diff, avlentop=avlentop)
    evol_data_full_diff0 = evol_data_full_diff0[-1] - evol_data_full_diff0[0]
    evol_data_full_diff = evol_data_full_diff[-1] - evol_data_full_diff[0]

    # evol_data_full_diff20 = evol_data - evol_data[0]

    # evol_data_full_diff2 = sign(evol_data_full_diff20) * np.log(
    #    np.abs(evol_data_full_diff20)
    # )

    # evol_data_full_diff_abs = (evol_data[-1] - evol_data[0]) / np.abs(evol_data[0])

    evol_data_log = signed_log(evol_data)
    evol_data_log = getAvData_1(evol_data_log, avlentop=avlentop)

    evol_data_init = evol_data_log[0]

    evol_data_fin = evol_data_log[-1]

    evol_data_av = getAvData_1(evol_data, avlentop=avlentop)
    evol_data_fin_actual = evol_data_av[-1]

    evol_data_init_actual = evol_data_av[0]

    evol_data_dict = {
        "rel_var": {
            "value": evol_data_full_diff0,
            "title": "Perc variation",
            "color": "black",
            "linestyle": "-",
        },
        "var": {
            "value": evol_data_full_diff,
            "title": "Signed log perc var",
            "color": "black",
            "linestyle": "-",
        },
        "final_log": {
            "value": evol_data_fin,
            "title": "Signed log final value",
            "color": "red",
            "linestyle": "--",
        },
        "initial_log": {
            "value": evol_data_init,
            "title": "Signed log initial value",
            "color": "black",
            "linestyle": "-",
        },
        "final": {
            "value": evol_data_fin_actual,
            "title": "Final value",
            "color": "red",
            "linestyle": "--",
        },
        "initial": {
            "value": evol_data_init_actual,
            "title": "Initial value",
            "color": "black",
            "linestyle": "-",
        },
    }

    for data_val in evol_data_dict.values():
        data_val["value"][np.isnan(data_val["value"])] = 0
        data_val["value"][np.isinf(data_val["value"])] = 0

    phen_names_set = sorted(set(phen_names))
    phen_name_list = []

    for phen_name in phen_names_set:
        phen_name_list.append(phen_name)
        # indices = [
        #    phen_nums[ind] - 1 for ind, val in enumerate(phen_names) if val == phen_name
        # ]
        indices = [ind for ind, val in enumerate(phen_names) if val == phen_name]
        for val in evol_data_dict.values():
            if "pheno_avs" not in val:
                val["pheno_avs"] = []
            val["pheno_avs"].append(np.mean(val["value"][indices]))

    # print(phen_name_list)

    fsize_cols, fsize_rows = 10, 10
    fsize_cols_2 = 10 * len(phen_names) / 30
    plot_cols = 1
    plot_rows = len(plot_list)
    fig, axs = plt.subplots(
        plot_rows, plot_cols, figsize=(fsize_cols, fsize_rows), squeeze=False
    )
    fig2, axs2 = plt.subplots(
        plot_rows, plot_cols, figsize=(fsize_cols_2, fsize_rows), squeeze=False
    )

    for ind, val in enumerate(plot_list):
        row_num, col_num = getRowsCols(ind, plot_cols)
        if type(val) is list:
            for val2 in val:
                val3 = evol_data_dict[val2]["pheno_avs"]
                axs[row_num, col_num].plot(
                    range(len(val3)),
                    val3,
                    label=evol_data_dict[val2]["title"],
                    color=evol_data_dict[val2]["color"],
                    linestyle=evol_data_dict[val2]["linestyle"],
                )
                val3 = evol_data_dict[val2]["value"]
                axs2[row_num, col_num].plot(
                    range(len(val3)),
                    val3,
                    label=evol_data_dict[val2]["title"],
                    color=evol_data_dict[val2]["color"],
                    linestyle=evol_data_dict[val2]["linestyle"],
                )
        else:
            val3 = evol_data_dict[val]["pheno_avs"]
            axs[row_num, col_num].plot(
                range(len(val3)),
                val3,
                label=evol_data_dict[val]["title"],
                color=evol_data_dict[val]["color"],
                linestyle=evol_data_dict[val]["linestyle"],
            )
            val3 = evol_data_dict[val]["value"]
            axs2[row_num, col_num].plot(
                range(len(val3)),
                val3,
                label=evol_data_dict[val]["title"],
                color=evol_data_dict[val]["color"],
                linestyle=evol_data_dict[val]["linestyle"],
            )

        axs2[row_num, col_num].set_xticks(range(len(val3)))
        axs[row_num, col_num].set_xticks(range(len(phen_name_list)))
        axs[row_num, col_num].set_xticklabels([""] * len(phen_name_list))
        axs2[row_num, col_num].set_xticklabels([""] * len(val3))

        # axs2[row_num, col_num].tick_params(axis='x', labelbottom=False)
        # axs[row_num, col_num].tick_params(axis='x', labelbottom=False)
        axs[row_num, col_num].legend()
        axs2[row_num, col_num].legend()

    # axs2[row_num, col_num].set_xticks(range(len(val3)))
    # axs[row_num, col_num].set_xticks(range(len(phen_name_list)))

    for axval in [axs, axs2]:
        for ind, val in enumerate(plot_list):
            row_num, col_num = getRowsCols(ind, plot_cols)
            for gridline, color in zip(
                axval[row_num, col_num].get_xgridlines(), cycle(colors30)
            ):
                gridline.set_color(color)
                gridline.set_linewidth(1.5)
                gridline.set_alpha(0.8)
            axval[row_num, col_num].grid(axis="x")
            axval[row_num, col_num].grid(axis="y")
            axval[row_num, col_num].axhline(0, color="0.25", linewidth=1.6, zorder=1)

    axs[row_num, col_num].set_xlabel("Phenotype #", fontsize=label_font_size)
    axs[row_num, col_num].set_xticklabels(phen_name_list, rotation="vertical")
    axs2[row_num, col_num].set_xlabel("Phenotype #", fontsize=label_font_size)
    axs2[row_num, col_num].set_xticklabels(phen_tags, rotation="vertical")
    for tick, color in zip(axs[row_num, col_num].get_xticklabels(), cycle(colors30)):
        tick.set_color(color)
    for tick, color in zip(axs2[row_num, col_num].get_xticklabels(), cycle(colors30)):
        tick.set_color(color)

    fig.tight_layout()
    # fig.subplots_adjust(hspace=0.5)

    filename = hf.rename_file("Evolution_averages.png")
    fig.savefig(filename, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename)
    plt.close(fig)

    fig2.tight_layout()
    # fig.subplots_adjust(hspace=0.5)

    filename = hf.rename_file("Evolution_averages_2.png")
    fig2.savefig(filename, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename)
    plt.close(fig2)


def plot_cols_fig_2(axslist, plot_data, titles, gen_indices, phen_names):
    initial_gen = 0
    final_gen = 1000

    for plot_data_1, title, gen_index, axs in zip(
        plot_data, titles, gen_indices, axslist
    ):
        gen_seg = (gen_index >= initial_gen) & (gen_index < final_gen)
        axs.set_title(title, fontsize=title_font_size)
        for phen, phen_name in enumerate(phen_names):
            axs.plot(
                gen_index[gen_seg],
                plot_data_1[gen_seg, phen],
                label=phen_name,  # + " %i" % (phen),
                # linewidth=0.5,
            )


def plot_cols_fig_1(
    fig, axs, plot_data, titles, gen_indices, phen_names, plot_cols, plot_num
):
    initial_gen = 0
    final_gen = 1000

    for plot_data_1, title, gen_index in zip(plot_data, titles, gen_indices):
        gen_seg = (gen_index >= initial_gen) & (gen_index < final_gen)
        row_num = int(plot_num / plot_cols)
        col_num = plot_num % plot_cols
        axs[row_num, col_num].set_title(title, fontsize=title_font_size)
        for phen, phen_name in enumerate(phen_names):
            axs[row_num, col_num].plot(
                gen_index[gen_seg],
                plot_data_1[gen_seg, phen],
                label=phen_name + " %i" % (phen),
                # linewidth=0.5,
            )

        # axs[row_num, col_num].set_xlabel("Generation", fontsize=label_font_size)
        plot_num += 1

    row_num = int((plot_num - 1) / plot_cols)
    # col_num = (plot_num-1) % plot_cols
    for col_num in range(plot_cols):
        axs[row_num, col_num].set_xlabel("Generation", fontsize=label_font_size)

    fig.subplots_adjust(bottom=0.25)
    handles, labels = axs.flat[1].get_legend_handles_labels()

    fig.legend(
        handles,
        labels,
        loc="lower center",
        ncol=4,  # 4 columns -> 16 items becomes about 4x4
        bbox_to_anchor=(0.5, 0.02),
        frameon=True,
    )


def plot_cols_fig(plot_data, titles, gen_indices, phen_names, plot_cols, filename1):
    # fig, axs = plt.subplots(plot_rows, 2, figsize=(10, plot_rows * 2))

    plot_rows = math.ceil((len(plot_data)) / plot_cols)
    if plot_rows > 1:
        fig, axs = plt.subplots(
            plot_rows,
            plot_cols,
            figsize=(plot_cols * 9 + 2, plot_rows * 3 + 16),
            squeeze=False,
        )
    else:
        fig, axs = plt.subplots(plot_rows, plot_cols, figsize=(10, 5), squeeze=False)

    # phen_range = range(0, phen_size)
    # phen_label = range(1, phen_size + 1)

    plot_cols_fig_1(fig, axs, plot_data, titles, gen_indices, phen_names, plot_cols, 0)

    filename = hf.rename_file(filename1)
    plt.savefig(filename, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename)
    plt.close()


def plot_hist_1(evol_data, phen_size, phen_offset, phen_names, filename1):
    gen_index_orig = evol_data[:, 0]
    gen_index_diff = gen_index_orig[1:]

    # generation number, phenotype number (first is gen index)

    evol_data = evol_data[
        :, 1 + phen_offset :
    ]  # here is the average values across the population

    plot_data = [evol_data] + getEvolTrans(evol_data)

    # plot_data = [evol_data, evol_data_diff_13, evol_data_diff_131, evol_data_diff_1, evol_data_diff_11]
    gen_indices = [
        gen_index_orig,
        gen_index_orig,
        gen_index_diff,
        gen_index_orig,
        gen_index_orig,
    ]
    titles = [
        "Gen History",
        "Gen Hist change",
        "Gen Hist chan log",
        "Gen Hist perc change",
        "Gen Hist perc change log",
    ]

    plot_cols = 2

    plot_cols_fig(plot_data, titles, gen_indices, phen_names, plot_cols, filename1)


def getFileName(filename1):
    file = hf.rename_file(filename1)
    if not os.path.isfile(file):
        hf.file_prefix = None
    file = hf.rename_file(filename1)
    if not os.path.isfile(file):
        print("doPlotEvol is True, fitness.dat file is necessary for evolution plots.")
        return
    return file


def getAvData_1(evol_data_1, avlentop=1):
    if evol_data_1.ndim == 1:
        evol_data_1 = evol_data_1[np.newaxis, :]
    avlen = evol_data_1.shape[0] - 1
    if avlen > avlentop:
        avlen = avlentop
    if avlen == 0:
        avlen = 1

    # print(evol_data_1.shape)
    evol_data = np.zeros((evol_data_1.shape[0] - avlen + 1, evol_data_1.shape[1]))
    for phen in range(evol_data_1.shape[1]):
        evol_data[:, phen] = np.convolve(
            evol_data_1[:, phen], np.ones(avlen) / avlen, mode="valid"
        )

    return evol_data


def getAvData(filename1, avlentop=1):
    evol_data_all = hf.load_nonragged_arrays(filename1)
    evol_data_1 = evol_data_all[len(evol_data_all) - 1]  # use only the last array

    return getAvData_1(evol_data_1, avlentop)


def plot_fit():
    file = getFileName("fitness.dat")
    if file is None:
        return

    evol_data = getAvData(file)

    fig_body, ax_body = plt.subplots(figsize=(5, 5))

    # gen_index_orig = evol_data[:, 0]
    ax_body.plot(evol_data[:, 0], np.log(evol_data[:, (1, 2)]), linewidth=0.5)
    filename = hf.rename_file("FitnessHist.png")
    fig_body.savefig(filename, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename)
    plt.close()


def plot_fig_g(plot_data_3, titles, gen_indices, phen_names, filename1):
    fig_g, axs = plt.subplots(2, 2, figsize=(12, 10), squeeze=False)
    plot_axes = list(axs.flat)

    plot_cols_fig_2(plot_axes, plot_data_3, titles, gen_indices, phen_names)
    axs[1, 0].set_xlabel("Generation", fontsize=label_font_size)
    axs[1, 1].set_xlabel("Generation", fontsize=label_font_size)

    handles, labels = plot_axes[0].get_legend_handles_labels()

    legend_fontsize = 10
    legend = None
    legend_bbox = None
    fig_width = fig_g.get_window_extent().width
    for ncols in range(len(labels), 0, -1):
        legend = fig_g.legend(
            handles,
            labels,
            loc="lower center",
            bbox_to_anchor=(0.5, 0.01),
            ncol=ncols,
            frameon=True,
            fontsize=legend_fontsize,
        )
        fig_g.canvas.draw()
        legend_bbox = legend.get_window_extent(renderer=fig_g.canvas.get_renderer())
        if legend_bbox.width <= fig_width * 0.96:
            break
        legend.remove()

    legend_height = legend_bbox.height / fig_g.get_window_extent().height
    fig_g.subplots_adjust(bottom=legend_height + 0.09)
    fig_g.savefig(filename1, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename1)
    plt.close(fig_g)

    if os.path.basename(filename1) == "EvoHist.png":
        save_evohist_legend_figures(handles, labels, legend_fontsize)


def plot_evohist_fitness(fit_data, filename):
    fig, ax = plt.subplots(figsize=(6, 4))
    gen_index = fit_data[:, 0]
    log_fitness = np.log(fit_data[:, (1, 2)])
    ax.plot(
        gen_index,
        log_fitness[:, 0],
        label="Best phenotype",
        color="black",
        linestyle="-",
    )
    ax.plot(
        gen_index,
        log_fitness[:, 1],
        label="Population fitness",
        color="red",
        linestyle="-",
    )
    ax.set_title("Log Fitness", fontsize=title_font_size)
    ax.set_xlabel("Generation", fontsize=label_font_size)
    ax.legend()
    fig.savefig(filename, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename)
    plt.close(fig)


def save_evohist_legend_figures(handles, labels, legend_fontsize):
    for ncols in [3, 4]:
        fig_leg = plt.figure(figsize=(8, 2.5))
        ax_leg = fig_leg.add_subplot(111)
        ax_leg.axis("off")
        ax_leg.legend(
            handles,
            labels,
            loc="center",
            ncol=ncols,
            frameon=True,
            fontsize=legend_fontsize,
        )
        filename = hf.rename_file("EvoHist_legend_%dcol.png" % ncols)
        fig_leg.savefig(filename, bbox_inches="tight", dpi=300)
        print("Saved plot image to: %s" % filename)
        plt.close(fig_leg)


def plot_hist(a=None):
    file = getFileName("genhistory.dat")
    if file is None:
        return
    evol_data = getAvData(file)
    fit_data = getAvData(getFileName("fitness.dat"))
    plot_evohist_fitness(fit_data, hf.rename_file("EvoHist_fitness.png"))

    # evol_data_all = hf.load_nonragged_arrays(hf.rename_file("genhistory.dat"))
    # evol_data_1 = evol_data_all[len(evol_data_all) - 1]  # use only the last array

    worm_file = hf.get_worm_file()
    network_json_data = utils.getJsonFile(worm_file)
    vectsize = network_json_data["Evolutionary Optimization Parameters"]["VectSize"][
        "value"
    ]

    evolvables = normalize_evolvable_range_entries(
        get_evolvable_ranges(network_json_data),
        get_evolved_used_order(network_json_data),
    )
    if evolvables and "name" in evolvables[0]:
        phen_names = []
        phen_tags = []
        phen_nums = []
        for val in evolvables:
            if not (("active" in val) & (not val["active"])):
                name = val["name"]
                for key, val2 in short_phen_names.items():
                    name = name.replace(key, val2)
                phen_names.append(name)
                phen_tags.append(val.get("evotag_key", str(val["evotag"])))
                phen_nums.append(val["evotag"])
    else:
        print("evolvable_ranges names not found for plot_hist")
        return
    # phen_nums[:] , phen_names[:] = map(list, zip(*sorted(zip(phen_nums, phen_names))))
    # phen names should already be ordered correctly for gene

    phen_offset = vectsize * 2
    phen_size = vectsize

    avlentop = 1
    if hasattr(a, "evoAvLen"):
        avlentop = a.evoAvLen
    gen_index_orig = evol_data[:, 0]
    # gen_index_diff = gen_index_orig[1:]
    # gen_index_orig_av = getAvData_1(gen_index_orig, avlentop = avlentop)
    gen_index_orig_av = getAvData_1(evol_data, avlentop=avlentop)[:, 0]

    # gen_index_orig_av = evol_data_av[:, 0]

    # generation number, phenotype number (first is gen index)

    evol_data_1 = evol_data[
        :, 1 + phen_offset :
    ]  # here is the average values across the population
    plot_data_1 = [evol_data_1] + getEvolTrans(evol_data_1)

    evol_data_1 = evol_data[:, 1 + phen_size :]  # here is the best genotype
    plot_data_2 = [evol_data_1] + getEvolTrans(evol_data_1)

    plot_data_4av = getAvData_1(plot_data_1[4], avlentop=avlentop)
    # gen_index_orig_av = plot_data_4av[:, 0]
    plot_data_3av = getAvData_1(plot_data_1[3], avlentop=avlentop)

    # plot_data_3 = [plot_data_1[0], plot_data_1[3], plot_data_1[4], plot_data_2[3]]
    plot_data_3 = [plot_data_3av, plot_data_4av, plot_data_1[0], plot_data_2[3]]

    # gen_indices = [gen_index_orig, gen_index_orig, gen_index_orig, gen_index_orig]

    gen_indices = [gen_index_orig_av, gen_index_orig_av, gen_index_orig, gen_index_orig]

    titles = [
        "Pop percent variation",
        "Pop signed log perc var",
        "Pop phenotype value",
        "Best fit percent variation",
    ]

    # print("phen names ", phen_names)

    # plot_hist_1(evol_data, phen_size, phen_offset, phen_names, "EvolutionHistory.png")
    # plot_hist_1(evol_data, phen_size, phen_size, phen_names, "EvolutionHistoryMax.png")

    plot_fig_g(
        plot_data_3,
        titles,
        gen_indices,
        phen_tags,
        hf.rename_file("EvoHist.png"),
    )

    phen_names_set = sorted(set(phen_names))
    # phen_names_set = set(phen_names)
    # phen_name_list = []
    # for phen_name in phen_names_set:
    #    phen_name_list.append(phen_name)

    plot_data_3_avs = []
    for plot_data_31 in plot_data_3:
        pdout = []
        for phen_name in phen_names_set:
            indices = [ind for ind, val in enumerate(phen_names) if val == phen_name]
            pdout.append(np.mean(plot_data_31[:, indices], axis=1))
        plot_data_3_avs.append(np.array(pdout).T)

    plot_fig_g(
        plot_data_3_avs,
        titles,
        gen_indices,
        phen_names_set,
        hf.rename_file("EvoHist_av.png"),
    )

    if False:
        plot_cols = 2
        plot_rows = math.ceil((len(plot_data_3) + 1) / plot_cols)
        if plot_rows > 1:
            fig, axs = plt.subplots(
                plot_rows,
                plot_cols,
                figsize=(plot_cols * 9 + 2, plot_rows * 3 + 16),
                squeeze=False,
            )
        else:
            fig, axs = plt.subplots(
                plot_rows, plot_cols, figsize=(10, 5), squeeze=False
            )

        # phen_range = range(0, phen_size)
        # phen_label = range(1, phen_size + 1)

        initial_gen = 0
        final_gen = 1000

        gen_index = gen_index_orig
        gen_seg = (gen_index >= initial_gen) & (gen_index < final_gen)

        axs[0, 0].plot(gen_index[gen_seg], fit_data)
        axs[0, 0].set_title("Fitness", fontsize=title_font_size)

        plot_cols_fig_1(
            fig, axs, plot_data_3, titles, gen_indices, phen_names, plot_cols, 1
        )

        filename = hf.rename_file("EvolutionHistoryMult.png")
        plt.savefig(filename, bbox_inches="tight", dpi=300)
        print("Saved plot image to: %s" % filename)
        plt.close()


def plot_evols(a=None, **kwargs):
    a = hf.build_namespace(hf.DEFAULTS, a, **kwargs)

    hf.setFolder(a)

    mpl.rcParams["xtick.labelsize"] = 12
    mpl.rcParams["ytick.labelsize"] = 12

    gen_file = getFileName("genhistory.dat")
    fit_file = getFileName("fitness.dat")
    if gen_file is None or fit_file is None:
        return
    if len(hf.load_nonragged_arrays(gen_file)) == 0:
        print("No evolution history data found; skipping evolution plots.")
        return
    if len(hf.load_nonragged_arrays(fit_file)) == 0:
        print("No fitness history data found; skipping evolution plots.")
        return

    plot_hist(a=a)
    plot_fit()
    plot_phenonames(a=a)

    return


# def reload_single_run(show_plot=True, verbose=False, plot_format_name=None):
def reload_single_run(a=None, **kwargs):
    a = hf.build_namespace(hf.DEFAULTS, a, **kwargs)

    hf.setFolder(a)

    act_file = hf.rename_file("act.dat")

    if not os.path.isfile(act_file):
        hf.file_prefix = None

    worm_file = hf.get_worm_file()
    # print(worm_file)

    if False:
        worm_file = hf.rename_file("worm_data_evo.json")
        if not os.path.isfile(worm_file):
            worm_file = hf.rename_file("worm_data.json")
        if not os.path.isfile(worm_file):
            worm_file = hf.rename_file("worm_data_worm.json")

    network_json_data = utils.getJsonFile(worm_file)

    main_model_name = None
    if network_json_data is not None:
        main_model_name = utils.getMainModelName(network_json_data)

    if a.modelName == "W2DSR":
        a.modelName = main_model_name or utils.getModelName(network_json_data)

    def imshow_time_extent(t_values, row_count):
        if len(t_values) == 0:
            return [0, 0, 0, row_count]
        return [t_values[0], t_values[-1], 0, row_count]

    def set_imshow_row_ticks(ax, row_count, max_labels=12):
        if row_count <= 0:
            ax.set_yticks([])
            return
        step = max(1, int(math.ceil(row_count / max_labels)))
        rows = np.arange(0, row_count, step)
        ax.set_yticks(rows + 0.5)
        ax.set_yticklabels([str(row + 1) for row in rows])

    # network_json_data = utils.getJsonFile(hf.rename_file("worm_data.json"))

    """ step_size = network_json_data["Evolutionary Optimization Parameters"]["StepSize"][
        "value"
    ]
    skip_steps = network_json_data["Evolutionary Optimization Parameters"][
        "skip_steps"
    ]["value"] """

    mpl.rcParams["xtick.labelsize"] = 12
    mpl.rcParams["ytick.labelsize"] = 12

    act_file = hf.rename_file("act.dat")
    print("Loading activity data from: %s" % act_file)
    act_data = np.loadtxt(act_file).T
    t_data = act_data[0]

    def class_title(class_name):
        class_key = str(class_name)
        if class_key in utils.jsonToStringMap:
            return utils.jsonToStringMap[class_key]
        normalised = _normalise_cell_class_name(class_key)
        if normalised == "vnc":
            return "VNC Neurons"
        if normalised == "head":
            return "Head Neurons"
        if normalised == "interneuron":
            return "Interneurons"
        return class_key.replace("_", " ").title()

    def get_int_value(section, key, default=0):
        if not isinstance(network_json_data, dict):
            return default
        value = network_json_data.get(section, {}).get(key)
        value = _json_value(value, default)
        if isinstance(value, bool) or not isinstance(value, (int, float)):
            return default
        return int(value)

    def has_new_cell_metadata():
        if not isinstance(network_json_data, dict):
            return False
        nervous_system = network_json_data.get("nervous_system")
        return (
            isinstance(nervous_system, dict)
            and isinstance(nervous_system.get("cells"), dict)
            and isinstance(_json_value(nervous_system.get("cell_names")), list)
        )

    def build_new_activity_panels():
        panels = []
        act_column_count = act_data.shape[0] - 1
        next_column = 1

        sr_count = 0
        stretch_receptor = network_json_data.get("stretch_receptor")
        if isinstance(stretch_receptor, dict):
            sr_count = _json_value(stretch_receptor.get("plot_size"), 0)
        elif "Stretch receptor" in network_json_data:
            sr_count = _json_value(
                network_json_data["Stretch receptor"].get("plot size"), 0
            )
        if isinstance(sr_count, (int, float)) and sr_count > 0:
            sr_count = min(int(sr_count), act_column_count - (next_column - 1))
            if sr_count > 0:
                panels.append(
                    {
                        "title": "Stretch receptors",
                        "indices": list(range(next_column, next_column + sr_count)),
                        "labels": ["SR {}".format(i) for i in range(sr_count)],
                    }
                )
                next_column += sr_count

        nervous_system = network_json_data["nervous_system"]
        cell_names = _json_value(nervous_system["cell_names"], [])
        cells = nervous_system["cells"]
        vnc_cell_count = get_int_value("worm", "N_units") * get_int_value(
            "worm", "N_neuronsperunit"
        )
        if vnc_cell_count > 0 and vnc_cell_count < len(cell_names):
            act_cell_names = cell_names[vnc_cell_count:] + cell_names[:vnc_cell_count]
        else:
            act_cell_names = list(cell_names)

        cell_count = min(len(act_cell_names), act_column_count - (next_column - 1))
        if cell_count > 0:
            class_groups = []
            for cell_name in act_cell_names[:cell_count]:
                cell = cells.get(cell_name, {})
                cell_class = _json_value(cell.get("cell_class"), "Neurons")
                if not class_groups or class_groups[-1]["class"] != cell_class:
                    class_groups.append({"class": cell_class, "cells": []})
                class_groups[-1]["cells"].append(cell_name)

            for group in class_groups:
                group_size = len(group["cells"])
                panels.append(
                    {
                        "title": class_title(group["class"]),
                        "indices": list(range(next_column, next_column + group_size)),
                        "labels": group["cells"],
                    }
                )
                next_column += group_size

        muscle_count = 0
        if "Muscle" in network_json_data:
            muscle_count = _json_value(network_json_data["Muscle"].get("Nmuscles"), 0)
            if isinstance(muscle_count, (int, float)):
                muscle_count = int(muscle_count) * 2
        muscle_count = min(muscle_count, act_column_count - (next_column - 1))
        if muscle_count > 0:
            panels.append(
                {
                    "title": "Muscles",
                    "indices": list(range(next_column, next_column + muscle_count)),
                    "labels": ["Mu {}".format(i) for i in range(muscle_count)],
                }
            )
            next_column += muscle_count

        remaining_count = act_data.shape[0] - next_column
        if remaining_count > 0:
            input_switcher = network_json_data.get("input_switcher", {})
            has_scheduled_driving_inputs = (
                isinstance(input_switcher, dict)
                and "input_indices" in input_switcher
                and "time_periods" in input_switcher
            )
            driving_count = 0
            driving_inputs = network_json_data.get("driving_inputs")
            if isinstance(driving_inputs, dict):
                driving_values = _json_value(
                    driving_inputs.get("inputs", {}).get("value"), []
                )
                if isinstance(driving_values, list):
                    driving_count = len(driving_values)
            if driving_count == 0 and has_scheduled_driving_inputs:
                driving_count = _json_value(input_switcher.get("size"), 0)
                if isinstance(driving_count, bool) or not isinstance(
                    driving_count, (int, float)
                ):
                    driving_count = 0
            driving_count = min(driving_count, remaining_count)

            if driving_count > 0:
                if has_scheduled_driving_inputs:
                    panels.append(
                        {
                            "title": "Scheduled driving inputs",
                            "indices": list(
                                range(next_column, next_column + driving_count)
                            ),
                            "labels": [
                                "input_{}".format(i + 1) for i in range(driving_count)
                            ],
                        }
                    )
                next_column += driving_count
                remaining_count -= driving_count

            sensor_count = 0
            sensors = network_json_data.get("sensors")
            if isinstance(sensors, dict):
                sensor_names = sorted(
                    key for key in sensors if str(key).startswith("sensor_")
                )
                sensor_count = min(len(sensor_names) * 2, remaining_count)
                if sensor_count > 0:
                    labels = []
                    for sensor_name in sensor_names:
                        labels.extend(
                            [
                                "{} output_1".format(sensor_name),
                                "{} output_2".format(sensor_name),
                            ]
                        )
                    panels.append(
                        {
                            "title": "Sensor outputs",
                            "indices": list(
                                range(next_column, next_column + sensor_count)
                            ),
                            "labels": labels[:sensor_count],
                        }
                    )
                    next_column += sensor_count
                    remaining_count -= sensor_count

            if remaining_count > 0:
                panels.append(
                    {
                        "title": "External inputs",
                        "indices": list(
                            range(next_column, next_column + remaining_count)
                        ),
                        "labels": [
                            "input_{}".format(i + 1) for i in range(remaining_count)
                        ],
                    }
                )

        return panels

    use_new_activity_panels = has_new_cell_metadata()
    if use_new_activity_panels:
        activity_panels = build_new_activity_panels()
        plot_format = {
            "do_curv_plot": True,
            "do_body_plot": True,
        }
    else:
        if a.modelName == "COW2DSR":
            plot_format = utils.getPlotFormat(network_json_data)
        else:
            plot_format = utils.plot_formats[a.modelName]
        activity_panels = []

    if a.modelName == "CO18" or a.modelName == "CO18Full":
        # network_json_data = utils.getJsonFile(hf.rename_file("worm_data.json"))
        CO18_size = utils.getNervousSystemSize(network_json_data)
        plot_format["data_sizes"] = [CO18_size, 2]
        plot_format["plot_cell_names"] = ["N" + str(i) for i in range(CO18_size)] + [
            "S" + str(i) for i in range(2)
        ]
        plot_format["plot_col_divs"] = [CO18_size, 2]

    curv_file = None
    if plot_format["do_curv_plot"]:
        curv_t_file = hf.rename_file("curv_t.dat")
        curv_dat_file = hf.rename_file("curv.dat")
        if os.path.isfile(curv_t_file):
            curv_file = curv_t_file
        elif os.path.isfile(curv_dat_file):
            curv_file = curv_dat_file
        else:
            print(
                "Skipping curvature plot: neither %s nor %s found"
                % (curv_t_file, curv_dat_file)
            )
    do_curv_plot = plot_format["do_curv_plot"] and curv_file is not None

    body_file = None
    if plot_format["do_body_plot"]:
        if a.modelName == "CO" or a.modelName == "W2DCO":
            body_file = hf.rename_file("bodypos.dat")
        else:
            body_file = hf.rename_file("body.dat")
        if not os.path.isfile(body_file):
            print("Skipping body position plot: %s not found" % body_file)
            body_file = None
    do_body_plot = plot_format["do_body_plot"] and body_file is not None

    def makePanel(indices, title, labels, plot_num):
        axs[plot_num, 0].set_title(title, fontsize=title_font_size)
        axs[plot_num, 1].set_title(title, fontsize=title_font_size)

        for row_index, label in zip(indices, labels):
            axs[plot_num, 0].plot(
                t_data[data_seg],
                act_data[row_index][data_seg],
                label=label,
                linewidth=0.5,
            )
            # axs[plot_num, 0].xaxis.set_ticklabels([])
        # plt.legend()

        data_list = act_data[indices, :][:, data_seg]
        t_plot = t_data[data_seg]
        # axs[plot_num, 1].set_title("Body curvature", fontsize=title_font_size)
        axs[plot_num, 1].imshow(
            data_list,
            aspect="auto",
            interpolation="nearest",
            extent=imshow_time_extent(t_plot, data_list.shape[0]),
            origin="lower",
        )

        # axs[plot_num, 1].imshow(data_list, aspect="auto", interpolation="nearest")
        # axs[plot_num, 1].xaxis.set_ticklabels([])
        set_imshow_row_ticks(axs[plot_num, 1], data_list.shape[0])

    def makeFigure(data_offset, data_size, title, label, plot_num):
        indices = list(range(data_offset, data_size + data_offset))
        labels = [label + " %i" % i for i in range(data_size)]
        makePanel(indices, title, labels, plot_num)

    plot_rows = (
        len(activity_panels)
        if use_new_activity_panels
        else len(plot_format["fig_titles"])
    )
    if do_curv_plot or do_body_plot:
        plot_rows += 1
    if plot_rows > 1:
        fig, axs = plt.subplots(plot_rows, 2, figsize=(10, plot_rows * 2))
    else:
        fig, axs = plt.subplots(plot_rows, 2, figsize=(10, 5), squeeze=False)

    ###  Worm neuron/muscle activation

    t_start = 0
    t_end = 10000
    data_seg = (t_data >= t_start) & (t_data < t_end)

    offset = 1
    count_num = 0
    if use_new_activity_panels:
        for panel in activity_panels:
            makePanel(
                panel["indices"],
                panel["title"],
                panel["labels"],
                count_num,
            )
            axs[count_num, 0].set_xlabel("Time (s)", fontsize=label_font_size)
            axs[count_num, 1].set_xlabel("Time (s)", fontsize=label_font_size)
            count_num += 1
    else:
        for val in zip(
            plot_format["data_sizes"],
            plot_format["fig_titles"],
            plot_format["fig_labels"],
        ):
            makeFigure(offset, *val, count_num)
            if False:
                if count_num < len(plot_format["data_sizes"]) - 1:
                    axs[count_num, 0].xaxis.set_ticklabels([])
                else:
                    axs[count_num, 0].set_xlabel("Time (s)", fontsize=label_font_size)
            axs[count_num, 0].set_xlabel("Time (s)", fontsize=label_font_size)
            axs[count_num, 1].set_xlabel("Time (s)", fontsize=label_font_size)
            count_num += 1
            offset += val[0]

    ###  Worm body curvature
    if do_curv_plot:
        print("Loading curvature data from: %s" % curv_file)
        curv_data = np.loadtxt(curv_file).T
        t_data = curv_data[0]
        data_seg = (t_data >= t_start) & (t_data < t_end)
        curv_data_less_time = curv_data[1:, data_seg]
        t_data = t_data[data_seg]

        axs[count_num, 1].set_title("Body curvature", fontsize=title_font_size)
        axs[count_num, 1].imshow(
            curv_data_less_time,
            aspect="auto",
            interpolation="nearest",
            extent=imshow_time_extent(t_data, curv_data_less_time.shape[0]),
        )
        if False:
            axs[count_num, 1].set_xticks(np.linspace(0, len(data_seg), 8))
            # axs[count_num, 1].set_xticks(np.linspace(t_data[0]/t_inc,t_data[-1]/t_inc, 8))
            axs[count_num, 1].xaxis.set_ticklabels(
                np.around(np.linspace(t_data[0], t_data[-1], 8), 2)
            )
        axs[count_num, 1].set_xlabel("Time (s)", fontsize=label_font_size)

        ###  Body position

    if do_body_plot:
        print("Loading body position data from: %s" % body_file)
        body_data = np.loadtxt(body_file).T

        # tmax = 1520
        tmax = body_data.shape[1]
        # if tmax >= body_data.shape[1]:
        #    tmax = body_data.shape[1]
        num = 60.0

        if not (a.modelName == "CO" or a.modelName == "W2DCO"):
            hf.plot_orients(body_data)

        # title = axs[count_num, 0].set_title("2D worm motion", fontsize=title_font_size, loc='right')
        axs[count_num, 0].set_title(
            "2D worm motion (mm)",
            fontsize=title_font_size,  # y=0.5, x=1.1
        )

        box = axs[count_num, 0].get_position()
        box.x0 = box.x0 - 0.1
        box.x1 = box.x1 - 0.1
        axs[count_num, 0].set_position(box)
        # offset = np.array([-0.15, 0.0])
        # title.set_position(axs[count_num, 0].get_position() + offset)

        wcon = {}
        wcon["data"] = []

        wcon["units"] = {
            "t": "s",
            "x": "mm",
            "y": "mm",
        }

        wcon["metadata"] = {
            "timestamp": datetime.now().isoformat(),
            "protocol": [
                "Simulation of worm behaviour by Worm2D",
            ],
            "software": {
                "name": "Worm2D",
                "version": hf.get_worm2d_version(),
            },
        }

        dd = {}
        wcon["data"].append(dd)
        dd["id"] = "test"
        dd["ptail"] = 0  # required??
        dd["t"] = []
        dd["x"] = []
        dd["y"] = []

        fig_body, ax_body = plt.subplots(figsize=(5, 5))

        for t in range(1, tmax, max(1, int(tmax / num))):
            f = float(t) / tmax

            dd["t"].append(body_data[0][t])

            color = "#%02x%02x00" % (int(0xFF * (f)), int(0xFF * (1 - f) * 0.8))
            # color2 = "#%06x" % random.randint(0, 0xFFFFFF)

            point_start = 0
            point_end = 50
            markersize = 3
            markersize_small = 0.4
            if a.modelName == "CO" or a.modelName == "W2DCO":
                point_start = 0
                point_end = 1
                markersize = 10
                markersize_small = 10
            xs = []
            ys = []

            for i in range(point_start, point_end):
                x = body_data[i * 3 + 1][t] * 1000
                # xs.append(x * 1000)
                xs.append(x)
                y = body_data[i * 3 + 2][t] * 1000
                # ys.append(y * 1000)
                ys.append(y)
                # y1 = body_data[i * 3 + 2][t]
                if i == 1 and a.verbose:
                    print(
                        "%s + Plotting %i at t=%s (%s,%s), %s"
                        % ("\n" if i == point_start else "", i, t, x, y, color)
                    )

                axs[count_num, 0].plot(
                    x,
                    y,
                    ".",
                    color=color,
                    markersize=markersize if t == 1 else markersize_small,
                )
                ax_body.plot(
                    x,
                    y,
                    ".",
                    color=color,
                    markersize=markersize if t == 1 else markersize_small,
                )

                # print("%s - Plotting %i at t=%s (%s,%s), %s"%('\n' if i==point_start else '', i, t,x,y1, color))
                # plt.plot([x],[y1],'.',color=color)

            dd["x"].append(xs)
            dd["y"].append(ys)

            # print("--- - Plotting at t=%s (%s,%s)" % (t, xs, ys))
        import json

        with open(hf.rename_file("output.wcon"), "w", encoding="utf-8") as json_file:
            json.dump(wcon, json_file, indent=4, ensure_ascii=False)

        # axs[count_num, 0].set_aspect("equal")

        ax_body.set_xlabel("X Position (mm)", fontsize=label_font_size)
        ax_body.set_ylabel("Y Position (mm)", fontsize=label_font_size)
        ax_body.set_aspect("equal")
        fig_body.tight_layout()
        filename = hf.rename_file("Motion.png")
        fig_body.savefig(filename, bbox_inches="tight", dpi=300)
        # fig_body.close()
        plt.close(fig_body)

    fig.tight_layout()
    # fig.subplots_adjust(hspace=0.5)

    filename = hf.rename_file("ExampleActivity.png")
    fig.savefig(filename, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename)

    if a.showPlot:
        print("Showing plot")
        plt.show()
    plt.close()

    from worm2d import F2_fig_behavior as f2fig

    notF2models = [
        "CO",
        "W2DCO",
        "W2Dosc",
        "W2Dosc21",
        "CO18Full",
        "COW2DSR",
        "RS18_CO18Full",
    ]
    if a.modelName not in notF2models:
        f2fig.make_fig(model_name=a.modelName)


if __name__ == "__main__":
    import sys

    reload_single_run(showPlot=False, modelName="Net21", folderName=sys.argv[1])
