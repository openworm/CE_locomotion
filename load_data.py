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
import neuromlLocal.utils as utils

# from matplotlib.ticker import MaxNLocator
import math
import helper_funcs as hf

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


def plot_json_dictionary_summary(
    output_folder=None,
    json_data=None,
    json_filename="worm_data_worm.json",
    exclude_keys=None,
    filename="json_dictionary_summary.png",
    save_png=True,
    columns=6,
    max_rows_per_box=18,
):
    """Return a figure summarising top-level JSON objects and their subfields."""
    from matplotlib.patches import FancyBboxPatch

    if json_data is None:
        if output_folder is None:
            raise ValueError("Either output_folder or json_data must be supplied")
        json_path = os.path.join(output_folder, json_filename)
        if not os.path.isfile(json_path):
            raise FileNotFoundError("Could not find {}".format(json_path))
        json_data = utils.getJsonFile(json_path)

    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")

    excluded = {
        "InputNS",
        "Muscle",
        "OutputNS",
        "PhenoNames",
        "PhenoNamesNums",
        "evolved_used",
    }
    if exclude_keys is not None:
        excluded.update(exclude_keys)

    sections = [(key, val) for key, val in json_data.items() if key not in excluded]
    if not sections:
        raise ValueError("No JSON sections remain after applying exclude_keys")

    columns = max(1, min(int(columns), len(sections)))

    def subfield_rows(section):
        if not isinstance(section, dict):
            return ["value"]
        return [str(subkey) for subkey in section]

    def ellipsize(text, max_len=28):
        text = str(text)
        if len(text) <= max_len:
            return text
        return text[: max_len - 1] + "..."

    def format_title(text):
        text = str(text)
        if text == "Evolutionary Optimization Parameters":
            return "Evolutionary Optimization\nParameters", 9.5
        if len(text) > 25:
            words = text.replace("_", " ").split()
            if len(words) > 1:
                split_at = int(math.ceil(len(words) / 2))
                return (
                    "{}\n{}".format(
                        " ".join(words[:split_at]),
                        " ".join(words[split_at:]),
                    ),
                    9.5,
                )
            return ellipsize(text, 25), 9.5
        return text, 11

    box_palette = [
        ("#F8FAFC", "#DDEBF7"),
        ("#FFF7E6", "#F6D8A8"),
        ("#F0F8EE", "#CFE8C6"),
        ("#F8F1FA", "#E4CCE9"),
        ("#EEF7F8", "#CBE5E8"),
    ]
    edge = "#2F3A45"
    text_color = "#1F2933"

    box_w = 0.82
    col_gap = 0.08
    box_gap = 0.10
    header_h = 0.14
    row_h = 0.055
    top_pad = 0.045
    bottom_pad = 0.045
    table_gap = 0.030

    section_specs = []
    for section_name, section in sections:
        rows_ = subfield_rows(section)
        if not rows_:
            continue
        shown_rows = rows_[:max_rows_per_box]
        omitted = len(rows_) - len(shown_rows)
        if omitted > 0:
            shown_rows.append("... {} more".format(omitted))
        box_h = (
            top_pad
            + header_h
            + table_gap
            + row_h * len(shown_rows)
            + bottom_pad
        )
        section_specs.append(
            {
                "name": section_name,
                "rows": shown_rows,
                "height": box_h,
            }
        )

    if not section_specs:
        raise ValueError("No non-empty JSON sections remain after applying exclude_keys")

    section_specs.sort(key=lambda spec: (-spec["height"], spec["name"].lower()))

    col_heights = [0.0] * columns
    for spec in section_specs:
        col = min(range(columns), key=lambda idx: col_heights[idx])
        spec["col"] = col
        spec["top"] = -col_heights[col]
        spec["bottom"] = spec["top"] - spec["height"]
        col_heights[col] += spec["height"] + box_gap

        neighbour_colours = set()
        for placed in section_specs:
            if "colour_index" not in placed:
                continue
            same_column_neighbour = (
                placed["col"] == col
                and abs(spec["top"] - placed["bottom"] + box_gap) < 1e-9
            )
            adjacent_column_overlap = (
                abs(placed["col"] - col) == 1
                and spec["bottom"] < placed["top"]
                and spec["top"] > placed["bottom"]
            )
            if same_column_neighbour or adjacent_column_overlap:
                neighbour_colours.add(placed["colour_index"])
        for colour_index in range(len(box_palette)):
            if colour_index not in neighbour_colours:
                spec["colour_index"] = colour_index
                break
        if "colour_index" not in spec:
            spec["colour_index"] = len(neighbour_colours) % len(box_palette)

    max_depth = max(col_heights) - box_gap
    fig_w = max(11.0, columns * 2.55)
    fig_h = max(6.0, max_depth * 4.0)
    fig, ax = plt.subplots(figsize=(fig_w, fig_h))
    ax.set_xlim(-0.05, columns * (box_w + col_gap) - col_gap + 0.05)
    ax.set_ylim(-max_depth - 0.05, 0.05)
    ax.axis("off")

    for spec in section_specs:
        section_name = spec["name"]
        shown_rows = spec["rows"]
        x = spec["col"] * (box_w + col_gap)
        h = spec["height"]
        y = spec["top"] - h
        w = box_w
        box_face, header_face = box_palette[spec["colour_index"]]

        patch = FancyBboxPatch(
            (x, y),
            w,
            h,
            boxstyle="round,pad=0.012",
            facecolor=box_face,
            edgecolor=edge,
            linewidth=1.2,
        )
        ax.add_patch(patch)

        header = FancyBboxPatch(
            (x, y + h - header_h),
            w,
            header_h,
            boxstyle="round,pad=0.012",
            facecolor=header_face,
            edgecolor=edge,
            linewidth=0.9,
        )
        ax.add_patch(header)
        title_text, title_fontsize = format_title(section_name)
        ax.text(
            x + w / 2,
            y + h - header_h / 2,
            title_text,
            ha="center",
            va="center",
            fontsize=title_fontsize,
            fontweight="bold",
            color=text_color,
            linespacing=0.95,
        )

        table_y_top = y + h - header_h - table_gap
        text_x = x + 0.055
        for row_index, subkey in enumerate(shown_rows):
            yy = table_y_top - row_h * (row_index + 0.5)
            if row_index % 2 == 1:
                ax.add_patch(
                    FancyBboxPatch(
                        (x + 0.025, yy - row_h / 2),
                        w - 0.05,
                        row_h,
                        boxstyle="square,pad=0",
                        facecolor="#FFFFFF",
                        edgecolor="none",
                    )
                )
            ax.text(
                text_x,
                yy,
                ellipsize(subkey),
                ha="left",
                va="center",
                fontsize=8.0,
                color=text_color,
            )

    fig.tight_layout(pad=0.25)

    if save_png:
        if output_folder is None:
            raise ValueError("output_folder is required when save_png=True")
        fig.savefig(os.path.join(output_folder, filename), bbox_inches="tight", dpi=300)

    return fig


def plot_json_dictionary_summary_2(
    output_folder=None,
    json_data=None,
    json_filename="worm_data_worm.json",
    exclude_keys=None,
    filename="json_dictionary_summary_2.png",
    save_png=True,
    columns=5,
    max_rows_per_box=18,
):
    """Return a two-column summary of top-level JSON subfield names and values."""
    from matplotlib.patches import FancyBboxPatch

    if json_data is None:
        if output_folder is None:
            raise ValueError("Either output_folder or json_data must be supplied")
        json_path = os.path.join(output_folder, json_filename)
        if not os.path.isfile(json_path):
            raise FileNotFoundError("Could not find {}".format(json_path))
        json_data = utils.getJsonFile(json_path)

    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")

    excluded = {
        "InputNS",
        "Muscle",
        "OutputNS",
        "PhenoNames",
        "PhenoNamesNums",
        "evolved_used",
    }
    if exclude_keys is not None:
        excluded.update(exclude_keys)

    sections = [(key, val) for key, val in json_data.items() if key not in excluded]
    if not sections:
        raise ValueError("No JSON sections remain after applying exclude_keys")

    columns = max(1, min(int(columns), len(sections)))

    def display_value(value):
        if isinstance(value, dict) and "value" in value:
            value = value["value"]
        elif isinstance(value, dict):
            return "object"
        elif isinstance(value, list):
            return "list"

        if isinstance(value, bool):
            return "true" if value else "false"
        if isinstance(value, float):
            return "{:.4g}".format(value)
        if isinstance(value, (int, np.integer)):
            return str(int(value))
        if isinstance(value, str):
            return value
        if value is None:
            return "null"
        if isinstance(value, list):
            return "list"
        if isinstance(value, dict):
            return "object"
        return str(value)

    def subfield_rows(section):
        if not isinstance(section, dict):
            return [("value", display_value(section))]
        return [(str(subkey), display_value(subval)) for subkey, subval in section.items()]

    def ellipsize(text, max_len=23):
        text = str(text)
        if len(text) <= max_len:
            return text
        return text[: max_len - 1] + "..."

    def format_title(text):
        text = str(text)
        if text == "Evolutionary Optimization Parameters":
            return "Evolutionary Optimization\nParameters", 9.0
        if len(text) > 25:
            words = text.replace("_", " ").split()
            if len(words) > 1:
                split_at = int(math.ceil(len(words) / 2))
                return (
                    "{}\n{}".format(
                        " ".join(words[:split_at]),
                        " ".join(words[split_at:]),
                    ),
                    9.0,
                )
            return ellipsize(text, 25), 9.0
        return text, 10.5

    box_palette = [
        ("#F8FAFC", "#DDEBF7"),
        ("#FFF7E6", "#F6D8A8"),
        ("#F0F8EE", "#CFE8C6"),
        ("#F8F1FA", "#E4CCE9"),
        ("#EEF7F8", "#CBE5E8"),
    ]
    edge = "#2F3A45"
    text_color = "#1F2933"

    box_w = 1.02
    col_gap = 0.10
    box_gap = 0.10
    header_h = 0.14
    column_header_h = 0.055
    row_h = 0.055
    top_pad = 0.045
    bottom_pad = 0.045
    table_gap = 0.030

    section_specs = []
    for section_name, section in sections:
        rows_ = subfield_rows(section)
        if not rows_:
            continue
        shown_rows = rows_[:max_rows_per_box]
        omitted = len(rows_) - len(shown_rows)
        if omitted > 0:
            shown_rows.append(("... {} more".format(omitted), ""))
        box_h = (
            top_pad
            + header_h
            + table_gap
            + column_header_h
            + row_h * len(shown_rows)
            + bottom_pad
        )
        section_specs.append(
            {
                "name": section_name,
                "rows": shown_rows,
                "height": box_h,
            }
        )

    if not section_specs:
        raise ValueError("No non-empty JSON sections remain after applying exclude_keys")

    section_specs.sort(key=lambda spec: (-spec["height"], spec["name"].lower()))

    col_heights = [0.0] * columns
    for spec in section_specs:
        col = min(range(columns), key=lambda idx: col_heights[idx])
        spec["col"] = col
        spec["top"] = -col_heights[col]
        spec["bottom"] = spec["top"] - spec["height"]
        col_heights[col] += spec["height"] + box_gap

        neighbour_colours = set()
        for placed in section_specs:
            if "colour_index" not in placed:
                continue
            same_column_neighbour = (
                placed["col"] == col
                and abs(spec["top"] - placed["bottom"] + box_gap) < 1e-9
            )
            adjacent_column_overlap = (
                abs(placed["col"] - col) == 1
                and spec["bottom"] < placed["top"]
                and spec["top"] > placed["bottom"]
            )
            if same_column_neighbour or adjacent_column_overlap:
                neighbour_colours.add(placed["colour_index"])
        for colour_index in range(len(box_palette)):
            if colour_index not in neighbour_colours:
                spec["colour_index"] = colour_index
                break
        if "colour_index" not in spec:
            spec["colour_index"] = len(neighbour_colours) % len(box_palette)

    max_depth = max(col_heights) - box_gap
    fig_w = max(11.0, columns * 3.25)
    fig_h = max(6.0, max_depth * 4.0)
    fig, ax = plt.subplots(figsize=(fig_w, fig_h))
    ax.set_xlim(-0.05, columns * (box_w + col_gap) - col_gap + 0.05)
    ax.set_ylim(-max_depth - 0.05, 0.05)
    ax.axis("off")

    for spec in section_specs:
        section_name = spec["name"]
        shown_rows = spec["rows"]
        x = spec["col"] * (box_w + col_gap)
        h = spec["height"]
        y = spec["top"] - h
        w = box_w
        box_face, header_face = box_palette[spec["colour_index"]]

        patch = FancyBboxPatch(
            (x, y),
            w,
            h,
            boxstyle="round,pad=0.012",
            facecolor=box_face,
            edgecolor=edge,
            linewidth=1.2,
        )
        ax.add_patch(patch)

        header = FancyBboxPatch(
            (x, y + h - header_h),
            w,
            header_h,
            boxstyle="round,pad=0.012",
            facecolor=header_face,
            edgecolor=edge,
            linewidth=0.9,
        )
        ax.add_patch(header)
        title_text, title_fontsize = format_title(section_name)
        ax.text(
            x + w / 2,
            y + h - header_h / 2,
            title_text,
            ha="center",
            va="center",
            fontsize=title_fontsize,
            fontweight="bold",
            color=text_color,
            linespacing=0.95,
        )

        table_y_top = y + h - header_h - table_gap
        name_x = x + 0.050
        value_x = x + 0.630
        ax.add_patch(
            FancyBboxPatch(
                (x + 0.025, table_y_top - column_header_h),
                w - 0.05,
                column_header_h,
                boxstyle="square,pad=0",
                facecolor="#FFFFFF",
                edgecolor="none",
                alpha=0.75,
            )
        )
        ax.text(
            name_x,
            table_y_top - column_header_h / 2,
            "name",
            ha="left",
            va="center",
            fontsize=7.8,
            fontweight="bold",
            color=text_color,
        )
        ax.text(
            value_x,
            table_y_top - column_header_h / 2,
            "value",
            ha="left",
            va="center",
            fontsize=7.8,
            fontweight="bold",
            color=text_color,
        )

        rows_top = table_y_top - column_header_h
        for row_index, (subkey, subval) in enumerate(shown_rows):
            yy = rows_top - row_h * (row_index + 0.5)
            if row_index % 2 == 1:
                ax.add_patch(
                    FancyBboxPatch(
                        (x + 0.025, yy - row_h / 2),
                        w - 0.05,
                        row_h,
                        boxstyle="square,pad=0",
                        facecolor="#FFFFFF",
                        edgecolor="none",
                        alpha=0.65,
                    )
                )
            ax.text(
                name_x,
                yy,
                ellipsize(subkey, 28),
                ha="left",
                va="center",
                fontsize=7.6,
                color=text_color,
            )
            ax.text(
                value_x,
                yy,
                ellipsize(subval, 18),
                ha="left",
                va="center",
                fontsize=7.6,
                color=text_color,
            )

    fig.tight_layout(pad=0.25)

    if save_png:
        if output_folder is None:
            raise ValueError("output_folder is required when save_png=True")
        fig.savefig(os.path.join(output_folder, filename), bbox_inches="tight", dpi=300)

    return fig


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


def plot_motion_all(
    input_folder,
    subfolders=None,
    legend_names=None,
    save_png=True,
    filename="motion_all.png",
    mean_filename="motion_all_mean.png",
    max_snapshots=60,
):
    """Plot body profiles and mean-position paths from simulation subfolders."""
    if not os.path.isdir(input_folder):
        raise NotADirectoryError(
            "Could not find input directory: {}".format(input_folder)
        )
    if (
        isinstance(max_snapshots, bool)
        or not isinstance(max_snapshots, int)
        or max_snapshots < 1
    ):
        raise ValueError("max_snapshots must be a positive integer")
    if legend_names is not None and (
        isinstance(legend_names, str) or not isinstance(legend_names, (list, tuple))
    ):
        raise TypeError("legend_names must be a list or tuple of strings")

    input_folder = os.path.abspath(input_folder)
    simulation_files = []

    def body_file_for_folder(folder):
        if not os.path.isdir(folder):
            raise NotADirectoryError(
                "Could not find simulation subfolder: {}".format(folder)
            )
        files = set(os.listdir(folder))
        body_filename = None
        if "body.dat" in files:
            body_filename = "body.dat"
        elif "bodypos.dat" in files:
            body_filename = "bodypos.dat"
        if body_filename is None:
            return None
        return os.path.join(folder, body_filename)

    if subfolders is None:
        for folder, child_subfolders, files in os.walk(input_folder):
            child_subfolders.sort()
            if folder == input_folder:
                continue
            body_file = body_file_for_folder(folder)
            if body_file is not None:
                simulation_files.append(
                    (os.path.relpath(folder, input_folder), body_file)
                )
    else:
        if isinstance(subfolders, str) or not isinstance(subfolders, (list, tuple)):
            raise TypeError("subfolders must be a list or tuple of folder names")
        for subfolder in subfolders:
            if not isinstance(subfolder, str) or not subfolder:
                raise ValueError("subfolders must contain non-empty strings")
            folder = os.path.abspath(os.path.join(input_folder, subfolder))
            if os.path.commonpath([input_folder, folder]) != input_folder:
                raise ValueError(
                    "Subfolder {!r} is outside input_folder".format(subfolder)
                )
            body_file = body_file_for_folder(folder)
            if body_file is None:
                raise FileNotFoundError(
                    "No body.dat or bodypos.dat file was found in {}".format(folder)
                )
            simulation_files.append((os.path.relpath(folder, input_folder), body_file))

    if not simulation_files:
        raise FileNotFoundError(
            "No body.dat or bodypos.dat files were found in subfolders of {}".format(
                input_folder
            )
        )

    if subfolders is None:
        simulation_files.sort(key=lambda item: item[0])
    if legend_names is not None:
        if len(legend_names) != len(simulation_files):
            raise ValueError(
                "legend_names must contain one label for each plotted subfolder"
            )
        for legend_name in legend_names:
            if not isinstance(legend_name, str) or not legend_name:
                raise ValueError("legend_names must contain non-empty strings")
        simulation_files = [
            (run_name, body_file, legend_name)
            for (run_name, body_file), legend_name in zip(
                simulation_files, legend_names
            )
        ]
    else:
        simulation_files = [
            (run_name, body_file, run_name) for run_name, body_file in simulation_files
        ]
    profile_fig, profile_ax = plt.subplots(figsize=(8, 8))
    mean_fig, mean_ax = plt.subplots(figsize=(8, 8))
    color_map = plt.get_cmap("tab20")

    for run_index, (run_name, body_file, legend_name) in enumerate(simulation_files):
        body_data = np.loadtxt(body_file)
        if body_data.ndim == 1:
            body_data = body_data.reshape(1, -1)
        if body_data.ndim != 2 or body_data.shape[1] < 4:
            raise ValueError(
                "{} does not contain time and x/y/z body coordinates".format(body_file)
            )
        coordinate_columns = body_data.shape[1] - 1
        if coordinate_columns % 3 != 0:
            raise ValueError(
                "{} has {} coordinate columns; expected a multiple of 3".format(
                    body_file, coordinate_columns
                )
            )

        segment_count = coordinate_columns // 3
        x_positions = body_data[:, 1 : 1 + 3 * segment_count : 3] * 1000.0
        y_positions = body_data[:, 2 : 2 + 3 * segment_count : 3] * 1000.0
        color = color_map(run_index % color_map.N)

        center_x = np.nanmean(x_positions, axis=1)
        center_y = np.nanmean(y_positions, axis=1)
        mean_ax.plot(
            center_x,
            center_y,
            color=color,
            linewidth=1.5,
            linestyle="-",
            label=legend_name,
        )

        sample_count = min(max_snapshots, body_data.shape[0])
        snapshot_indices = np.unique(
            np.linspace(0, body_data.shape[0] - 1, sample_count).astype(int)
        )
        for snapshot_number, snapshot_index in enumerate(snapshot_indices):
            profile_ax.plot(
                x_positions[snapshot_index],
                y_positions[snapshot_index],
                ".",
                color=color,
                markersize=1.2,
                alpha=0.35,
                label=legend_name if snapshot_number == 0 else "_nolegend_",
            )

    def finish_figure(fig, ax, title):
        ax.set_title(title)
        ax.set_xlabel("X Position (mm)")
        ax.set_ylabel("Y Position (mm)")
        ax.set_aspect("equal", adjustable="datalim")
        ax.grid(True, linewidth=0.4, alpha=0.25)
        ax.legend(
            title="Simulation",
            loc="upper left",
            bbox_to_anchor=(1.02, 1.0),
            frameon=False,
        )
        fig.tight_layout()

    finish_figure(profile_fig, profile_ax, "Worm body profiles")
    finish_figure(mean_fig, mean_ax, "Mean body-position trajectories")

    if save_png:
        for fig, output_file in (
            (profile_fig, filename),
            (mean_fig, mean_filename),
        ):
            if not os.path.isabs(output_file):
                output_file = os.path.join(input_folder, output_file)
            fig.savefig(output_file, bbox_inches="tight", dpi=300)

    return profile_fig, mean_fig


def _body_file_for_simulation_folder(folder):
    if not os.path.isdir(folder):
        raise NotADirectoryError(
            "Could not find simulation output directory: {}".format(folder)
        )
    for body_filename in ("body.dat", "bodypos.dat"):
        body_file = os.path.join(folder, body_filename)
        if os.path.isfile(body_file):
            return body_file
    raise FileNotFoundError(
        "No body.dat or bodypos.dat file was found in {}".format(folder)
    )


def _load_body_position_file(body_file):
    body_data = np.loadtxt(body_file)
    if body_data.ndim == 1:
        body_data = body_data.reshape(1, -1)
    if body_data.ndim != 2 or body_data.shape[1] < 4:
        raise ValueError(
            "{} does not contain time and x/y/z body coordinates".format(body_file)
        )
    coordinate_columns = body_data.shape[1] - 1
    if coordinate_columns % 3 != 0:
        raise ValueError(
            "{} has {} coordinate columns; expected a multiple of 3".format(
                body_file, coordinate_columns
            )
        )
    segment_count = coordinate_columns // 3
    times = body_data[:, 0]
    x_positions = body_data[:, 1 : 1 + 3 * segment_count : 3] * 1000.0
    y_positions = body_data[:, 2 : 2 + 3 * segment_count : 3] * 1000.0
    return times, x_positions, y_positions


def plot_head_motion(
    output_folder,
    save_png=True,
    filename="HeadMotion.png",
    head_index=0,
    max_snapshots=60,
    show_path=False,
    marker_size=8.0,
    line_width=1.0,
):
    """Plot only the worm head trajectory from body.dat/bodypos.dat."""
    output_folder = os.path.abspath(output_folder)
    body_file = _body_file_for_simulation_folder(output_folder)
    times, x_positions, y_positions = _load_body_position_file(body_file)

    if (
        isinstance(head_index, bool)
        or not isinstance(head_index, int)
        or head_index < 0
        or head_index >= x_positions.shape[1]
    ):
        raise ValueError(
            "head_index must be an integer in the range 0 to {}".format(
                x_positions.shape[1] - 1
            )
        )

    if (
        isinstance(max_snapshots, bool)
        or not isinstance(max_snapshots, int)
        or max_snapshots < 1
    ):
        raise ValueError("max_snapshots must be a positive integer")

    head_x = x_positions[:, head_index]
    head_y = y_positions[:, head_index]
    sample_count = min(max_snapshots, len(head_x))
    snapshot_indices = np.unique(
        np.linspace(0, len(head_x) - 1, sample_count).astype(int)
    )
    plot_x = head_x[snapshot_indices]
    plot_y = head_y[snapshot_indices]
    plot_times = times[snapshot_indices]

    fig, ax = plt.subplots(figsize=(5, 5))
    if len(plot_times) > 1 and np.nanmax(plot_times) > np.nanmin(plot_times):
        colours = plot_times
        colour_label = "Time (s)"
    else:
        colours = snapshot_indices
        colour_label = "Sample"

    if show_path:
        ax.plot(
            head_x,
            head_y,
            color="0.75",
            linewidth=line_width,
            zorder=1,
        )
    scatter = ax.scatter(
        plot_x,
        plot_y,
        c=colours,
        cmap="viridis",
        s=marker_size,
        edgecolors="none",
        zorder=2,
    )
    ax.scatter(
        plot_x[0],
        plot_y[0],
        color="black",
        s=marker_size * 2.0,
        label="start",
        zorder=3,
    )
    ax.scatter(
        plot_x[-1],
        plot_y[-1],
        color="red",
        s=marker_size * 2.0,
        label="end",
        zorder=3,
    )
    ax.set_title("Head trajectory")
    ax.set_xlabel("X Position (mm)")
    ax.set_ylabel("Y Position (mm)")
    ax.set_aspect("equal", adjustable="datalim")
    ax.grid(True, linewidth=0.4, alpha=0.25)
    ax.legend(frameon=False)
    cbar = fig.colorbar(scatter, ax=ax, fraction=0.046, pad=0.04)
    cbar.set_label(colour_label)
    fig.tight_layout()

    if save_png:
        output_file = filename
        if not os.path.isabs(output_file):
            output_file = os.path.join(output_folder, output_file)
        fig.savefig(output_file, bbox_inches="tight", dpi=300)

    return fig


def plot_trajectory_properties(
    input_folder,
    x_values=None,
    subfolders=None,
    properties=("speed", "curvature"),
    oscillation_cell_name=None,
    panel_columns=1,
    save_png=True,
    filename="trajectory_properties.png",
):
    """Extract average trajectory properties from simulation folders and plot them."""
    if not os.path.isdir(input_folder):
        raise NotADirectoryError(
            "Could not find input directory: {}".format(input_folder)
        )
    if isinstance(properties, str):
        properties = [properties]
    if not isinstance(properties, (list, tuple)) or not properties:
        raise ValueError("properties must be a non-empty list or tuple")
    if (
        isinstance(panel_columns, bool)
        or not isinstance(panel_columns, int)
        or panel_columns not in (1, 2)
    ):
        raise ValueError("panel_columns must be 1 or 2")

    valid_properties = {
        "speed",
        "curvature",
        "path_curvature",
        "path_deviation",
        "displacement",
        "oscillation_frequency",
    }
    properties = [str(prop) for prop in properties]
    unknown_properties = sorted(set(properties) - valid_properties)
    if unknown_properties:
        raise ValueError(
            "Unknown trajectory properties: {}".format(", ".join(unknown_properties))
        )

    input_folder = os.path.abspath(input_folder)

    def body_file_for_folder(folder):
        if "body.dat" in os.listdir(folder):
            return os.path.join(folder, "body.dat")
        if "bodypos.dat" in os.listdir(folder):
            return os.path.join(folder, "bodypos.dat")
        return None

    def curv_file_for_folder(folder):
        if "curv_t.dat" in os.listdir(folder):
            return os.path.join(folder, "curv_t.dat")
        if "curv.dat" in os.listdir(folder):
            return os.path.join(folder, "curv.dat")
        return None

    def act_file_for_folder(folder):
        act_file = os.path.join(folder, "act.dat")
        return act_file if os.path.isfile(act_file) else None

    def worm_json_file_for_folder(folder):
        for filename1 in ("worm_data_worm.json", "worm_data_evo.json", "worm_data.json"):
            worm_file = os.path.join(folder, filename1)
            if os.path.isfile(worm_file):
                return worm_file
        return None

    def choose_oscillation_cell(network_json_data):
        nervous_system = network_json_data.get("nervous_system")
        if not isinstance(nervous_system, dict):
            raise KeyError("JSON does not contain a 'nervous_system' object")
        cell_names = _json_value(nervous_system.get("cell_names"), [])
        if not isinstance(cell_names, list) or not cell_names:
            raise ValueError(
                "'nervous_system.cell_names.value' must be a non-empty list"
            )
        if oscillation_cell_name is not None:
            if oscillation_cell_name not in cell_names:
                raise ValueError(
                    "Oscillation cell {!r} was not found in {}".format(
                        oscillation_cell_name, cell_names
                    )
                )
            return oscillation_cell_name

        cells = nervous_system.get("cells", {})
        if isinstance(cells, dict):
            for cell_name in cell_names:
                cell = cells.get(cell_name, {})
                if (
                    isinstance(cell, dict)
                    and _normalise_cell_class_name(
                        _json_value(cell.get("cell_class"), "")
                    )
                    == "vnc"
                ):
                    return cell_name
        return cell_names[0]

    def estimate_oscillation_frequency(times, values):
        valid = np.isfinite(times) & np.isfinite(values)
        times = np.asarray(times[valid], dtype=float)
        values = np.asarray(values[valid], dtype=float)
        if times.size < 3 or times[-1] <= times[0]:
            return np.nan
        centered = values - np.nanmean(values)
        if not np.any(centered > 0) or not np.any(centered < 0):
            return np.nan

        crossing_times = []
        for index in range(len(centered) - 1):
            y0 = centered[index]
            y1 = centered[index + 1]
            if y0 < 0 <= y1 and y1 != y0:
                t0 = times[index]
                t1 = times[index + 1]
                crossing_times.append(t0 - y0 * (t1 - t0) / (y1 - y0))
        if len(crossing_times) < 2:
            return np.nan
        elapsed_time = crossing_times[-1] - crossing_times[0]
        if elapsed_time <= 0:
            return np.nan
        return (len(crossing_times) - 1) / elapsed_time

    simulation_folders = []
    if subfolders is None:
        for folder, child_subfolders, files in os.walk(input_folder):
            child_subfolders.sort()
            if folder == input_folder:
                continue
            if "body.dat" in files or "bodypos.dat" in files:
                simulation_folders.append(
                    (os.path.relpath(folder, input_folder), folder)
                )
        simulation_folders.sort(key=lambda item: item[0])
    else:
        if isinstance(subfolders, str) or not isinstance(subfolders, (list, tuple)):
            raise TypeError("subfolders must be a list or tuple of folder names")
        for subfolder in subfolders:
            if not isinstance(subfolder, str) or not subfolder:
                raise ValueError("subfolders must contain non-empty strings")
            folder = os.path.abspath(os.path.join(input_folder, subfolder))
            if os.path.commonpath([input_folder, folder]) != input_folder:
                raise ValueError(
                    "Subfolder {!r} is outside input_folder".format(subfolder)
                )
            if not os.path.isdir(folder):
                raise NotADirectoryError(
                    "Could not find simulation subfolder: {}".format(folder)
                )
            simulation_folders.append((os.path.relpath(folder, input_folder), folder))

    if not simulation_folders:
        raise FileNotFoundError(
            "No simulation subfolders with body.dat or bodypos.dat were found in {}".format(
                input_folder
            )
        )

    if x_values is None:
        x_values = list(range(len(simulation_folders)))
    if not isinstance(x_values, (list, tuple, np.ndarray)):
        raise TypeError("x_values must be a list, tuple, or numpy array")
    if len(x_values) != len(simulation_folders):
        raise ValueError("x_values must contain one value for each simulation folder")
    x_values = np.asarray(x_values, dtype=float)

    results = {
        "subfolders": [run_name for run_name, _ in simulation_folders],
        "x_values": x_values,
    }
    if "oscillation_frequency" in properties:
        results["oscillation_cell_name"] = []
    for prop in properties:
        results[prop] = []

    for run_name, folder in simulation_folders:
        body_file = body_file_for_folder(folder)
        if body_file is None:
            raise FileNotFoundError(
                "No body.dat or bodypos.dat file was found in {}".format(folder)
            )

        body_data = np.loadtxt(body_file)
        if body_data.ndim == 1:
            body_data = body_data.reshape(1, -1)
        if body_data.ndim != 2 or body_data.shape[1] < 4:
            raise ValueError(
                "{} does not contain time and x/y/z body coordinates".format(body_file)
            )
        coordinate_columns = body_data.shape[1] - 1
        if coordinate_columns % 3 != 0:
            raise ValueError(
                "{} has {} coordinate columns; expected a multiple of 3".format(
                    body_file, coordinate_columns
                )
            )

        segment_count = coordinate_columns // 3
        times = body_data[:, 0]
        x_positions = body_data[:, 1 : 1 + 3 * segment_count : 3] * 1000.0
        y_positions = body_data[:, 2 : 2 + 3 * segment_count : 3] * 1000.0
        center_x = np.nanmean(x_positions, axis=1)
        center_y = np.nanmean(y_positions, axis=1)

        if "speed" in properties:
            elapsed_time = times[-1] - times[0] if len(times) > 1 else np.nan
            if elapsed_time > 0:
                step_distances = np.sqrt(
                    np.diff(center_x) ** 2 + np.diff(center_y) ** 2
                )
                speed = np.nansum(step_distances) / elapsed_time
            else:
                speed = np.nan
            results["speed"].append(speed)

        if "displacement" in properties:
            displacement = np.hypot(
                center_x[-1] - center_x[0],
                center_y[-1] - center_y[0],
            )
            results["displacement"].append(displacement)

        if "path_curvature" in properties:
            chord_x = center_x[-1] - center_x[0]
            chord_y = center_y[-1] - center_y[0]
            chord_length = np.hypot(chord_x, chord_y)
            if chord_length > 0:
                lateral_distances = (
                    np.abs(
                        chord_x * (center_y - center_y[0])
                        - chord_y * (center_x - center_x[0])
                    )
                    / chord_length
                )
                path_curvature = np.nanmax(lateral_distances)
            else:
                path_curvature = np.nan
            results["path_curvature"].append(path_curvature)

        if "path_deviation" in properties:
            dx = np.diff(center_x)
            dy = np.diff(center_y)
            if len(dx) > 1:
                headings = np.unwrap(np.arctan2(dy, dx))
                heading_changes = np.abs(np.diff(headings))
                step_distances = np.sqrt(dx[1:] ** 2 + dy[1:] ** 2)
                path_length = np.nansum(step_distances)
                if path_length > 0:
                    path_deviation = np.nansum(heading_changes) / path_length
                else:
                    path_deviation = np.nan
            else:
                path_deviation = np.nan
            results["path_deviation"].append(path_deviation)

        if "curvature" in properties:
            curv_file = curv_file_for_folder(folder)
            if curv_file is None:
                curvature = np.nan
            else:
                curv_data = np.loadtxt(curv_file)
                if curv_data.ndim == 1:
                    curv_data = curv_data.reshape(1, -1)
                if curv_data.ndim != 2 or curv_data.shape[1] < 2:
                    raise ValueError(
                        "{} does not contain time and curvature columns".format(
                            curv_file
                        )
                    )
                curvature = np.nanmean(np.abs(curv_data[:, 1:]))
            results["curvature"].append(curvature)

        if "oscillation_frequency" in properties:
            act_file = act_file_for_folder(folder)
            if act_file is None:
                oscillation_frequency = np.nan
                cell_name = oscillation_cell_name
            else:
                worm_file = worm_json_file_for_folder(folder)
                if worm_file is None:
                    raise FileNotFoundError(
                        "No worm JSON file was found in {}".format(folder)
                    )
                network_json_data = utils.getJsonFile(worm_file)
                cell_name = choose_oscillation_cell(network_json_data)
                cell_names = _json_value(
                    network_json_data["nervous_system"].get("cell_names"), []
                )
                cell_index = cell_names.index(cell_name) + 1

                act_data = np.loadtxt(act_file)
                if act_data.ndim == 1:
                    act_data = act_data.reshape(1, -1)
                if act_data.ndim != 2 or act_data.shape[1] <= cell_index:
                    raise ValueError(
                        "{} does not contain activity data for cell {!r}".format(
                            act_file, cell_name
                        )
                    )
                oscillation_frequency = estimate_oscillation_frequency(
                    act_data[:, 0], act_data[:, cell_index]
                )
            results["oscillation_frequency"].append(oscillation_frequency)
            results["oscillation_cell_name"].append(cell_name)

    for prop in properties:
        results[prop] = np.asarray(results[prop], dtype=float)

    panel_columns = min(panel_columns, len(properties))
    panel_rows = math.ceil(len(properties) / panel_columns)
    fig, axs = plt.subplots(
        panel_rows,
        panel_columns,
        figsize=(7 * panel_columns, 3.2 * panel_rows),
        squeeze=False,
    )
    y_labels = {
        "speed": "Average speed (mm/s)",
        "curvature": "Average |curvature|",
        "path_curvature": "Path curvature (mm)",
        "path_deviation": "Path deviation (1/mm)",
        "displacement": "Displacement (mm)",
        "oscillation_frequency": "Oscillation frequency (Hz)",
    }
    flat_axes = axs.ravel()
    for ax, prop in zip(flat_axes, properties):
        ax.plot(x_values, results[prop], marker="o", linewidth=1.8)
        ax.set_ylabel(y_labels[prop])
        ax.grid(True, linewidth=0.4, alpha=0.25)
    for ax in flat_axes[len(properties) :]:
        ax.set_visible(False)
    for axis_index, ax in enumerate(flat_axes[: len(properties)]):
        row_index = axis_index // panel_columns
        if row_index == panel_rows - 1:
            ax.set_xlabel("Condition")
    fig.tight_layout()

    if save_png:
        output_file = filename
        if not os.path.isabs(output_file):
            output_file = os.path.join(input_folder, output_file)
        fig.savefig(output_file, bbox_inches="tight", dpi=300)

    return fig, results


def _compute_center_trajectory_properties(times, center_x, center_y, properties):
    results = {}
    if "speed" in properties:
        elapsed_time = times[-1] - times[0] if len(times) > 1 else np.nan
        if elapsed_time > 0:
            step_distances = np.sqrt(np.diff(center_x) ** 2 + np.diff(center_y) ** 2)
            results["speed"] = np.nansum(step_distances) / elapsed_time
        else:
            results["speed"] = np.nan

    if "displacement" in properties:
        results["displacement"] = np.hypot(
            center_x[-1] - center_x[0],
            center_y[-1] - center_y[0],
        )

    if "path_curvature" in properties:
        chord_x = center_x[-1] - center_x[0]
        chord_y = center_y[-1] - center_y[0]
        chord_length = np.hypot(chord_x, chord_y)
        if chord_length > 0:
            lateral_distances = (
                np.abs(
                    chord_x * (center_y - center_y[0])
                    - chord_y * (center_x - center_x[0])
                )
                / chord_length
            )
            results["path_curvature"] = np.nanmax(lateral_distances)
        else:
            results["path_curvature"] = np.nan

    if "path_deviation" in properties:
        dx = np.diff(center_x)
        dy = np.diff(center_y)
        if len(dx) > 1:
            headings = np.unwrap(np.arctan2(dy, dx))
            heading_changes = np.abs(np.diff(headings))
            step_distances = np.sqrt(dx[1:] ** 2 + dy[1:] ** 2)
            path_length = np.nansum(step_distances)
            if path_length > 0:
                results["path_deviation"] = np.nansum(heading_changes) / path_length
            else:
                results["path_deviation"] = np.nan
        else:
            results["path_deviation"] = np.nan
    return results


def _mean_abs_spine_curvature(x_positions, y_positions):
    if x_positions.ndim != 2 or y_positions.ndim != 2:
        raise ValueError("WCON x and y body coordinates must be two-dimensional")
    if x_positions.shape != y_positions.shape:
        raise ValueError("WCON x and y body coordinate arrays must have the same shape")
    if x_positions.shape[1] < 3:
        return np.nan

    x0 = x_positions[:, :-2]
    y0 = y_positions[:, :-2]
    x1 = x_positions[:, 1:-1]
    y1 = y_positions[:, 1:-1]
    x2 = x_positions[:, 2:]
    y2 = y_positions[:, 2:]

    a = np.hypot(x1 - x0, y1 - y0)
    b = np.hypot(x2 - x1, y2 - y1)
    c = np.hypot(x2 - x0, y2 - y0)
    twice_area = np.abs((x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0))
    denom = a * b * c
    curvature = np.full_like(twice_area, np.nan, dtype=float)
    valid = denom > 0
    curvature[valid] = 2.0 * twice_area[valid] / denom[valid]
    return np.nanmean(curvature)


def _wcon_unit_scale_to_mm(units, key):
    unit = str(units.get(key, "mm")).strip().lower()
    if unit in {"mm", "millimeter", "millimeters", "millimetre", "millimetres"}:
        return 1.0
    if unit in {"cm", "centimeter", "centimeters", "centimetre", "centimetres"}:
        return 10.0
    if unit in {"m", "meter", "meters", "metre", "metres"}:
        return 1000.0
    if unit in {"um", "micrometer", "micrometers", "micrometre", "micrometres"}:
        return 0.001
    return 1.0


def _wcon_unit_scale_to_seconds(units, key):
    unit = str(units.get(key, "s")).strip().lower()
    if unit in {"s", "sec", "second", "seconds"}:
        return 1.0
    if unit in {"ms", "millisecond", "milliseconds"}:
        return 0.001
    if unit in {"min", "minute", "minutes"}:
        return 60.0
    return 1.0


def _as_wcon_spine_array(values, name):
    array = np.asarray(values, dtype=float)
    if array.ndim == 1:
        array = array.reshape(-1, 1)
    if array.ndim != 2:
        raise ValueError("WCON {} values must be one- or two-dimensional".format(name))
    return array


def _wcon_records_for_file(wcon_file, worm_id=None):
    import json

    with open(wcon_file, "r") as file_obj:
        wcon = json.load(file_obj)

    units = wcon.get("units", {})
    data = wcon.get("data")
    if isinstance(data, dict):
        records = [data]
    elif isinstance(data, list):
        records = data
    else:
        raise ValueError("{} does not contain WCON data records".format(wcon_file))
    if not records:
        raise ValueError("{} contains no WCON data records".format(wcon_file))

    if worm_id is None:
        selected_id = records[0].get("id")
        selected_records = [record for record in records if record.get("id") == selected_id]
    else:
        selected_id = str(worm_id)
        selected_records = [record for record in records if str(record.get("id")) == selected_id]
    if not selected_records:
        raise ValueError(
            "WCON worm id {!r} was not found in {}".format(selected_id, wcon_file)
        )

    t_scale = _wcon_unit_scale_to_seconds(units, "t")
    x_scale = _wcon_unit_scale_to_mm(units, "x")
    y_scale = _wcon_unit_scale_to_mm(units, "y")

    times = []
    x_rows = []
    y_rows = []
    for record in selected_records:
        if not all(key in record for key in ("t", "x", "y")):
            raise ValueError("Each WCON record must contain t, x, and y")
        record_times = np.asarray(record["t"], dtype=float).reshape(-1) * t_scale
        record_x = _as_wcon_spine_array(record["x"], "x") * x_scale
        record_y = _as_wcon_spine_array(record["y"], "y") * y_scale
        if len(record_times) != record_x.shape[0] or len(record_times) != record_y.shape[0]:
            raise ValueError("WCON t, x, and y lengths do not match")
        times.extend(record_times.tolist())
        x_rows.extend(record_x.tolist())
        y_rows.extend(record_y.tolist())

    times = np.asarray(times, dtype=float)
    x_positions = np.asarray(x_rows, dtype=float)
    y_positions = np.asarray(y_rows, dtype=float)
    order = np.argsort(times)
    return selected_id, times[order], x_positions[order], y_positions[order]


def plot_wcon_trajectory_properties(
    wcon_files,
    x_values=None,
    worm_id=None,
    properties=("speed", "curvature"),
    panel_columns=1,
    save_png=True,
    filename="wcon_trajectory_properties.png",
):
    """Extract trajectory properties from one or more WCON files and plot them."""
    if isinstance(wcon_files, (str, os.PathLike)):
        wcon_files = [wcon_files]
    if not isinstance(wcon_files, (list, tuple)) or not wcon_files:
        raise ValueError("wcon_files must be a path or a non-empty list of paths")

    wcon_files = [os.fspath(wcon_file) for wcon_file in wcon_files]
    for wcon_file in wcon_files:
        if not os.path.isfile(wcon_file):
            raise FileNotFoundError("Could not find WCON file: {}".format(wcon_file))

    if isinstance(properties, str):
        properties = [properties]
    if not isinstance(properties, (list, tuple)) or not properties:
        raise ValueError("properties must be a non-empty list or tuple")
    valid_properties = {
        "speed",
        "curvature",
        "path_curvature",
        "path_deviation",
        "displacement",
    }
    properties = [str(prop) for prop in properties]
    unknown_properties = sorted(set(properties) - valid_properties)
    if unknown_properties:
        raise ValueError(
            "Unknown WCON trajectory properties: {}".format(
                ", ".join(unknown_properties)
            )
        )
    if (
        isinstance(panel_columns, bool)
        or not isinstance(panel_columns, int)
        or panel_columns not in (1, 2)
    ):
        raise ValueError("panel_columns must be 1 or 2")

    if x_values is None:
        x_values = list(range(len(wcon_files)))
    if not isinstance(x_values, (list, tuple, np.ndarray)):
        raise TypeError("x_values must be a list, tuple, or numpy array")
    if len(x_values) != len(wcon_files):
        raise ValueError("x_values must contain one value for each WCON file")
    x_values = np.asarray(x_values, dtype=float)

    results = {
        "wcon_files": wcon_files,
        "worm_ids": [],
        "x_values": x_values,
    }
    for prop in properties:
        results[prop] = []

    for wcon_file in wcon_files:
        selected_id, times, x_positions, y_positions = _wcon_records_for_file(
            wcon_file, worm_id
        )
        center_x = np.nanmean(x_positions, axis=1)
        center_y = np.nanmean(y_positions, axis=1)
        computed = _compute_center_trajectory_properties(
            times, center_x, center_y, properties
        )
        if "curvature" in properties:
            computed["curvature"] = _mean_abs_spine_curvature(
                x_positions, y_positions
            )
        for prop in properties:
            results[prop].append(computed.get(prop, np.nan))
        results["worm_ids"].append(selected_id)

    for prop in properties:
        results[prop] = np.asarray(results[prop], dtype=float)

    panel_columns = min(panel_columns, len(properties))
    panel_rows = math.ceil(len(properties) / panel_columns)
    fig, axs = plt.subplots(
        panel_rows,
        panel_columns,
        figsize=(7 * panel_columns, 3.2 * panel_rows),
        squeeze=False,
    )
    y_labels = {
        "speed": "Average speed (mm/s)",
        "curvature": "Average |body curvature| (1/mm)",
        "path_curvature": "Path curvature (mm)",
        "path_deviation": "Path deviation (1/mm)",
        "displacement": "Displacement (mm)",
    }
    flat_axes = axs.ravel()
    for ax, prop in zip(flat_axes, properties):
        ax.plot(x_values, results[prop], marker="o", linewidth=1.8)
        ax.set_ylabel(y_labels[prop])
        ax.grid(True, linewidth=0.4, alpha=0.25)
    for ax in flat_axes[len(properties) :]:
        ax.set_visible(False)
    for axis_index, ax in enumerate(flat_axes[: len(properties)]):
        row_index = axis_index // panel_columns
        if row_index == panel_rows - 1:
            ax.set_xlabel("Condition")
    fig.tight_layout()

    if save_png:
        output_file = filename
        if not os.path.isabs(output_file):
            output_file = os.path.join(os.path.dirname(wcon_files[0]), output_file)
        fig.savefig(output_file, bbox_inches="tight", dpi=300)

    return fig, results


def plot_cell_connections(
    output_folder=None,
    cell_names=None,
    save_png=False,
    filename="CellConnections.png",
    hide_self_connections=True,
    primary_rotation=0.0,
    secondary_rotation=None,
    external_rotation=None,
    symbol_text_scale=1.0,
    node_scale=None,
    text_scale=None,
    highlight_cell_pairs=None,
    highlight_cells=None,
    arrow_thickness_scale=1.0,
    evotag=None,
    json_data=None,
):
    """Return a figure showing selected cells and connected model objects.

    Model data can be loaded from output_folder/worm_data_worm.json, supplied
    directly with json_data, or both. If both are supplied, json_data is used
    for the plot and output_folder is used only as the save location.
    """
    from matplotlib.lines import Line2D
    from matplotlib.patches import FancyArrowPatch

    if isinstance(output_folder, dict) and json_data is None:
        json_data = output_folder
        output_folder = None
    if output_folder is not None:
        output_folder = os.fspath(output_folder)
    if json_data is None and output_folder is None:
        raise ValueError("Either output_folder or json_data must be supplied")
    if save_png and output_folder is None:
        raise ValueError("output_folder is required when save_png is True")
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
        ("arrow_thickness_scale", arrow_thickness_scale),
    ):
        if (
            isinstance(scale_value, bool)
            or not isinstance(scale_value, (int, float))
            or scale_value <= 0
        ):
            raise ValueError("{} must be a positive number".format(scale_name))

    if json_data is not None:
        if not isinstance(json_data, dict):
            raise TypeError("json_data must be a dictionary")
        network_json_data = json_data
    else:
        worm_file = os.path.join(output_folder, "worm_data_worm.json")
        if not os.path.isfile(worm_file):
            raise FileNotFoundError(
                "Could not find worm_data_worm.json in {}".format(output_folder)
            )
        network_json_data = utils.getJsonFile(worm_file)
    if evotag is not None and (not isinstance(evotag, str) or not evotag):
        raise ValueError("evotag must be a non-empty string")

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
    cell_name_set = set(json_cell_names)

    highlighted_cells = set()
    if highlight_cells is not None:
        if isinstance(highlight_cells, str):
            highlighted_cells = {highlight_cells}
        elif isinstance(highlight_cells, (list, tuple, set)):
            highlighted_cells = {str(cell_name) for cell_name in highlight_cells}
        else:
            raise TypeError(
                "highlight_cells must be a cell name or a list/tuple/set of cell names"
            )
        unknown_highlighted_cells = sorted(highlighted_cells - cell_name_set)
        if unknown_highlighted_cells:
            raise ValueError(
                "Unknown highlighted cell name(s): {}".format(
                    ", ".join(unknown_highlighted_cells)
                )
            )

    highlighted_pairs = set()
    if highlight_cell_pairs is not None:
        if not isinstance(highlight_cell_pairs, (list, tuple)):
            raise TypeError("highlight_cell_pairs must be a list or tuple of pairs")
        for pair in highlight_cell_pairs:
            if not isinstance(pair, (list, tuple)) or len(pair) != 2:
                raise ValueError("Each highlighted cell pair must contain two names")
            from_cell, to_cell = str(pair[0]), str(pair[1])
            unknown = [
                cell_name
                for cell_name in (from_cell, to_cell)
                if cell_name not in cell_name_set
            ]
            if unknown:
                raise ValueError(
                    "Unknown highlighted cell name(s): {}".format(", ".join(unknown))
                )
            highlighted_pairs.add((from_cell, to_cell))

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

    def object_has_evotag(value):
        if evotag is None:
            return False
        if isinstance(value, dict):
            if value.get("evotag") == evotag:
                return True
            return any(object_has_evotag(child) for child in value.values())
        if isinstance(value, list):
            return any(object_has_evotag(child) for child in value)
        return False

    def object_without_keys(value, excluded_keys):
        if not isinstance(value, dict):
            return value
        return {key: child for key, child in value.items() if key not in excluded_keys}

    cells_object = nervous_system.get("cells", {})
    nodes = {
        cell_name: {
            "type": "cell",
            "evotag_highlighted": object_has_evotag(
                cells_object.get(cell_name, {})
                if isinstance(cells_object, dict)
                else {}
            ),
        }
        for cell_name in selected_cells
    }
    edges = []

    def add_node(node_name, node_type, evotag_highlighted=False):
        node_name = str(node_name)
        if node_name not in nodes:
            nodes[node_name] = {
                "type": node_type,
                "evotag_highlighted": bool(evotag_highlighted),
            }
            return
        nodes[node_name]["evotag_highlighted"] = nodes[node_name].get(
            "evotag_highlighted", False
        ) or bool(evotag_highlighted)

    def add_edge(
        from_node,
        to_node,
        weight,
        edge_type,
        bidirectional=False,
        highlighted=False,
        evotag_highlighted=False,
    ):
        if hide_self_connections and str(from_node) == str(to_node):
            return
        edges.append(
            {
                "from": str(from_node),
                "to": str(to_node),
                "weight": float(weight),
                "type": edge_type,
                "bidirectional": bidirectional,
                "highlighted": bool(highlighted),
                "evotag_highlighted": bool(evotag_highlighted),
            }
        )

    secondary_cells = []

    def add_secondary_cell(cell_name):
        if cell_name in selected_set or cell_name not in cell_name_set:
            return
        if cell_name not in nodes:
            nodes[cell_name] = {
                "type": "secondary cell",
                "evotag_highlighted": object_has_evotag(
                    cells_object.get(cell_name, {})
                    if isinstance(cells_object, dict)
                    else {}
                ),
            }
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
                highlighted=(connection["from"], connection["to"]) in highlighted_pairs,
                evotag_highlighted=object_has_evotag(connection),
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
                evotag_highlighted=object_has_evotag(connection),
            )

    stretch_receptor = network_json_data.get("stretch_receptor")
    if isinstance(stretch_receptor, dict):
        stretch_receptor_parameter_highlight = object_has_evotag(
            {
                key: value
                for key, value in stretch_receptor.items()
                if not key.endswith("_weights")
                and key not in ("d_weights", "v_weights")
            }
        )
        receptor_weight_fields = []
        for weights_key in stretch_receptor:
            if not weights_key.startswith("ns_") or not weights_key.endswith(
                "_weights"
            ):
                continue
            field_parts = weights_key[len("ns_") : -len("_weights")].split("_")
            if not field_parts or field_parts[-1] not in ("d", "v"):
                continue
            side = field_parts[-1].upper()
            receptor_class = "_".join(field_parts[:-1]).upper()
            prefix = "{}_SR".format(side)
            if receptor_class:
                prefix = "{}_{}".format(prefix, receptor_class)
            receptor_weight_fields.append((weights_key, prefix))

        for weights_key, prefix in receptor_weight_fields:
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
                add_node(
                    from_node,
                    "stretch receptor",
                    evotag_highlighted=stretch_receptor_parameter_highlight,
                )
                add_edge(
                    from_node,
                    to_cell,
                    connection_weight(connection),
                    "stretch receptor",
                    evotag_highlighted=object_has_evotag(connection),
                )

    sensors = network_json_data.get("sensors")
    if isinstance(sensors, dict):
        for sensor_name, sensor in sensors.items():
            if not isinstance(sensor, dict):
                continue
            sensor_parameter_highlight = object_has_evotag(
                object_without_keys(sensor, {"weights"})
            )
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
                add_node(
                    from_node,
                    "sensor",
                    evotag_highlighted=sensor_parameter_highlight,
                )
                add_edge(
                    from_node,
                    to_cell,
                    connection_weight(connection),
                    "sensor",
                    evotag_highlighted=object_has_evotag(connection),
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
                add_node(
                    from_node,
                    "driving input",
                    evotag_highlighted=object_has_evotag(
                        object_without_keys(driving_inputs, {"weights"})
                    ),
                )
                add_edge(
                    from_node,
                    to_cell,
                    connection_weight(connection),
                    "driving input",
                    evotag_highlighted=object_has_evotag(connection),
                )

    for nmj_key, prefix in (("dorsal_nmj", "D_musc"), ("ventral_nmj", "V_musc")):
        nmj = network_json_data.get(nmj_key)
        if not isinstance(nmj, dict):
            continue
        nmj_parameter_highlight = object_has_evotag(
            object_without_keys(nmj, {"weights"})
        )
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
            add_node(to_node, "muscle", evotag_highlighted=nmj_parameter_highlight)
            add_edge(
                from_cell,
                to_node,
                connection_weight(connection),
                "muscle",
                evotag_highlighted=object_has_evotag(connection),
            )

    displayed_weights = [abs(edge["weight"]) for edge in edges]
    max_abs_weight = max(displayed_weights) if displayed_weights else 1.0
    if max_abs_weight <= 0:
        max_abs_weight = 1.0

    def connection_width(edge):
        scaled = abs(edge["weight"]) / max_abs_weight
        return (0.6 + 3.4 * scaled) * arrow_thickness_scale

    fig_size = max(6.0, 0.35 * len(nodes) + 3.5)
    fig, ax = plt.subplots(figsize=(fig_size, fig_size))
    ax.set_aspect("equal")
    ax.axis("off")

    primary_rotation_rad = np.deg2rad(primary_rotation)
    if len(selected_cells) == 1:
        positions = {selected_cells[0]: np.array([0.0, 0.0])}
    else:
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

    def external_node_order(node_name):
        if str(node_name).startswith("D_SR_"):
            return (1, str(node_name))
        if str(node_name).startswith("D_musc_"):
            return (2, str(node_name))
        if str(node_name).startswith("V_SR_"):
            return (3, str(node_name))
        if str(node_name).startswith("V_musc_"):
            return (4, str(node_name))
        return (0, str(node_name))

    external_nodes = sorted(external_nodes, key=external_node_order)
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
        if edge.get("evotag_highlighted"):
            return "#D000FF"
        if edge.get("highlighted") and edge["type"] == "chemical":
            return "#00C853" if edge["weight"] >= 0 else "#000000"
        if edge["type"] == "electrical":
            return "#2E8B57"
        return "#D95F02" if edge["weight"] >= 0 else "#7570B3"

    def draw_edge(edge, rad, alpha=0.85):
        from_cell = edge["from"]
        to_cell = edge["to"]
        linewidth = connection_width(edge)
        color = edge_color(edge)
        linestyle = "--" if edge["type"] == "electrical" else "-"
        arrowstyle = "-" if edge["type"] == "electrical" else "-|>"
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
        evotag_node_highlighted = nodes[node_name].get("evotag_highlighted", False)
        ax.scatter(
            [position[0]],
            [position[1]],
            s=style["size"] * marker_area_scale,
            marker=style["marker"],
            facecolors=(
                "#FFD600"
                if evotag_node_highlighted
                else "#00E676"
                if node_name in highlighted_cells
                and node_type in ("cell", "secondary cell")
                else style["facecolor"]
            ),
            edgecolors="#D000FF" if evotag_node_highlighted else style["edgecolor"],
            linewidths=2.0 if evotag_node_highlighted else 1.2,
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
            color="#2E8B57",
            linewidth=1.8,
            linestyle="--",
            label="Electrical",
        ),
        Line2D([0], [0], color="0.25", linewidth=0.8, label="Weak"),
        Line2D([0], [0], color="0.25", linewidth=4.0, label="Strong"),
    ]
    if evotag is not None and any(edge.get("evotag_highlighted") for edge in edges):
        edge_legend.append(
            Line2D([0], [0], color="#D000FF", linewidth=3.0, label=evotag)
        )
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
    if any(node_name in highlighted_cells for node_name in nodes):
        node_legend.append(
            Line2D(
                [0],
                [0],
                marker="o",
                color="none",
                markerfacecolor="#00E676",
                markeredgecolor="black",
                markersize=8 * node_scale,
                label="Highlighted Cell",
            )
        )
    if evotag is not None and any(
        nodes[node].get("evotag_highlighted") for node in nodes
    ):
        node_legend.append(
            Line2D(
                [0],
                [0],
                marker="o",
                color="none",
                markerfacecolor="#FFD600",
                markeredgecolor="#D000FF",
                markersize=8 * node_scale,
                label=evotag,
            )
        )
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

    def stretch_receptor_weight_count(section, side, to_nervous_system=False):
        if not isinstance(section, dict):
            return 0
        direct_key = "{}{}_weights".format("ns_" if to_nervous_system else "", side)
        keys = {direct_key}
        suffix = "_{}_weights".format(side)
        for key in section:
            if to_nervous_system:
                if key.startswith("ns_") and key.endswith(suffix):
                    keys.add(key)
            elif not key.startswith("ns_") and key.endswith(suffix):
                keys.add(key)
        return sum(len(values_list(section.get(key, {}))) for key in keys)

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
    dorsal_body = network_json_data.get("dorsal_body")
    ventral_body = network_json_data.get("ventral_body")
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
        ns_d = stretch_receptor_weight_count(sr, "d", to_nervous_system=True)
        ns_v = stretch_receptor_weight_count(sr, "v", to_nervous_system=True)
        d_body = stretch_receptor_weight_count(sr, "d")
        v_body = stretch_receptor_weight_count(sr, "v")
        dorsal_muscle_body = weight_count(dorsal_body)
        ventral_muscle_body = weight_count(ventral_body)
        sr_type = _json_value(sr.get("type"))
        sr_count = _json_value(sr.get("plot_size"))
        if not isinstance(sr_count, (int, float)):
            sr_count = _json_value(sr.get("n_stretch"))
        sr_lines = []
        if sr_type in ("SR18", "SRCE"):
            sr_lines.append("type: {}".format(sr_type))
        if isinstance(sr_count, (int, float)):
            sr_lines.append("{} receptors".format(int(sr_count)))
        sr_lines.extend(
            [
                "{} dorsal-to-NS weights".format(ns_d),
                "{} ventral-to-NS weights".format(ns_v),
            ]
        )
        boxes["stretch_receptor"] = {
            "xy": (0.04, 0.58),
            "wh": (0.19, 0.23),
            "text": box_text("stretch_receptor", sr_lines),
            "facecolor": "#F1E5A6",
        }
        if d_body or v_body or dorsal_muscle_body or ventral_muscle_body:
            body_lines = []
            if dorsal_muscle_body:
                body_lines.append("{} dorsal muscle weights".format(dorsal_muscle_body))
            if ventral_muscle_body:
                body_lines.append(
                    "{} ventral muscle weights".format(ventral_muscle_body)
                )
            if d_body:
                body_lines.append("{} dorsal SR weights".format(d_body))
            if v_body:
                body_lines.append("{} ventral SR weights".format(v_body))
            boxes["body_segments"] = {
                "xy": (0.56, 0.65),
                "wh": (0.18, 0.16),
                "text": box_text("body segments", body_lines),
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


def safe_percent_change_from_initial(evol_data):
    evol_data = np.asarray(evol_data, dtype=float)
    initial = evol_data[0]
    initial_scale = np.abs(initial)
    out = np.zeros_like(evol_data, dtype=float)
    np.divide(
        evol_data - initial,
        initial_scale,
        out=out,
        where=np.isfinite(initial_scale) & (initial_scale != 0),
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
    evol_data_diff_1 = safe_percent_change_from_initial(evol_data)
    evol_data_diff_11 = signed_log(evol_data_diff_1)
    evol_data_diff_13 = evol_data - evol_data[0]
    evol_data_diff_131 = signed_log(evol_data_diff_13[1:])

    return [evol_data_diff_13, evol_data_diff_131, evol_data_diff_1, evol_data_diff_11]


def plot_phenonames(
    plot_list=[["initial", "final"], ["initial_log", "final_log"], "rel_var", "var"],
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

    evol_data_full_diff0 = safe_percent_change_from_initial(evol_data)
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
            "title": "Proportional change",
            "color": "black",
            "linestyle": "-",
        },
        "var": {
            "value": evol_data_full_diff,
            "title": "Signed log proportional variation",
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
    plot_data_3av = getAvData_1(plot_data_1[3], avlentop=avlentop)
    plot_data_0av = getAvData_1(plot_data_1[0], avlentop=avlentop)
    plot_data_best_percent_av = getAvData_1(plot_data_2[3], avlentop=avlentop)

    # plot_data_3 = [plot_data_1[0], plot_data_1[3], plot_data_1[4], plot_data_2[3]]
    plot_data_3 = [
        plot_data_3av,
        plot_data_4av,
        plot_data_0av,
        plot_data_best_percent_av,
    ]

    # gen_indices = [gen_index_orig, gen_index_orig, gen_index_orig, gen_index_orig]

    gen_indices = [
        gen_index_orig_av,
        gen_index_orig_av,
        gen_index_orig_av,
        gen_index_orig_av,
    ]

    titles = [
        "Pop proportional change",
        "Pop signed log proportional variation",
        "Pop phenotype value",
        "Best fit proportional change",
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

    def set_imshow_row_ticks(ax, row_count, max_labels=6):
        if row_count <= 0:
            ax.set_yticks([])
            return
        if row_count <= max_labels:
            rows = np.arange(row_count)
        else:
            rows = np.unique(
                np.rint(np.linspace(0, row_count - 1, max_labels)).astype(int)
            )
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

    from F2_fig_behavior import make_fig

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
        make_fig(model_name=a.modelName)


if __name__ == "__main__":
    import sys

    reload_single_run(showPlot=False, modelName="Net21", folderName=sys.argv[1])
