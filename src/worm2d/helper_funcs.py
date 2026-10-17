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


DEFAULTS = {
    "modelName": None,
    "showPlot": True,
    "folderName": None,
    "verbose": False,
    "compactActivityTimeLabels": False,
    "showExampleActivityBottomLeftPanel": True,
}

MFUNC_NAMES = {
    1: "mult_func",
    2: "zero_func",
}
MFUNC_INDICES = {value: key for key, value in MFUNC_NAMES.items()}


def _mfunc_name(function_id):
    if isinstance(function_id, bool):
        raise TypeError("mfunc function identifier must be an integer or string")
    if isinstance(function_id, int):
        if function_id not in MFUNC_NAMES:
            raise ValueError("Unknown mfunc function index {}".format(function_id))
        return MFUNC_NAMES[function_id]
    if isinstance(function_id, str) and function_id:
        if function_id not in MFUNC_INDICES:
            raise ValueError("Unknown mfunc function name {!r}".format(function_id))
        return function_id
    raise TypeError("mfunc function identifier must be an integer or string")


def _wrapped_json_value(obj, default=None):
    if isinstance(obj, dict) and "value" in obj:
        return obj["value"]
    if obj is None:
        return default
    return obj


def _get_muscle_count(json_data):
    worm = json_data.get("worm")
    if isinstance(worm, dict):
        muscle_count = _wrapped_json_value(worm.get("N_muscles"))
        if isinstance(muscle_count, int) and not isinstance(muscle_count, bool):
            return muscle_count

    muscle = json_data.get("Muscle")
    if isinstance(muscle, dict):
        muscle_count = _wrapped_json_value(muscle.get("Nmuscles"))
        if isinstance(muscle_count, int) and not isinstance(muscle_count, bool):
            return muscle_count

    raise TypeError("'worm.N_muscles.value' must be an integer")


def get_worm2d_version():
    """Return the installed Worm2D package version."""
    try:
        from importlib.metadata import version

        return "v" + version("worm2d")
    except Exception:
        from . import __version__

        return "v" + __version__


def short_repr(x, max_len=80):
    text = json.dumps(x, ensure_ascii=False)
    if len(text) > max_len:
        text = text[:max_len] + "..."
    return html.escape(text)


def _driving_input_id(input_num):
    if isinstance(input_num, bool):
        raise TypeError("Driving input identifiers cannot be boolean values")
    if isinstance(input_num, int):
        if input_num < 1:
            raise ValueError("Driving input numbers must be at least 1")
        return "input_{}".format(input_num)
    if isinstance(input_num, str) and input_num.strip():
        return input_num.strip()
    raise TypeError("Driving input identifiers must be strings or positive integers")


def _driving_input_index(input_id):
    if isinstance(input_id, bool):
        raise TypeError("Driving input identifiers cannot be boolean values")
    if isinstance(input_id, int):
        if input_id < 1:
            raise ValueError("Driving input numbers must be at least 1")
        return input_id - 1
    if isinstance(input_id, str) and input_id.startswith("input_"):
        suffix = input_id[len("input_") :]
        if suffix.isdigit() and int(suffix) > 0:
            return int(suffix) - 1
    raise ValueError(
        "Cannot infer a numeric vector index from driving input {!r}".format(input_id)
    )


def _input_switcher_id(input_index):
    if isinstance(input_index, bool):
        raise TypeError("input_index cannot be a boolean value")
    if isinstance(input_index, int):
        if input_index < 0:
            raise ValueError("input_index must be non-negative")
        return "input_pattern_{}".format(input_index)
    if isinstance(input_index, str) and input_index.strip():
        return input_index.strip()
    raise TypeError("input_index must be a string or non-negative integer")


def _input_switcher_index(input_index):
    if isinstance(input_index, bool):
        raise TypeError("input_index cannot be a boolean value")
    if isinstance(input_index, int):
        if input_index < 0:
            raise ValueError("input_index must be non-negative")
        return input_index
    if isinstance(input_index, str) and input_index.startswith("input_pattern_"):
        suffix = input_index[len("input_pattern_") :]
        if suffix.isdigit():
            return int(suffix)
    raise ValueError(
        "Cannot infer a numeric pattern index from input_index {!r}".format(input_index)
    )


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


def _json_tree_contains(value, query, key=None):
    if key is not None and query in str(key).lower():
        return True
    if isinstance(value, dict):
        return any(
            _json_tree_contains(child, query, child_key)
            for child_key, child in value.items()
        )
    if isinstance(value, list):
        return any(_json_tree_contains(child, query) for child in value)
    return query in json.dumps(value, ensure_ascii=False).lower()


def _json_tree_scalar_html(value):
    if value is None:
        css_class = "json-null"
    elif isinstance(value, bool):
        css_class = "json-boolean"
    elif isinstance(value, (int, float)):
        css_class = "json-number"
    else:
        css_class = "json-string"
    text = html.escape(json.dumps(value, ensure_ascii=False))
    return '<span class="{}">{}</span>'.format(css_class, text)


def _json_tree_preview(value, preview_items):
    if isinstance(value, dict):
        if not value:
            return '<span class="json-bracket">{}</span>'
        items = list(value.items())
        visible_items = items if preview_items is None else items[:preview_items]
        parts = []
        for key, child in visible_items:
            parts.append(
                '<span class="json-key">{}</span><span class="json-punctuation">:'
                "</span> {}".format(html.escape(str(key)), _json_tree_preview(child, 0))
            )
        if len(visible_items) < len(items):
            parts.append('<span class="json-ellipsis">...</span>')
        return (
            '<span class="json-bracket">{{</span> {} '
            '<span class="json-bracket">}}</span>'
        ).format(", ".join(parts))
    if isinstance(value, list):
        if not value:
            return '<span class="json-bracket">[]</span>'
        return (
            '<span class="json-count">({})</span> '
            '<span class="json-bracket">[</span><span class="json-ellipsis">'
            '...</span><span class="json-bracket">]</span>'
        ).format(len(value))
    return _json_tree_scalar_html(value)


def _json_tree_html(value, expanded_depth, list_preview, query, depth=0):
    force_open = bool(query) or depth < expanded_depth
    open_attribute = " open" if force_open else ""

    if isinstance(value, dict):
        if not value:
            return '<span class="json-empty">{}</span>'
        rows = []
        for key, child in value.items():
            key_matches = bool(query) and query in str(key).lower()
            if query and not _json_tree_contains(child, query, key):
                continue
            child_query = "" if key_matches else query
            key_html = html.escape(str(key))
            if isinstance(child, (dict, list)):
                child_html = _json_tree_html(
                    child,
                    expanded_depth,
                    list_preview,
                    child_query,
                    depth + 1,
                )
                rows.append(
                    '<details class="json-node"{}><summary><span '
                    'class="json-key">{}</span><span class="json-punctuation">:'
                    '</span> <span class="json-preview">{}</span></summary>'
                    '<div class="json-children">'
                    "{}</div></details>".format(
                        open_attribute,
                        key_html,
                        _json_tree_preview(child, list_preview),
                        child_html,
                    )
                )
            else:
                rows.append(
                    '<div class="json-leaf"><span class="json-key">{}</span>'
                    ": {}</div>".format(key_html, _json_tree_scalar_html(child))
                )
        return "".join(rows) or '<div class="json-no-match">No matches</div>'

    if isinstance(value, list):
        if not value:
            return '<span class="json-empty">[]</span>'
        matching_items = [
            (index, child)
            for index, child in enumerate(value)
            if not query or _json_tree_contains(child, query)
        ]
        if not matching_items:
            return '<div class="json-no-match">No matches</div>'

        def render_items(items):
            rendered = []
            for index, child in items:
                index_html = "[{}]".format(index)
                if isinstance(child, (dict, list)):
                    child_html = _json_tree_html(
                        child,
                        expanded_depth,
                        list_preview,
                        query,
                        depth + 1,
                    )
                    rendered.append(
                        '<details class="json-node"{}><summary><span '
                        'class="json-index">{}</span><span '
                        'class="json-punctuation">:</span> <span '
                        'class="json-preview">{}</span></summary>'
                        '<div class="json-children">'
                        "{}</div></details>".format(
                            open_attribute,
                            index_html,
                            _json_tree_preview(child, list_preview),
                            child_html,
                        )
                    )
                else:
                    rendered.append(
                        '<div class="json-leaf"><span class="json-index">{}</span>'
                        " {}</div>".format(index_html, _json_tree_scalar_html(child))
                    )
            return "".join(rendered)

        return render_items(matching_items)

    return _json_tree_scalar_html(value)


def display_json_tree(
    json_data,
    expanded_depth=0,
    list_preview=3,
    searchable=True,
):
    """Return a searchable, partially expanded JSON tree for a notebook.

    Parameters
    ----------
    json_data : dict or list
        JSON-compatible data to display.
    expanded_depth : int, default 0
        Deepest hierarchy level initially opened. Searching temporarily opens
        all branches containing matches.
    list_preview : int or None, default 3
        Maximum number of fields included in each collapsed inline preview.
        ``None`` includes every field. Opening a branch always shows all of its
        children.
    searchable : bool, default True
        Include a live search field that matches keys and scalar values.

    Returns
    -------
    ipywidgets.Widget
        Display it as the final expression in a notebook cell, or pass it to
        ``IPython.display.display``.
    """
    if widgets is None:
        raise ImportError("ipywidgets is required to display JSON trees")
    if not isinstance(json_data, (dict, list)):
        raise TypeError("json_data must be a dictionary or list")
    if isinstance(expanded_depth, bool) or not isinstance(expanded_depth, int):
        raise TypeError("expanded_depth must be a non-negative integer")
    if expanded_depth < 0:
        raise ValueError("expanded_depth must be non-negative")
    if list_preview is not None:
        if isinstance(list_preview, bool) or not isinstance(list_preview, int):
            raise TypeError("list_preview must be a non-negative integer or None")
        if list_preview < 0:
            raise ValueError("list_preview must be non-negative")
    if not isinstance(searchable, bool):
        raise TypeError("searchable must be a boolean")

    styles = """
    <style>
    .worm2d-json-tree {font-family: ui-monospace, SFMono-Regular, Menlo, Monaco,
        Consolas, monospace; font-size: 15px; line-height: 1.55;
        border-top: 1px solid #d7d7d7; border-bottom: 1px solid #d7d7d7;
        padding: 6px 0; max-height: 680px; overflow: auto; background: #fff;
        color: #006fe6;}
    .worm2d-json-tree details {margin: 0;}
    .worm2d-json-tree summary {cursor: pointer; white-space: nowrap;
        padding: 1px 8px;}
    .worm2d-json-tree summary:hover {background: #eef6ff;}
    .worm2d-json-tree summary:focus {background: #087fe5; color: #fff;
        outline: none;}
    .worm2d-json-tree summary:focus span {color: #fff;}
    .worm2d-json-tree details[open] > summary > .json-preview {display: none;}
    .worm2d-json-tree .json-children {margin-left: 18px; padding-left: 7px;}
    .worm2d-json-tree .json-leaf {min-height: 22px; padding-left: 8px;
        white-space: pre-wrap;}
    .worm2d-json-tree .json-key, .worm2d-json-tree .json-index,
    .worm2d-json-tree .json-bracket, .worm2d-json-tree .json-punctuation {
        color: #006fe6;}
    .worm2d-json-tree .json-count, .worm2d-json-tree .json-empty,
    .worm2d-json-tree .json-ellipsis {color: #6f7780;}
    .worm2d-json-tree .json-string {color: #ec008c;}
    .worm2d-json-tree .json-number {color: #006fe6;}
    .worm2d-json-tree .json-boolean {color: #7b3fc6; font-weight: 600;}
    .worm2d-json-tree .json-null {color: #cf222e;}
    .worm2d-json-tree .json-no-match {color: #6e7781; font-style: italic;}
    </style>
    """
    tree = widgets.HTML()

    def update_tree(query=""):
        normalized_query = query.strip().lower()
        body = _json_tree_html(
            json_data,
            expanded_depth,
            list_preview,
            normalized_query,
        )
        tree.value = '{}<div class="worm2d-json-tree">{}</div>'.format(styles, body)

    update_tree()
    if not searchable:
        return tree

    search = widgets.Text(
        placeholder="Search keys and values",
        description="Search:",
        continuous_update=True,
        layout=widgets.Layout(width="100%"),
    )

    def on_search(change):
        if change.get("name") == "value":
            update_tree(change.get("new", ""))

    search.observe(on_search, names="value")
    return widgets.VBox([search, tree], layout=widgets.Layout(width="100%"))


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


def _normalize_json_section_names(json_data):
    if isinstance(json_data, dict) and "Simulation" in json_data:
        if "simulation" not in json_data:
            json_data["simulation"] = json_data["Simulation"]
        del json_data["Simulation"]
    nervous_system = (
        json_data.get("nervous_system") if isinstance(json_data, dict) else None
    )
    if isinstance(nervous_system, dict):
        nervous_system.pop("cell_names_no_suffix", None)


def write_worm_json(folder_name, json_data):
    """Write worm_data_worm.json to a run directory."""
    os.makedirs(folder_name, exist_ok=True)
    _normalize_json_section_names(json_data)
    filename = os.path.join(folder_name, "worm_data_worm.json")
    with open(filename, "w") as f:
        json.dump(json_data, f)


def _json_path_string(path):
    parts = []
    for item in path:
        if isinstance(item, int):
            parts.append("[{}]".format(item))
        else:
            if parts:
                parts.append(".")
            parts.append(str(item))
    return "".join(parts)


def _is_number(value):
    return isinstance(value, (int, float)) and not isinstance(value, bool)


def _is_finite_number(value):
    return _is_number(value) and math.isfinite(value)


def _collect_evotag_uses(value, path=()):
    uses = []
    if isinstance(value, dict):
        evotag = value.get("evotag")
        if isinstance(evotag, str) and evotag:
            uses.append((evotag, path))
        for key, child in value.items():
            if key in {"evolvable_ranges", "evolved_used"}:
                continue
            uses.extend(_collect_evotag_uses(child, path + (key,)))
    elif isinstance(value, list):
        for index, child in enumerate(value):
            uses.extend(_collect_evotag_uses(child, path + (index,)))
    return uses


def validate_w2dsr_json(json_data, source_name=None):
    """Validate a W2DSR JSON dictionary before it is sent to the C++ model.

    Validation covers the nervous system, optional external inputs and sensors,
    muscle/body and stretch-receptor connections, schedules, mfunc objects, and
    evolvable-range consistency. All detected problems are reported together.
    """
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")

    errors = []
    location = " in {}".format(source_name) if source_name else ""

    def check_finite_numbers(value, path=()):
        if isinstance(value, dict):
            for key, child in value.items():
                check_finite_numbers(child, path + (key,))
        elif isinstance(value, list):
            for index, child in enumerate(value):
                check_finite_numbers(child, path + (index,))
        elif _is_number(value) and not math.isfinite(value):
            errors.append("'{}' must be finite".format(_json_path_string(path)))

    def check_mfuncs(value, path=()):
        if isinstance(value, dict):
            for field_name in ("mfunc", "m_func"):
                if field_name not in value:
                    continue
                mfunc_path = path + (field_name,)
                mfunc = value[field_name]
                if field_name == "m_func":
                    errors.append(
                        "'{}' must be named 'mfunc'".format(
                            _json_path_string(mfunc_path)
                        )
                    )
                if "value" not in value:
                    errors.append(
                        "'{}' must belong to an object containing 'value'".format(
                            _json_path_string(mfunc_path)
                        )
                    )
                if not isinstance(mfunc, dict):
                    errors.append(
                        "'{}' must be an object".format(_json_path_string(mfunc_path))
                    )
                    continue
                if "f_ind" not in mfunc:
                    errors.append(
                        "'{}' is missing key 'f_ind'".format(
                            _json_path_string(mfunc_path)
                        )
                    )
                    continue
                try:
                    function_name = _mfunc_name(mfunc["f_ind"])
                except (TypeError, ValueError) as exc:
                    errors.append(
                        "'{}.f_ind' is invalid: {}".format(
                            _json_path_string(mfunc_path), exc
                        )
                    )
                    continue
                if function_name == "mult_func":
                    factor = mfunc.get("fact")
                    if not _is_finite_number(factor):
                        errors.append(
                            "'{}.fact' must be a finite number".format(
                                _json_path_string(mfunc_path)
                            )
                        )
                    elif factor == 0:
                        errors.append(
                            "'{}.fact' must be non-zero".format(
                                _json_path_string(mfunc_path)
                            )
                        )
                elif function_name == "zero_func":
                    condition = mfunc.get("cond")
                    if isinstance(condition, bool) or not isinstance(condition, int):
                        errors.append(
                            "'{}.cond' must be an integer".format(
                                _json_path_string(mfunc_path)
                            )
                        )
            legacy_sections = {
                "Evolutionary Optimization Parameters",
                "InputNS",
                "Muscle",
                "Nervous system",
                "OutputNS",
                "Simulation",
                "Stretch receptor",
            }
            for key, child in value.items():
                if not path and key in legacy_sections:
                    continue
                check_mfuncs(child, path + (key,))
        elif isinstance(value, list):
            for index, child in enumerate(value):
                check_mfuncs(child, path + (index,))

    check_finite_numbers(json_data)
    check_mfuncs(json_data)

    nervous_system = json_data.get("nervous_system")
    if not isinstance(nervous_system, dict):
        if isinstance(json_data.get("Nervous system"), dict):
            return True
        errors.append("Missing required 'nervous_system' object")
        nervous_system = {}

    cells = nervous_system.get("cells")
    if not isinstance(cells, dict):
        errors.append("Missing required 'nervous_system.cells' object")
        cells = {}

    cell_names_object = nervous_system.get("cell_names")
    cell_names = None
    if not isinstance(cell_names_object, dict):
        errors.append("Missing required 'nervous_system.cell_names' object")
    else:
        cell_names = cell_names_object.get("value")
        if not isinstance(cell_names, list):
            errors.append("'nervous_system.cell_names.value' must be a list")

    if isinstance(cell_names, list):
        seen_cell_names = set()
        duplicate_cell_names = []
        for cell_name in cell_names:
            if not isinstance(cell_name, str) or not cell_name:
                errors.append(
                    "'nervous_system.cell_names.value' contains a non-string or empty name"
                )
                continue
            if cell_name in seen_cell_names:
                duplicate_cell_names.append(cell_name)
            seen_cell_names.add(cell_name)
        if duplicate_cell_names:
            errors.append(
                "Duplicate cell names in 'nervous_system.cell_names.value': {}".format(
                    ", ".join(sorted(set(duplicate_cell_names)))
                )
            )

        cell_key_names = set(cells)
        cell_list_names = set(
            cell_name for cell_name in cell_names if isinstance(cell_name, str)
        )
        missing_cell_objects = sorted(cell_list_names - cell_key_names)
        extra_cell_objects = sorted(cell_key_names - cell_list_names)
        if missing_cell_objects:
            errors.append(
                "Cells listed in 'cell_names' but missing from 'cells': {}".format(
                    ", ".join(missing_cell_objects)
                )
            )
        if extra_cell_objects:
            errors.append(
                "Cells present in 'cells' but missing from 'cell_names': {}".format(
                    ", ".join(extra_cell_objects)
                )
            )

    required_numeric_cell_parameters = ("bias", "tau", "gain", "state")
    for cell_name, cell_data in cells.items():
        if not isinstance(cell_name, str) or not cell_name:
            errors.append("'nervous_system.cells' contains a non-string or empty key")
            continue
        if not isinstance(cell_data, dict):
            errors.append(
                "'nervous_system.cells.{}' must be an object".format(cell_name)
            )
            continue
        for parameter_name in required_numeric_cell_parameters:
            parameter = cell_data.get(parameter_name)
            parameter_path = "nervous_system.cells.{}.{}".format(
                cell_name, parameter_name
            )
            if not isinstance(parameter, dict):
                errors.append("'{}' must be an object".format(parameter_path))
            elif "value" not in parameter:
                errors.append(
                    "'{}' must contain a 'value' field".format(parameter_path)
                )
            elif not _is_finite_number(parameter["value"]):
                errors.append(
                    "'{}.value' must be a finite number".format(parameter_path)
                )
            elif parameter_name == "tau" and parameter["value"] <= 0:
                errors.append(
                    "'{}.value' must be greater than zero".format(parameter_path)
                )
        cell_class = cell_data.get("cell_class")
        if cell_class is not None:
            if not isinstance(cell_class, dict) or not isinstance(
                cell_class.get("value"), str
            ):
                errors.append(
                    "'nervous_system.cells.{}.cell_class.value' must be a string".format(
                        cell_name
                    )
                )

    valid_cells = set(cells)

    def wrapped_value(container, key, default=None):
        if not isinstance(container, dict):
            return default
        field = container.get(key)
        if key == "value":
            return field if "value" in container else default
        if isinstance(field, dict) and "value" in field:
            return field["value"]
        return default

    def check_weight_object(weight, path):
        if not isinstance(weight, dict):
            errors.append("'{}' must be an object".format(path))
        elif "value" not in weight:
            errors.append("'{}' must contain a 'value' field".format(path))
        elif not _is_finite_number(weight["value"]):
            errors.append("'{}.value' must be a finite number".format(path))

    def check_positive_integer(value, path):
        if isinstance(value, bool) or not isinstance(value, int) or value < 1:
            errors.append("'{}' must be a positive integer".format(path))
            return False
        return True

    def check_connection_list(container, object_path, required_keys):
        if container is None:
            return
        if not isinstance(container, dict):
            errors.append("'{}' must be an object".format(object_path))
            return
        connections = container.get("value")
        if connections is None:
            return
        if not isinstance(connections, list):
            errors.append("'{}.value' must be a list".format(object_path))
            return
        for index, connection in enumerate(connections):
            connection_path = "{}.value[{}]".format(object_path, index)
            if not isinstance(connection, dict):
                errors.append("'{}' must be an object".format(connection_path))
                continue
            for key in required_keys:
                if key not in connection:
                    errors.append(
                        "'{}' is missing key '{}'".format(connection_path, key)
                    )
            weight = connection.get("weight")
            if not isinstance(weight, dict):
                errors.append("'{}.weight' must be an object".format(connection_path))
            elif "value" not in weight:
                errors.append(
                    "'{}.weight' must contain a 'value' field".format(connection_path)
                )
            elif not _is_finite_number(weight["value"]):
                errors.append(
                    "'{}.weight.value' must be a finite number".format(connection_path)
                )

    for connection_key in ("chemical_conns", "electrical_conns"):
        check_connection_list(
            nervous_system.get(connection_key),
            "nervous_system.{}".format(connection_key),
            ("from", "to", "weight"),
        )
        connections_object = nervous_system.get(connection_key)
        connections = (
            connections_object.get("value")
            if isinstance(connections_object, dict)
            else None
        )
        if isinstance(connections, list):
            for index, connection in enumerate(connections):
                if not isinstance(connection, dict):
                    continue
                for key in ("from", "to"):
                    cell_name = connection.get(key)
                    if isinstance(cell_name, str) and cell_name not in valid_cells:
                        errors.append(
                            "'nervous_system.{}.value[{}].{}' refers to unknown cell '{}'".format(
                                connection_key, index, key, cell_name
                            )
                        )

    driving_weights = (
        json_data.get("driving_inputs", {}).get("weights", {}).get("value")
        if isinstance(json_data.get("driving_inputs"), dict)
        and isinstance(json_data.get("driving_inputs", {}).get("weights"), dict)
        else None
    )
    if driving_weights is not None:
        if not isinstance(driving_weights, list):
            errors.append("'driving_inputs.weights.value' must be a list")
        else:
            for index, connection in enumerate(driving_weights):
                if not isinstance(connection, dict):
                    errors.append(
                        "'driving_inputs.weights.value[{}]' must be an object".format(
                            index
                        )
                    )
                    continue
                to_cell = connection.get("to_cell")
                if isinstance(to_cell, str) and to_cell not in valid_cells:
                    errors.append(
                        "'driving_inputs.weights.value[{}].to_cell' refers to unknown cell '{}'".format(
                            index, to_cell
                        )
                    )
                weight = connection.get("weight")
                if not isinstance(weight, dict):
                    errors.append(
                        "'driving_inputs.weights.value[{}].weight' must be an object".format(
                            index
                        )
                    )
                elif "value" not in weight:
                    errors.append(
                        "'driving_inputs.weights.value[{}].weight' must contain a 'value' field".format(
                            index
                        )
                    )
                elif not _is_finite_number(weight["value"]):
                    errors.append(
                        "'driving_inputs.weights.value[{}].weight.value' must be a finite number".format(
                            index
                        )
                    )

    for connection_key in ("chemical_conns", "electrical_conns"):
        connection_object = nervous_system.get(connection_key)
        connections = wrapped_value(connection_object, "value")
        if not isinstance(connections, list):
            continue
        for index, connection in enumerate(connections):
            if not isinstance(connection, dict):
                continue
            for endpoint in ("from", "to"):
                cell_name = connection.get(endpoint)
                path = "nervous_system.{}.value[{}].{}".format(
                    connection_key, index, endpoint
                )
                if not isinstance(cell_name, str) or not cell_name:
                    errors.append("'{}' must be a non-empty string".format(path))

    environments = json_data.get("environments", {})
    environment_names = set()
    if not isinstance(environments, dict):
        errors.append("'environments' must be an object")
        environments = {}
    for environment_key, environment in environments.items():
        path = "environments.{}".format(environment_key)
        if not isinstance(environment_key, str) or not environment_key:
            errors.append("'environments' contains a non-string or empty key")
            continue
        environment_names.add(environment_key)
        if not isinstance(environment, dict):
            errors.append("'{}' must be an object".format(path))
            continue
        stored_name = wrapped_value(environment, "name")
        if not isinstance(stored_name, str) or not stored_name:
            errors.append("'{}.name.value' must be a non-empty string".format(path))
        elif stored_name in environment_names and stored_name != environment_key:
            errors.append(
                "Environment identifier {!r} is duplicated".format(stored_name)
            )
        else:
            environment_names.add(stored_name)
        for parameter_name in ("x_center", "y_center", "grad_steep"):
            parameter = environment.get(parameter_name)
            parameter_path = "{}.{}.value".format(path, parameter_name)
            if not isinstance(parameter, dict) or "value" not in parameter:
                errors.append("'{}' is required".format(parameter_path))
            elif not _is_finite_number(parameter["value"]):
                errors.append("'{}' must be a finite number".format(parameter_path))

    sensors = json_data.get("sensors", {})
    if not isinstance(sensors, dict):
        errors.append("'sensors' must be an object")
        sensors = {}
    for sensor_name, sensor in sensors.items():
        sensor_path = "sensors.{}".format(sensor_name)
        if not isinstance(sensor_name, str) or not sensor_name:
            errors.append("'sensors' contains a non-string or empty key")
            continue
        if not isinstance(sensor, dict):
            errors.append("'{}' must be an object".format(sensor_path))
            continue
        environment_name = wrapped_value(sensor, "environment")
        if not isinstance(environment_name, str) or not environment_name:
            errors.append(
                "'{}.environment.value' must be a non-empty string".format(sensor_path)
            )
        elif environment_name not in environment_names:
            errors.append(
                "'{}.environment.value' refers to unknown environment '{}'".format(
                    sensor_path, environment_name
                )
            )
        for parameter_name in ("sensor_m", "sensor_n"):
            parameter = sensor.get(parameter_name)
            parameter_path = "{}.{}.value".format(sensor_path, parameter_name)
            if not isinstance(parameter, dict) or "value" not in parameter:
                errors.append("'{}' is required".format(parameter_path))
            elif not _is_finite_number(parameter["value"]):
                errors.append("'{}' must be a finite number".format(parameter_path))
            elif parameter["value"] <= 0:
                errors.append("'{}' must be greater than zero".format(parameter_path))

        outputs_object = sensor.get("outputs")
        outputs = wrapped_value(outputs_object, "value")
        output_names = set()
        if outputs_object is None:
            # Older sensor files predate the descriptive outputs list. Those
            # sensors still have the two outputs understood by the C++ model.
            outputs = [{"name": "output_1"}, {"name": "output_2"}]
        elif not isinstance(outputs, list) or not outputs:
            errors.append(
                "'{}.outputs.value' must be a non-empty list".format(sensor_path)
            )
            outputs = []
        for output_index, output in enumerate(outputs):
            output_path = "{}.outputs.value[{}]".format(sensor_path, output_index)
            if not isinstance(output, dict):
                errors.append("'{}' must be an object".format(output_path))
                continue
            output_name = output.get("name")
            if not isinstance(output_name, str) or not output_name:
                errors.append(
                    "'{}.name' must be a non-empty string".format(output_path)
                )
            elif output_name in output_names:
                errors.append(
                    "Duplicate sensor output name {!r} in '{}'".format(
                        output_name, sensor_path
                    )
                )
            else:
                output_names.add(output_name)

        sensor_weights = wrapped_value(sensor.get("weights"), "value", [])
        if not isinstance(sensor_weights, list):
            errors.append("'{}.weights.value' must be a list".format(sensor_path))
            sensor_weights = []
        for weight_index, connection in enumerate(sensor_weights):
            connection_path = "{}.weights.value[{}]".format(sensor_path, weight_index)
            if not isinstance(connection, dict):
                errors.append("'{}' must be an object".format(connection_path))
                continue
            from_output = connection.get("from_output")
            if (
                isinstance(from_output, bool)
                or not isinstance(from_output, int)
                or not 1 <= from_output <= len(outputs)
            ):
                errors.append(
                    "'{}.from_output' must identify an existing sensor output".format(
                        connection_path
                    )
                )
            to_cell = connection.get("to_cell")
            if not isinstance(to_cell, str) or not to_cell:
                errors.append(
                    "'{}.to_cell' must be a non-empty string".format(connection_path)
                )
            elif to_cell not in valid_cells:
                errors.append(
                    "'{}.to_cell' refers to unknown cell '{}'".format(
                        connection_path, to_cell
                    )
                )
            check_weight_object(connection.get("weight"), connection_path + ".weight")

    driving_inputs = json_data.get("driving_inputs")
    driving_input_ids = set()
    if driving_inputs is not None and not isinstance(driving_inputs, dict):
        errors.append("'driving_inputs' must be an object")
        driving_inputs = None
    if isinstance(driving_inputs, dict):
        input_entries = wrapped_value(driving_inputs.get("inputs"), "value")
        if input_entries is None:
            input_entries = []
        elif not isinstance(input_entries, list):
            errors.append("'driving_inputs.inputs.value' must be a list")
            input_entries = []
        for input_index, input_entry in enumerate(input_entries):
            input_path = "driving_inputs.inputs.value[{}]".format(input_index)
            if not isinstance(input_entry, dict):
                errors.append("'{}' must be an object".format(input_path))
                continue
            input_id = input_entry.get("input_num")
            if not isinstance(input_id, str) or not input_id:
                errors.append(
                    "'{}.input_num' must be a non-empty string".format(input_path)
                )
            elif input_id in driving_input_ids:
                errors.append(
                    "Duplicate driving input identifier {!r}".format(input_id)
                )
            else:
                driving_input_ids.add(input_id)
            strength = input_entry.get("strength")
            if not isinstance(strength, dict) or "value" not in strength:
                errors.append("'{}.strength.value' is required".format(input_path))
            elif not _is_finite_number(strength["value"]):
                errors.append(
                    "'{}.strength.value' must be a finite number".format(input_path)
                )

        if isinstance(driving_weights, list):
            for weight_index, connection in enumerate(driving_weights):
                if not isinstance(connection, dict):
                    continue
                connection_path = "driving_inputs.weights.value[{}]".format(
                    weight_index
                )
                from_input = connection.get("from_input")
                if not isinstance(from_input, str) or not from_input:
                    errors.append(
                        "'{}.from_input' must be a non-empty string".format(
                            connection_path
                        )
                    )
                elif from_input not in driving_input_ids:
                    errors.append(
                        "'{}.from_input' refers to unknown driving input '{}'".format(
                            connection_path, from_input
                        )
                    )

    input_switcher = json_data.get("input_switcher")
    if input_switcher is not None and not isinstance(input_switcher, dict):
        errors.append("'input_switcher' must be an object")
        input_switcher = None
    if isinstance(input_switcher, dict) and "inputs" in input_switcher:
        patterns = wrapped_value(input_switcher.get("inputs"), "value")
        if not isinstance(patterns, list):
            errors.append("'input_switcher.inputs.value' must be a list")
            patterns = []
        pattern_ids = set()
        for pattern_index, pattern in enumerate(patterns):
            pattern_path = "input_switcher.inputs.value[{}]".format(pattern_index)
            if not isinstance(pattern, dict):
                errors.append("'{}' must be an object".format(pattern_path))
                continue
            pattern_id = pattern.get("input_index")
            if not isinstance(pattern_id, str) or not pattern_id:
                errors.append(
                    "'{}.input_index' must be a non-empty string".format(pattern_path)
                )
            elif pattern_id in pattern_ids:
                errors.append(
                    "Duplicate input pattern identifier {!r}".format(pattern_id)
                )
            else:
                pattern_ids.add(pattern_id)
            values = pattern.get("value")
            if not isinstance(values, list):
                errors.append("'{}.value' must be a list".format(pattern_path))
                continue
            pattern_inputs = set()
            for value_index, input_value in enumerate(values):
                value_path = "{}.value[{}]".format(pattern_path, value_index)
                if not isinstance(input_value, dict):
                    errors.append("'{}' must be an object".format(value_path))
                    continue
                input_id = input_value.get("input_num")
                if not isinstance(input_id, str) or not input_id:
                    errors.append(
                        "'{}.input_num' must be a non-empty string".format(value_path)
                    )
                elif input_id not in driving_input_ids:
                    errors.append(
                        "'{}.input_num' refers to unknown driving input '{}'".format(
                            value_path, input_id
                        )
                    )
                elif input_id in pattern_inputs:
                    errors.append(
                        "Input pattern {!r} contains driving input {!r} more than once".format(
                            pattern_id, input_id
                        )
                    )
                else:
                    pattern_inputs.add(input_id)
                if not _is_finite_number(input_value.get("value")):
                    errors.append(
                        "'{}.value' must be a finite number".format(value_path)
                    )

        configured_size = wrapped_value(input_switcher, "size")
        if (
            isinstance(configured_size, bool)
            or not isinstance(configured_size, int)
            or configured_size != len(patterns)
        ):
            errors.append(
                "'input_switcher.size.value' must equal the number of input patterns"
            )

        periods_present = "time_periods" in input_switcher
        indices_present = "input_indices" in input_switcher
        if periods_present != indices_present:
            errors.append(
                "'input_switcher.time_periods' and 'input_switcher.input_indices' "
                "must either both be present or both be absent"
            )
        if periods_present and indices_present:
            periods = wrapped_value(input_switcher, "time_periods")
            indices = wrapped_value(input_switcher, "input_indices")
            if not isinstance(periods, list):
                errors.append("'input_switcher.time_periods.value' must be a list")
                periods = []
            if not isinstance(indices, list):
                errors.append("'input_switcher.input_indices.value' must be a list")
                indices = []
            if len(periods) != len(indices):
                errors.append(
                    "Input-switcher time periods and input indices must have equal length"
                )
            for period_index, period in enumerate(periods):
                if not _is_finite_number(period) or period <= 0:
                    errors.append(
                        "'input_switcher.time_periods.value[{}]' must be finite and greater than zero".format(
                            period_index
                        )
                    )
            for index, pattern_id in enumerate(indices):
                if not isinstance(pattern_id, str) or pattern_id not in pattern_ids:
                    errors.append(
                        "'input_switcher.input_indices.value[{}]' refers to unknown input pattern {!r}".format(
                            index, pattern_id
                        )
                    )

        time_offset = wrapped_value(input_switcher, "time_offset", 0.0)
        if not _is_finite_number(time_offset) or time_offset < 0:
            errors.append(
                "'input_switcher.time_offset.value' must be finite and non-negative"
            )
        do_evolution = wrapped_value(input_switcher, "doEvolution", False)
        if not isinstance(do_evolution, bool):
            errors.append("'input_switcher.doEvolution.value' must be boolean")

    funcable = json_data.get("funcable", json_data.get("Funcable"))
    if funcable is not None and not isinstance(funcable, dict):
        errors.append("'funcable' must be an object")
        funcable = None
    if isinstance(funcable, dict):
        for function_name, settings in funcable.items():
            if function_name == "schedules":
                continue
            try:
                _mfunc_name(function_name)
            except (TypeError, ValueError):
                errors.append("Unknown funcable function {!r}".format(function_name))
                continue
            if not isinstance(settings, dict):
                errors.append("'funcable.{}' must be an object".format(function_name))
                continue
            if "doInverse" in settings and not isinstance(settings["doInverse"], bool):
                errors.append(
                    "'funcable.{}.doInverse' must be boolean".format(function_name)
                )
            if "condval" in settings and (
                isinstance(settings["condval"], bool)
                or not isinstance(settings["condval"], int)
            ):
                errors.append(
                    "'funcable.{}.condval' must be an integer".format(function_name)
                )
        schedules = funcable.get("schedules", {})
        if not isinstance(schedules, dict):
            errors.append("'funcable.schedules' must be an object")
            schedules = {}
        for schedule_name, schedule in schedules.items():
            schedule_path = "funcable.schedules.{}".format(schedule_name)
            if not isinstance(schedule, dict):
                errors.append("'{}' must be an object".format(schedule_path))
                continue
            function_name = wrapped_value(schedule, "function_index")
            try:
                _mfunc_name(function_name)
            except (TypeError, ValueError) as exc:
                errors.append(
                    "'{}.function_index.value' is invalid: {}".format(
                        schedule_path, exc
                    )
                )
            intervals = wrapped_value(schedule, "time_intervals")
            if not isinstance(intervals, list) or not intervals:
                errors.append(
                    "'{}.time_intervals.value' must be a non-empty list".format(
                        schedule_path
                    )
                )
                intervals = []
            for interval_index, interval in enumerate(intervals):
                if not _is_finite_number(interval) or interval <= 0:
                    errors.append(
                        "'{}.time_intervals.value[{}]' must be finite and greater than zero".format(
                            schedule_path, interval_index
                        )
                    )
            condvals = wrapped_value(schedule, "condvals")
            if condvals is not None:
                if not isinstance(condvals, list) or len(condvals) != len(intervals):
                    errors.append(
                        "'{}.condvals.value' must be a list matching time_intervals".format(
                            schedule_path
                        )
                    )
                elif any(
                    isinstance(value, bool) or not isinstance(value, int)
                    for value in condvals
                ):
                    errors.append(
                        "'{}.condvals.value' must contain only integers".format(
                            schedule_path
                        )
                    )
            offset = wrapped_value(schedule, "time_offset", 0.0)
            if not _is_finite_number(offset) or offset < 0:
                errors.append(
                    "'{}.time_offset.value' must be finite and non-negative".format(
                        schedule_path
                    )
                )
            schedule_evolution = wrapped_value(schedule, "doEvolution", False)
            if not isinstance(schedule_evolution, bool):
                errors.append(
                    "'{}.doEvolution.value' must be boolean".format(schedule_path)
                )

    worm = json_data.get("worm", {})
    muscle_count = wrapped_value(worm, "N_muscles")
    if muscle_count is not None:
        check_positive_integer(muscle_count, "worm.N_muscles.value")
    body = json_data.get("body", {})
    segment_count = wrapped_value(body, "n_segments")
    if segment_count is not None:
        check_positive_integer(segment_count, "body.n_segments.value")

    for side in ("dorsal", "ventral"):
        nmj_name = side + "_nmj"
        nmj = json_data.get(nmj_name)
        if nmj is not None:
            if not isinstance(nmj, dict):
                errors.append("'{}' must be an object".format(nmj_name))
            else:
                weights = wrapped_value(nmj.get("weights"), "value")
                if not isinstance(weights, list):
                    errors.append("'{}.weights.value' must be a list".format(nmj_name))
                    weights = []
                for index, connection in enumerate(weights):
                    path = "{}.weights.value[{}]".format(nmj_name, index)
                    if not isinstance(connection, dict):
                        errors.append("'{}' must be an object".format(path))
                        continue
                    from_cell = connection.get("from_cell")
                    if not isinstance(from_cell, str) or from_cell not in valid_cells:
                        errors.append(
                            "'{}.from_cell' refers to unknown cell {!r}".format(
                                path, from_cell
                            )
                        )
                    to_muscle = connection.get("to_musc")
                    if check_positive_integer(to_muscle, path + ".to_musc"):
                        if muscle_count is not None and to_muscle > muscle_count:
                            errors.append(
                                "'{}.to_musc' exceeds the configured muscle count".format(
                                    path
                                )
                            )
                    check_weight_object(connection.get("weight"), path + ".weight")

        body_name = side + "_body"
        body_connections = json_data.get(body_name)
        if body_connections is not None:
            if not isinstance(body_connections, dict):
                errors.append("'{}' must be an object".format(body_name))
            else:
                weights = wrapped_value(body_connections.get("weights"), "value")
                if not isinstance(weights, list):
                    errors.append("'{}.weights.value' must be a list".format(body_name))
                    weights = []
                for index, connection in enumerate(weights):
                    path = "{}.weights.value[{}]".format(body_name, index)
                    if not isinstance(connection, dict):
                        errors.append("'{}' must be an object".format(path))
                        continue
                    from_muscle = connection.get("from_musc")
                    if check_positive_integer(from_muscle, path + ".from_musc"):
                        if muscle_count is not None and from_muscle > muscle_count:
                            errors.append(
                                "'{}.from_musc' exceeds the configured muscle count".format(
                                    path
                                )
                            )
                    to_segment = connection.get("to_seg")
                    if check_positive_integer(to_segment, path + ".to_seg"):
                        if segment_count is not None and to_segment > segment_count:
                            errors.append(
                                "'{}.to_seg' exceeds the configured segment count".format(
                                    path
                                )
                            )
                    check_weight_object(connection.get("weight"), path + ".weight")

    stretch_receptor = json_data.get("stretch_receptor")
    if stretch_receptor is not None and not isinstance(stretch_receptor, dict):
        errors.append("'stretch_receptor' must be an object")
        stretch_receptor = None
    if isinstance(stretch_receptor, dict):
        receptor_type = wrapped_value(stretch_receptor, "type")
        if receptor_type not in ("SR18", "SRCE"):
            errors.append("'stretch_receptor.type.value' must be 'SR18' or 'SRCE'")
        receptor_segments = wrapped_value(stretch_receptor, "n_segs")
        if receptor_segments is not None:
            check_positive_integer(receptor_segments, "stretch_receptor.n_segs.value")
            if segment_count is not None and receptor_segments != segment_count:
                errors.append(
                    "'stretch_receptor.n_segs.value' must match 'body.n_segments.value'"
                )
        receptor_count = wrapped_value(stretch_receptor, "n_stretch")
        if receptor_count is not None:
            check_positive_integer(receptor_count, "stretch_receptor.n_stretch.value")
        set_direct = wrapped_value(stretch_receptor, "set_direct", False)
        if not isinstance(set_direct, bool):
            errors.append("'stretch_receptor.set_direct.value' must be boolean")

        if receptor_type == "SR18":
            body_weight_names = ("d_weights", "v_weights")
            neural_weight_names = ("ns_d_weights", "ns_v_weights")
        elif receptor_type == "SRCE":
            body_weight_names = (
                "a_d_weights",
                "a_v_weights",
                "b_d_weights",
                "b_v_weights",
            )
            neural_weight_names = (
                "ns_a_d_weights",
                "ns_a_v_weights",
                "ns_b_d_weights",
                "ns_b_v_weights",
            )
        else:
            body_weight_names = ()
            neural_weight_names = ()
        if set_direct:
            for weight_name in body_weight_names + neural_weight_names:
                if weight_name not in stretch_receptor:
                    errors.append(
                        "'stretch_receptor.{}' is required when set_direct is true".format(
                            weight_name
                        )
                    )
        for weight_name in body_weight_names:
            if weight_name not in stretch_receptor:
                continue
            weights = wrapped_value(stretch_receptor.get(weight_name), "value")
            path_base = "stretch_receptor.{}.value".format(weight_name)
            if not isinstance(weights, list):
                errors.append("'{}' must be a list".format(path_base))
                continue
            for index, connection in enumerate(weights):
                path = "{}[{}]".format(path_base, index)
                if not isinstance(connection, dict):
                    errors.append("'{}' must be an object".format(path))
                    continue
                from_segment = connection.get("from_seg")
                if check_positive_integer(from_segment, path + ".from_seg"):
                    if (
                        receptor_segments is not None
                        and from_segment > receptor_segments
                    ):
                        errors.append(
                            "'{}.from_seg' exceeds the stretch-receptor segment count".format(
                                path
                            )
                        )
                check_positive_integer(connection.get("to_sr"), path + ".to_sr")
                check_weight_object(connection.get("weight"), path + ".weight")
        for weight_name in neural_weight_names:
            if weight_name not in stretch_receptor:
                continue
            weights = wrapped_value(stretch_receptor.get(weight_name), "value")
            path_base = "stretch_receptor.{}.value".format(weight_name)
            if not isinstance(weights, list):
                errors.append("'{}' must be a list".format(path_base))
                continue
            for index, connection in enumerate(weights):
                path = "{}[{}]".format(path_base, index)
                if not isinstance(connection, dict):
                    errors.append("'{}' must be an object".format(path))
                    continue
                check_positive_integer(connection.get("from_sr"), path + ".from_sr")
                to_cell = connection.get("to_ns")
                if not isinstance(to_cell, str) or to_cell not in valid_cells:
                    errors.append(
                        "'{}.to_ns' refers to unknown cell {!r}".format(path, to_cell)
                    )
                check_weight_object(connection.get("weight"), path + ".weight")

    for connectivity_name in ("vnc_18", "vnc_nmj"):
        connectivity = json_data.get(connectivity_name)
        if connectivity is None:
            continue
        if not isinstance(connectivity, dict):
            errors.append("'{}' must be an object".format(connectivity_name))
            continue
        set_from_this = wrapped_value(connectivity, "set_from_this", False)
        if not isinstance(set_from_this, bool):
            errors.append(
                "'{}.set_from_this.value' must be boolean".format(connectivity_name)
            )
        for connection_group in ("dorsal_conns", "ventral_conns"):
            connections = connectivity.get(connection_group)
            path_base = "{}.{}".format(connectivity_name, connection_group)
            if connections is None:
                if set_from_this:
                    errors.append(
                        "'{}' is required when set_from_this is true".format(path_base)
                    )
                continue
            if not isinstance(connections, dict):
                errors.append("'{}' must be an object".format(path_base))
                continue
            if set_from_this and not connections:
                errors.append(
                    "'{}' must not be empty when set_from_this is true".format(
                        path_base
                    )
                )
            for cell_stem, connection in connections.items():
                path = "{}.{}".format(path_base, cell_stem)
                if not isinstance(cell_stem, str) or not cell_stem:
                    errors.append(
                        "'{}' contains an invalid cell stem".format(path_base)
                    )
                elif not any(
                    cell_name == cell_stem or cell_name.startswith(cell_stem + "_")
                    for cell_name in valid_cells
                ):
                    errors.append(
                        "'{}' refers to unknown cell stem '{}'".format(path, cell_stem)
                    )
                if not isinstance(connection, dict):
                    errors.append("'{}' must be an object".format(path))
                    continue
                check_positive_integer(connection.get("cell_ind"), path + ".cell_ind")
                check_weight_object(connection.get("weight"), path + ".weight")

    active_connectivity_objects = [
        name
        for name in ("vnc_18", "vnc_nmj")
        if wrapped_value(json_data.get(name), "set_from_this", False) is True
    ]
    if len(active_connectivity_objects) > 1:
        errors.append(
            "Only one prescribed VNC-to-muscle connectivity object may have "
            "set_from_this enabled: {}".format(", ".join(active_connectivity_objects))
        )

    def check_evotag_fields(value, path=()):
        if isinstance(value, dict):
            if "evotag" in value and (
                not isinstance(value["evotag"], str) or not value["evotag"]
            ):
                errors.append(
                    "'{}.evotag' must be a non-empty string".format(
                        _json_path_string(path)
                    )
                )
            for key, child in value.items():
                if key not in {"evolvable_ranges", "evolved_used"}:
                    check_evotag_fields(child, path + (key,))
        elif isinstance(value, list):
            for index, child in enumerate(value):
                check_evotag_fields(child, path + (index,))

    check_evotag_fields(json_data)

    evolvable_ranges = json_data.get("evolvable_ranges")
    if evolvable_ranges is None:
        evolvable_ranges = {}
    if not isinstance(evolvable_ranges, dict):
        errors.append("'evolvable_ranges' must be an object")
        evolvable_ranges = {}

    range_evotags = set()
    range_entries = {}
    legacy_ranges = evolvable_ranges.get("value")
    if isinstance(legacy_ranges, list):
        for index, range_entry in enumerate(legacy_ranges):
            if not isinstance(range_entry, dict) or len(range_entry) != 1:
                errors.append(
                    "'evolvable_ranges.value[{}]' must contain one evotag object".format(
                        index
                    )
                )
                continue
            evotag_name, evotag_range = next(iter(range_entry.items()))
            range_evotags.add(evotag_name)
            range_entries[evotag_name] = evotag_range
    else:
        range_evotags = set(evolvable_ranges)
        range_entries = dict(evolvable_ranges)

    if "value" in range_evotags:
        range_evotags.remove("value")
    invalid_range_evotags = [
        evotag for evotag in range_evotags if not isinstance(evotag, str) or not evotag
    ]
    for evotag in invalid_range_evotags:
        errors.append(
            "Evolvable-range key {!r} must be a non-empty string".format(evotag)
        )
        range_evotags.discard(evotag)
        range_entries.pop(evotag, None)

    evotag_uses = _collect_evotag_uses(json_data)
    used_evotags = set(evotag for evotag, _ in evotag_uses)
    missing_from_ranges = sorted(used_evotags - range_evotags)
    unused_ranges = sorted(range_evotags - used_evotags)
    if missing_from_ranges:
        errors.append(
            "Evotags used in the JSON but missing from 'evolvable_ranges': {}".format(
                ", ".join(missing_from_ranges)
            )
        )
    if unused_ranges:
        errors.append(
            "Evotags in 'evolvable_ranges' but not used elsewhere in the JSON: {}".format(
                ", ".join(unused_ranges)
            )
        )

    for evotag_name, range_entry in range_entries.items():
        if evotag_name == "value":
            continue
        range_path = "evolvable_ranges.{}".format(evotag_name)
        if not isinstance(range_entry, dict):
            errors.append("'{}' must be an object".format(range_path))
            continue
        for key in ("active", "lower_limit", "upper_limit", "name"):
            if key not in range_entry:
                errors.append("'{}' is missing key '{}'".format(range_path, key))
        if "active" in range_entry and not isinstance(range_entry["active"], bool):
            errors.append("'{}.active' must be boolean".format(range_path))
        for key in ("lower_limit", "upper_limit"):
            if key in range_entry and not _is_finite_number(range_entry[key]):
                errors.append("'{}.{}' must be a finite number".format(range_path, key))
        if "name" in range_entry and (
            not isinstance(range_entry["name"], str) or not range_entry["name"]
        ):
            errors.append("'{}.name' must be a non-empty string".format(range_path))
        if (
            _is_finite_number(range_entry.get("lower_limit"))
            and _is_finite_number(range_entry.get("upper_limit"))
            and range_entry["lower_limit"] > range_entry["upper_limit"]
        ):
            errors.append(
                "'{}.lower_limit' must not be greater than 'upper_limit'".format(
                    range_path
                )
            )

    if errors:
        message_lines = [
            "W2DSR JSON validation failed{}:".format(location),
        ]
        message_lines.extend("  - {}".format(error) for error in errors)
        raise ValueError("\n".join(message_lines))

    return True


def validate_w2dsr_json_file(filename):
    """Validate a W2DSR JSON file before it is sent to the C++ model."""
    with open(filename, "r") as f:
        json_data = json.load(f)
    return validate_w2dsr_json(json_data, source_name=filename)


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

    muscle_count = _get_muscle_count(result)
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


def add_cell(json_data, cell_name=None):
    """Return a copy with one default cell added.

    If cell_name is supplied, the new cell is renamed to that value and only
    the updated JSON is returned. Without cell_name, return the updated JSON
    plus the generated default cell name for backward compatibility.
    """
    if cell_name is not None:
        if not isinstance(cell_name, str) or not cell_name.strip():
            raise ValueError("cell_name must be a non-empty string")
        cell_name = cell_name.strip()

    result, cell_names = add_random_cell_network(json_data, 1, 0.0)
    generated_name = cell_names[0]
    if cell_name is None:
        return result, generated_name
    return rename_cell(result, generated_name, cell_name)


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
    input_index_id = _input_switcher_id(input_index)
    try:
        input_index_number = _input_switcher_index(input_index_id)
    except ValueError:
        input_index_number = None
    if not isinstance(input_nums, (list, tuple)) or not input_nums:
        raise ValueError("input_nums must be a non-empty list or tuple")
    if not isinstance(values, (list, tuple)) or not values:
        raise ValueError("values must be a non-empty list or tuple")
    if len(input_nums) != len(values):
        raise ValueError("input_nums and values must have the same length")

    pattern_values = []
    seen_input_nums = set()
    for input_num, value in zip(input_nums, values):
        input_id = _driving_input_id(input_num)
        if input_id in seen_input_nums:
            raise ValueError(
                "Input identifier {} occurs more than once".format(input_id)
            )
        seen_input_nums.add(input_id)

        if isinstance(value, bool) or not isinstance(value, (int, float)):
            raise TypeError("Each input value must be a number")
        if not math.isfinite(value):
            raise ValueError("Each input value must be finite")
        pattern_values.append({"input_num": input_id, "value": float(value)})

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
            if isinstance(existing_input_num, bool):
                raise TypeError("Each existing input identifier must be valid")
            if isinstance(existing_input_num, int) and existing_input_num < 0:
                raise TypeError(
                    "Each existing input number must be a non-negative integer"
                )
            if not isinstance(existing_input_num, (int, str)):
                raise TypeError(
                    "Each existing input identifier must be a string or integer"
                )
            if existing_input_num == 0:
                legacy_zero_based = True

    if legacy_zero_based:
        for pattern in inputs:
            for entry in pattern["value"]:
                entry["input_num"] = _driving_input_id(entry["input_num"] + 1)
    else:
        for pattern in inputs:
            for entry in pattern["value"]:
                entry["input_num"] = _driving_input_id(entry["input_num"])

    new_pattern = {
        "input_index": input_index_id,
        "value": pattern_values,
    }
    matching_positions = []
    for position, pattern in enumerate(inputs):
        pattern["input_index"] = _input_switcher_id(pattern.get("input_index"))
        if pattern["input_index"] == input_index_id:
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
    if input_index_number is None:
        size_object["value"] = max(current_size, len(inputs))
    else:
        size_object["value"] = max(current_size, input_index_number + 1, len(inputs))
    return result


def _get_driving_input_ids(json_data):
    driving_inputs = json_data.get("driving_inputs")
    if not isinstance(driving_inputs, dict):
        raise KeyError("JSON does not contain a 'driving_inputs' object")
    inputs = driving_inputs.get("inputs", {}).get("value")
    if not isinstance(inputs, list):
        raise TypeError("'driving_inputs.inputs.value' must be a list")

    input_ids = set()
    for entry in inputs:
        if not isinstance(entry, dict):
            raise TypeError("Each driving input must be a dictionary")
        input_ids.add(_driving_input_id(entry.get("input_num")))
    return input_ids


def _next_input_switcher_pattern_name(inputs):
    used_names = set()
    for pattern in inputs:
        if not isinstance(pattern, dict):
            raise TypeError("Each input-switcher pattern must be a dictionary")
        if "input_index" in pattern:
            used_names.add(_input_switcher_id(pattern["input_index"]))

    pattern_number = 0
    while True:
        pattern_name = "input_pattern_{}".format(pattern_number)
        if pattern_name not in used_names:
            return pattern_name
        pattern_number += 1


def add_input_switcher_pattern(
    json_data,
    pattern_name=None,
    input_nums=None,
    values=None,
):
    """Return a copy with an input-switcher pattern added or replaced."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if input_nums is None:
        raise ValueError("input_nums must be supplied")
    if values is None:
        raise ValueError("values must be supplied")
    if not isinstance(input_nums, (list, tuple)) or not input_nums:
        raise ValueError("input_nums must be a non-empty list or tuple")
    if not isinstance(values, (list, tuple)) or not values:
        raise ValueError("values must be a non-empty list or tuple")
    if len(input_nums) != len(values):
        raise ValueError("input_nums and values must have the same length")

    allowed_input_ids = _get_driving_input_ids(json_data)
    pattern_values = []
    seen_input_nums = set()
    for input_num, value in zip(input_nums, values):
        input_id = _driving_input_id(input_num)
        if input_id not in allowed_input_ids:
            raise KeyError(
                "Driving input {!r} does not exist in driving_inputs".format(input_id)
            )
        if input_id in seen_input_nums:
            raise ValueError(
                "Input identifier {} occurs more than once".format(input_id)
            )
        seen_input_nums.add(input_id)

        if isinstance(value, bool) or not isinstance(value, (int, float)):
            raise TypeError("Each input value must be a number")
        if not math.isfinite(value):
            raise ValueError("Each input value must be finite")
        pattern_values.append({"input_num": input_id, "value": float(value)})

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

    if pattern_name is None:
        pattern_id = _next_input_switcher_pattern_name(inputs)
    else:
        pattern_id = _input_switcher_id(pattern_name)

    new_pattern = {"input_index": pattern_id, "value": pattern_values}
    replaced = False
    kept_inputs = []
    for pattern in inputs:
        if not isinstance(pattern, dict):
            raise TypeError("Each input-switcher pattern must be a dictionary")
        existing_pattern_id = _input_switcher_id(pattern.get("input_index"))
        pattern["input_index"] = existing_pattern_id
        if existing_pattern_id == pattern_id:
            if not replaced:
                kept_inputs.append(new_pattern)
                replaced = True
            continue
        kept_inputs.append(pattern)
    if not replaced:
        kept_inputs.append(new_pattern)

    inputs_object["value"] = kept_inputs
    input_switcher["size"] = {"value": len(kept_inputs)}
    return result


def delete_input_switcher_pattern(json_data, pattern_name):
    """Return a copy with one input-switcher pattern and schedule uses removed."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    pattern_id = _input_switcher_id(pattern_name)

    result = copy.deepcopy(json_data)
    input_switcher = result.get("input_switcher")
    if not isinstance(input_switcher, dict):
        raise KeyError("JSON does not contain an 'input_switcher' object")
    inputs_object = input_switcher.get("inputs")
    if not isinstance(inputs_object, dict):
        raise KeyError("JSON does not contain 'input_switcher.inputs'")
    inputs = inputs_object.get("value")
    if not isinstance(inputs, list):
        raise TypeError("'input_switcher.inputs.value' must be a list")

    kept_inputs = []
    found_pattern = False
    for pattern in inputs:
        if not isinstance(pattern, dict):
            raise TypeError("Each input-switcher pattern must be a dictionary")
        existing_pattern_id = _input_switcher_id(pattern.get("input_index"))
        pattern["input_index"] = existing_pattern_id
        if existing_pattern_id == pattern_id:
            found_pattern = True
            continue
        kept_inputs.append(pattern)
    if not found_pattern:
        raise KeyError("Input-switcher pattern {!r} does not exist".format(pattern_id))

    inputs_object["value"] = kept_inputs
    input_switcher["size"] = {"value": len(kept_inputs)}

    scheduled = input_switcher.get("input_indices", {}).get("value")
    periods = input_switcher.get("time_periods", {}).get("value")
    if scheduled is not None or periods is not None:
        if not isinstance(scheduled, list):
            raise TypeError("'input_switcher.input_indices.value' must be a list")
        if not isinstance(periods, list):
            raise TypeError("'input_switcher.time_periods.value' must be a list")
        if len(scheduled) != len(periods):
            raise ValueError(
                "input_switcher.input_indices and time_periods must have "
                "the same length"
            )
        kept_schedule = []
        kept_periods = []
        for scheduled_id, period in zip(scheduled, periods):
            scheduled_pattern_id = _input_switcher_id(scheduled_id)
            if scheduled_pattern_id == pattern_id:
                continue
            kept_schedule.append(scheduled_pattern_id)
            kept_periods.append(period)
        input_switcher["input_indices"]["value"] = kept_schedule
        input_switcher["time_periods"]["value"] = kept_periods

    if not kept_inputs:
        result.pop("input_switcher", None)
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
    inputs = input_switcher.get("inputs", {}).get("value")
    if not isinstance(inputs, list):
        raise TypeError("'input_switcher.inputs.value' must be a list")
    allowed_indices = set()
    for pattern in inputs:
        if not isinstance(pattern, dict) or "input_index" not in pattern:
            raise TypeError("Each input-switcher pattern must contain input_index")
        pattern_id = _input_switcher_id(pattern["input_index"])
        pattern["input_index"] = pattern_id
        allowed_indices.add(pattern_id)

    periods = []
    for period in time_periods:
        if isinstance(period, bool) or not isinstance(period, (int, float)):
            raise TypeError("Each time period must be a number")
        if not math.isfinite(period) or period <= 0:
            raise ValueError("Each time period must be finite and greater than zero")
        periods.append(float(period))

    indices = []
    for input_index in input_indices:
        input_index_id = _input_switcher_id(input_index)
        if input_index_id not in allowed_indices:
            raise ValueError(
                "Input index {} is not one of the configured input_index values".format(
                    input_index_id
                )
            )
        indices.append(input_index_id)

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
    """Return a copy with a repeating schedule for one funcable function."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    function_name = _mfunc_name(function_index)
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
    if "funcable" not in result and "Funcable" in result:
        result["funcable"] = result.pop("Funcable")
    funcable = result.setdefault("funcable", {})
    if not isinstance(funcable, dict):
        raise TypeError("'funcable' must be a dictionary")
    schedules = funcable.setdefault("schedules", {})
    if not isinstance(schedules, dict):
        raise TypeError("'funcable.schedules' must be a dictionary")

    if schedule_name is None:
        schedule_name = "function_{}".format(function_name)
    schedule = {
        "function_index": {"value": function_name},
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


def delete_evotag(json_data, evotag_name):
    """Return a copy with one evotag removed from all model values."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(evotag_name, str) or not evotag_name:
        raise ValueError("evotag_name must be a non-empty string")

    result = copy.deepcopy(json_data)

    def normalize_evotag(value):
        if isinstance(value, dict) and "value" in value:
            value = value["value"]
        if isinstance(value, bool):
            return None
        if isinstance(value, int):
            return "evotag_{}".format(value)
        if isinstance(value, str):
            return value
        return None

    def remove_from_values(value):
        if isinstance(value, dict):
            if normalize_evotag(value.get("evotag")) == evotag_name:
                value.pop("evotag", None)
            for child in value.values():
                remove_from_values(child)
        elif isinstance(value, list):
            for child in value:
                remove_from_values(child)

    remove_from_values(result)

    evolvable_ranges = result.get("evolvable_ranges")
    if isinstance(evolvable_ranges, dict):
        evolvable_ranges.pop(evotag_name, None)
        legacy_entries = evolvable_ranges.get("value")
        if isinstance(legacy_entries, list):
            evolvable_ranges["value"] = [
                entry
                for entry in legacy_entries
                if not (
                    isinstance(entry, dict)
                    and (
                        entry.get("evotag") == evotag_name
                        or (len(entry) == 1 and evotag_name in entry)
                    )
                )
            ]

    evolved_used = result.get("evolved_used")
    if isinstance(evolved_used, dict) and isinstance(evolved_used.get("value"), list):
        evolved_used["value"] = [
            evotag for evotag in evolved_used["value"] if evotag != evotag_name
        ]

    return result


def _collect_evotags(value, output=None):
    if output is None:
        output = set()
    if isinstance(value, dict):
        evotag = value.get("evotag")
        if isinstance(evotag, dict) and "value" in evotag:
            evotag = evotag["value"]
        if isinstance(evotag, (str, int)) and not isinstance(evotag, bool):
            output.add(evotag)
        for child in value.values():
            _collect_evotags(child, output)
    elif isinstance(value, list):
        for child in value:
            _collect_evotags(child, output)
    return output


def collect_evotags(value, output=None):
    """Return the set of evotags contained recursively within value."""
    return _collect_evotags(value, output)


def strip_unretained_evotags(json_data, retained_evotags):
    """Return a copy with evotag fields removed unless they are retained."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if retained_evotags is None:
        raise TypeError("retained_evotags must be an iterable of evotag names")

    retained = {str(evotag) for evotag in retained_evotags}
    result = copy.deepcopy(json_data)

    def strip(value):
        if isinstance(value, dict):
            evotag = value.get("evotag")
            if isinstance(evotag, dict) and "value" in evotag:
                evotag = evotag["value"]
            if (
                isinstance(evotag, (str, int))
                and not isinstance(evotag, bool)
                and str(evotag) not in retained
            ):
                value.pop("evotag", None)
            for child in value.values():
                strip(child)
        elif isinstance(value, list):
            for child in value:
                strip(child)

    strip(result)
    return result


def retain_cell_related_evotags(json_data, cell_names):
    """Keep only evotags associated with the supplied nervous-system cells.

    Related tags include cell parameters, chemical and electrical connections
    touching a selected cell, neuromuscular connections originating at one,
    stretch-receptor connections targeting one, and prescribed VNC-to-muscle
    entries for the corresponding cell stems.  Head stretch-receptor gain is
    retained when one of the standard SMD/RMD head cells is selected.
    """
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(cell_names, (list, tuple, set)) or not cell_names:
        raise ValueError("cell_names must be a non-empty list, tuple, or set")
    if any(not isinstance(name, str) or not name for name in cell_names):
        raise ValueError("Every cell name must be a non-empty string")

    selected = set(cell_names)
    nervous_system = json_data.get("nervous_system", {})
    cells = nervous_system.get("cells", {})
    if not isinstance(cells, dict):
        raise KeyError("JSON does not contain nervous_system.cells")
    missing = sorted(selected.difference(cells))
    if missing:
        raise KeyError("Unknown nervous-system cells: {}".format(", ".join(missing)))

    retained = set()
    for cell_name in selected:
        _collect_evotags(cells[cell_name], retained)

    for connection_type in ("chemical_conns", "electrical_conns"):
        connections = nervous_system.get(connection_type, {}).get("value", [])
        if isinstance(connections, list):
            for connection in connections:
                if not isinstance(connection, dict):
                    continue
                if (
                    connection.get("from") in selected
                    or connection.get("to") in selected
                ):
                    _collect_evotags(connection, retained)

    for section_name in ("dorsal_nmj", "ventral_nmj"):
        connections = (
            json_data.get(section_name, {}).get("weights", {}).get("value", [])
        )
        if isinstance(connections, list):
            for connection in connections:
                if (
                    isinstance(connection, dict)
                    and connection.get("from_cell") in selected
                ):
                    _collect_evotags(connection, retained)

    stretch_receptor = json_data.get("stretch_receptor", {})
    if isinstance(stretch_receptor, dict):
        for section_name in ("ns_d_weights", "ns_v_weights"):
            connections = stretch_receptor.get(section_name, {}).get("value", [])
            if isinstance(connections, list):
                for connection in connections:
                    if (
                        isinstance(connection, dict)
                        and connection.get("to_ns") in selected
                    ):
                        _collect_evotags(connection, retained)

    stems = {name.rsplit("_", 1)[0] for name in selected}
    for prescribed_name in ("vnc_18", "vnc_nmj"):
        prescribed = json_data.get(prescribed_name, {})
        if not isinstance(prescribed, dict):
            continue
        for section_name in ("dorsal_conns", "ventral_conns"):
            section = prescribed.get(section_name, {})
            if not isinstance(section, dict):
                continue
            for stem in stems:
                if stem in section:
                    _collect_evotags(section[stem], retained)

    head_stems = {"RMDD", "RMDV", "SMDD", "SMDV"}
    if stems.intersection(head_stems) and isinstance(stretch_receptor, dict):
        if "sr_head_gain" in stretch_receptor:
            _collect_evotags(stretch_receptor["sr_head_gain"], retained)

    result = strip_unretained_evotags(json_data, retained)
    ranges = result.get("evolvable_ranges")
    if isinstance(ranges, dict):
        for evotag in list(ranges):
            if evotag != "value" and str(evotag) not in retained:
                ranges.pop(evotag, None)
        legacy_entries = ranges.get("value")
        if isinstance(legacy_entries, list):
            ranges["value"] = [
                entry
                for entry in legacy_entries
                if isinstance(entry, dict)
                and (
                    str(entry.get("evotag")) in retained
                    or any(str(name) in retained for name in entry)
                )
            ]

    evolved_used = result.get("evolved_used")
    used = evolved_used.get("value") if isinstance(evolved_used, dict) else evolved_used
    if isinstance(used, list):
        used[:] = [evotag for evotag in used if str(evotag) in retained]
    return result


def _collect_model_evotags(value, at_root=False, output=None):
    if output is None:
        output = set()
    if isinstance(value, dict):
        evotag = value.get("evotag")
        if isinstance(evotag, dict) and "value" in evotag:
            evotag = evotag["value"]
        if isinstance(evotag, (str, int)) and not isinstance(evotag, bool):
            output.add(evotag)
        for key, child in value.items():
            if at_root and key in {"evolvable_ranges", "evolved_used", "Evolvable"}:
                continue
            _collect_model_evotags(child, output=output)
    elif isinstance(value, list):
        for child in value:
            _collect_model_evotags(child, output=output)
    return output


def _remove_unused_evotags(result, candidate_evotags):
    unused_evotags = set(candidate_evotags) - _collect_model_evotags(
        result, at_root=True
    )
    ranges = result.get("evolvable_ranges")
    if isinstance(ranges, dict):
        for evotag in unused_evotags:
            ranges.pop(str(evotag), None)

    evolved_used = result.get("evolved_used")
    if isinstance(evolved_used, dict) and isinstance(evolved_used.get("value"), list):
        evolved_used["value"] = [
            evotag for evotag in evolved_used["value"] if evotag not in unused_evotags
        ]


def _set_evolvable_range_active(evolvable_ranges, evotag_name, active):
    if not isinstance(evolvable_ranges, dict):
        raise KeyError("JSON does not contain an 'evolvable_ranges' dictionary")

    if evotag_name in evolvable_ranges and isinstance(
        evolvable_ranges[evotag_name], dict
    ):
        evolvable_ranges[evotag_name]["active"] = active
        return True

    legacy_entries = evolvable_ranges.get("value")
    if isinstance(legacy_entries, list):
        for entry in legacy_entries:
            if not isinstance(entry, dict):
                continue
            if entry.get("evotag") == evotag_name:
                entry["active"] = active
                return True
            if (
                len(entry) == 1
                and evotag_name in entry
                and isinstance(entry[evotag_name], dict)
            ):
                entry[evotag_name]["active"] = active
                return True

    return False


def set_vnc_set_from_this(json_data, object_name, set_from_this):
    """Set vnc_nmj/vnc_18 set_from_this and toggle its evotags active state."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if object_name not in {"vnc_nmj", "vnc_18"}:
        raise ValueError("object_name must be 'vnc_nmj' or 'vnc_18'")
    if not isinstance(set_from_this, bool):
        raise TypeError("set_from_this must be a bool")
    if object_name not in json_data or not isinstance(json_data[object_name], dict):
        raise KeyError("JSON does not contain a '{}' object".format(object_name))

    result = copy.deepcopy(json_data)
    source_object = result[object_name]
    set_from_this_entry = source_object.get("set_from_this")
    if isinstance(set_from_this_entry, dict):
        set_from_this_entry["value"] = set_from_this
    else:
        source_object["set_from_this"] = {"value": set_from_this}

    evotags = {str(evotag) for evotag in _collect_evotags(source_object)}
    evolvable_ranges = result.get("evolvable_ranges")
    missing_evotags = []
    for evotag in sorted(evotags):
        if not _set_evolvable_range_active(evolvable_ranges, evotag, set_from_this):
            missing_evotags.append(evotag)
    if missing_evotags:
        raise KeyError(
            "The following evotags from '{}' are not in evolvable_ranges: {}".format(
                object_name, ", ".join(missing_evotags)
            )
        )

    return result


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


def find_mfunc_paths(json_data):
    """Return all mfunc entries indexed by their JSON paths."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")

    mfuncs = {}

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
            for key, child in value.items():
                child_path = path + (key,)
                if key in ("mfunc", "m_func"):
                    mfuncs[path_string(child_path)] = {
                        "path": list(child_path),
                        "mfunc": copy.deepcopy(child),
                    }
                find_matches(child, child_path)
        elif isinstance(value, list):
            for index, child in enumerate(value):
                find_matches(child, path + (index,))

    find_matches(json_data)
    return mfuncs


def find_mfunc_objects(json_data):
    """Return the JSON hierarchy containing objects with an mfunc field."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")

    no_match = object()
    context_keys = (
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

    def object_context(value):
        return {key: copy.deepcopy(value[key]) for key in context_keys if key in value}

    def matching_hierarchy(value):
        if isinstance(value, dict):
            matches = {}
            for key, child in value.items():
                if key in ("mfunc", "m_func"):
                    matches[key] = copy.deepcopy(child)
                    continue
                child_match = matching_hierarchy(child)
                if child_match is not no_match:
                    matches[key] = child_match
            if matches:
                matches = {**object_context(value), **matches}
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


def _ensure_driving_input_connection_containers(result, input_id):
    driving_inputs = result.setdefault(
        "driving_inputs",
        {
            "inputs": {
                "message": "Driving input strength to Nervous System",
                "value": [],
            },
            "weights": {
                "message": (
                    "Weights of driving inputs to Nervous System in sparse format"
                ),
                "value": [],
            },
        },
    )
    if not isinstance(driving_inputs, dict):
        raise TypeError("'driving_inputs' must be a dictionary")

    inputs_object = driving_inputs.setdefault(
        "inputs",
        {
            "message": "Driving input strength to Nervous System",
            "value": [],
        },
    )
    if not isinstance(inputs_object, dict):
        raise TypeError("'driving_inputs.inputs' must be a dictionary")
    if inputs_object.get("value") is None:
        inputs_object["value"] = []
    inputs = inputs_object.get("value")
    if not isinstance(inputs, list):
        raise TypeError("'driving_inputs.inputs.value' must be a list")
    inputs_object.setdefault("message", "Driving input strength to Nervous System")

    input_exists = False
    for entry in inputs:
        if not isinstance(entry, dict):
            raise TypeError("Each driving input must be a dictionary")
        entry_id = _driving_input_id(entry.get("input_num"))
        entry["input_num"] = entry_id
        if entry_id == input_id:
            input_exists = True
    if not input_exists:
        inputs.append({"input_num": input_id, "strength": {"value": 0.0}})

    weights_object = driving_inputs.setdefault(
        "weights",
        {
            "message": ("Weights of driving inputs to Nervous System in sparse format"),
            "value": [],
        },
    )
    if not isinstance(weights_object, dict):
        raise TypeError("'driving_inputs.weights' must be a dictionary")
    if weights_object.get("value") is None:
        weights_object["value"] = []
    weights = weights_object.get("value")
    if not isinstance(weights, list):
        raise TypeError("'driving_inputs.weights.value' must be a list")
    weights_object.setdefault(
        "message", "Weights of driving inputs to Nervous System in sparse format"
    )
    return weights


def _add_driving_input_connection_inplace(result, input_id, cell_name, weight):
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
    cells = nervous_system.get("cells")
    if not isinstance(cells, dict):
        raise KeyError("JSON does not contain 'nervous_system.cells'")
    if cell_name not in cells:
        raise KeyError(
            "Cell {!r} was not found in nervous_system.cells".format(cell_name)
        )

    weights = _ensure_driving_input_connection_containers(result, input_id)
    for connection in weights:
        if not isinstance(connection, dict):
            raise TypeError("Each driving-input connection must be a dictionary")
        from_input = _driving_input_id(connection.get("from_input"))
        connection["from_input"] = from_input
        if from_input == input_id and connection.get("to_cell") == cell_name:
            return

    weights.append(
        {
            "from_input": input_id,
            "to_cell": cell_name,
            "weight": {"value": float(weight)},
        }
    )


def add_driving_input_connection(
    json_data,
    input_num,
    cell_name,
    weight=None,
    random_seed=None,
):
    """Return a copy with a driving-input-to-cell connection added if absent."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    input_id = _driving_input_id(input_num)
    if not isinstance(cell_name, str) or not cell_name:
        raise ValueError("cell_name must be a non-empty string")
    if weight is not None and (
        isinstance(weight, bool) or not isinstance(weight, (int, float))
    ):
        raise TypeError("weight must be a number or None")
    if random_seed is not None and (
        isinstance(random_seed, bool) or not isinstance(random_seed, int)
    ):
        raise TypeError("random_seed must be an integer or None")

    if weight is None:
        weight = random.Random(random_seed).uniform(-1.0, 1.0)

    result = copy.deepcopy(json_data)
    _add_driving_input_connection_inplace(result, input_id, cell_name, weight)
    return result


def add_driving_input_connections_by_cell_stem(
    json_data,
    input_num,
    cell_stem,
    weight=None,
    random_seed=None,
):
    """Return a copy with one driving-input connection to each matching cell."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    input_id = _driving_input_id(input_num)
    if not isinstance(cell_stem, str) or not cell_stem:
        raise ValueError("cell_stem must be a non-empty string")
    if weight is not None and (
        isinstance(weight, bool) or not isinstance(weight, (int, float))
    ):
        raise TypeError("weight must be a number or None")
    if random_seed is not None and (
        isinstance(random_seed, bool) or not isinstance(random_seed, int)
    ):
        raise TypeError("random_seed must be an integer or None")

    cell_names = get_cell_names_by_stem(json_data, cell_stem)
    if not cell_names:
        raise ValueError(
            "No nervous-system cells were found with stem {!r}".format(cell_stem)
        )

    rng = random.Random(random_seed)
    result = copy.deepcopy(json_data)
    for cell_name in cell_names:
        connection_weight = weight
        if connection_weight is None:
            connection_weight = rng.uniform(-1.0, 1.0)
        _add_driving_input_connection_inplace(
            result, input_id, cell_name, connection_weight
        )
    return result


def add_driving_input_connection_evotag(
    json_data,
    input_num,
    cell_name,
    evotag_name,
):
    """Return a copy with an evotag added to a driving-input connection weight."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    input_id = _driving_input_id(input_num)
    if not isinstance(cell_name, str) or not cell_name:
        raise ValueError("cell_name must be a non-empty string")
    if not isinstance(evotag_name, str) or not evotag_name:
        raise ValueError("evotag_name must be a non-empty string")

    result = copy.deepcopy(json_data)
    driving_inputs = result.get("driving_inputs")
    if not isinstance(driving_inputs, dict):
        raise KeyError("JSON does not contain a 'driving_inputs' object")
    weights_object = driving_inputs.get("weights")
    if not isinstance(weights_object, dict):
        raise KeyError("JSON does not contain 'driving_inputs.weights'")
    weights = weights_object.get("value")
    if not isinstance(weights, list):
        raise TypeError("'driving_inputs.weights.value' must be a list")

    for connection_index, connection in enumerate(weights):
        if not isinstance(connection, dict):
            raise TypeError("Each driving-input connection must be a dictionary")
        from_input = _driving_input_id(connection.get("from_input"))
        connection["from_input"] = from_input
        if from_input == input_id and connection.get("to_cell") == cell_name:
            return add_evotag(
                result,
                [
                    "driving_inputs",
                    "weights",
                    "value",
                    connection_index,
                    "weight",
                ],
                evotag_name=evotag_name,
            )

    raise KeyError(
        "No driving-input connection from {!r} to cell {!r} was found".format(
            input_id, cell_name
        )
    )


def set_all_driving_input_connection_weights(json_data, weight):
    """Return a copy with all driving-input-to-cell weights set to one value."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if isinstance(weight, bool) or not isinstance(weight, (int, float)):
        raise TypeError("weight must be a number")

    result = copy.deepcopy(json_data)
    driving_inputs = result.get("driving_inputs")
    if driving_inputs is None:
        return result
    if not isinstance(driving_inputs, dict):
        raise TypeError("'driving_inputs' must be a dictionary")

    weights_object = driving_inputs.get("weights")
    if weights_object is None:
        return result
    if not isinstance(weights_object, dict):
        raise TypeError("'driving_inputs.weights' must be a dictionary")

    weights = weights_object.get("value")
    if weights is None:
        return result
    if not isinstance(weights, list):
        raise TypeError("'driving_inputs.weights.value' must be a list")

    for connection in weights:
        if not isinstance(connection, dict):
            raise TypeError("Each driving-input connection must be a dictionary")
        weight_object = connection.get("weight")
        if not isinstance(weight_object, dict):
            raise TypeError("Each driving-input connection weight must be a dictionary")
        weight_object["value"] = float(weight)

    return result


def _get_cell_connection(
    json_data,
    from_cell,
    to_cell,
    connection_key,
    reciprocal=False,
):
    """Return a copy of a matching nervous-system connection and its index."""
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
        return None, None
    connection_object = nervous_system.get(connection_key)
    if connection_object is None:
        return None, None
    if not isinstance(connection_object, dict):
        raise TypeError(
            "'nervous_system.{}' must be a dictionary".format(connection_key)
        )
    connections = connection_object.get("value")
    if connections is None:
        return None, None
    if not isinstance(connections, list):
        raise TypeError(
            "'nervous_system.{}.value' must be a list".format(connection_key)
        )

    for connection_index, connection in enumerate(connections):
        if not isinstance(connection, dict):
            raise TypeError("Each connection must be a dictionary")
        direct_match = (
            connection.get("from") == from_cell and connection.get("to") == to_cell
        )
        reverse_match = reciprocal and (
            connection.get("from") == to_cell and connection.get("to") == from_cell
        )
        if direct_match or reverse_match:
            return copy.deepcopy(connection), connection_index
    return None, None


def _get_cell_connections_for_cell(json_data, cell_name, connection_key):
    """Return copies and indices of nervous-system connections involving one cell."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(cell_name, str) or not cell_name:
        raise ValueError("cell_name must be a non-empty string")

    nervous_system = json_data.get("nervous_system")
    if not isinstance(nervous_system, dict):
        return [], []
    connection_object = nervous_system.get(connection_key)
    if connection_object is None:
        return [], []
    if not isinstance(connection_object, dict):
        raise TypeError(
            "'nervous_system.{}' must be a dictionary".format(connection_key)
        )
    connections = connection_object.get("value")
    if connections is None:
        return [], []
    if not isinstance(connections, list):
        raise TypeError(
            "'nervous_system.{}.value' must be a list".format(connection_key)
        )

    matching_connections = []
    matching_indices = []
    for connection_index, connection in enumerate(connections):
        if not isinstance(connection, dict):
            raise TypeError("Each connection must be a dictionary")
        if connection.get("from") == cell_name or connection.get("to") == cell_name:
            matching_connections.append(copy.deepcopy(connection))
            matching_indices.append(connection_index)
    return matching_connections, matching_indices


def get_chemical_connection(json_data, from_cell, to_cell):
    """Return (connection, index) for a chemical connection, or (None, None)."""
    return _get_cell_connection(
        json_data,
        from_cell,
        to_cell,
        "chemical_conns",
    )


def get_electrical_connection(json_data, first_cell, second_cell):
    """Return (connection, index) for an electrical connection, or (None, None)."""
    return _get_cell_connection(
        json_data,
        first_cell,
        second_cell,
        "electrical_conns",
        reciprocal=True,
    )


def get_nmj_connection(json_data, muscle_side, from_cell, to_muscle=None):
    """Return NMJ connection data for one cell.

    With to_muscle supplied, return (connection, index) for one connection. If
    to_muscle is omitted, return (connections, indices) for all muscles that
    from_cell connects to on the selected side.
    """
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(muscle_side, str):
        raise TypeError("muscle_side must be a string")
    muscle_side = muscle_side.lower()
    if muscle_side not in {"dorsal", "ventral"}:
        raise ValueError("muscle_side must be 'dorsal' or 'ventral'")
    if not isinstance(from_cell, str) or not from_cell:
        raise ValueError("from_cell must be a non-empty string")

    nmj_key = "{}_nmj".format(muscle_side)
    nmj_object = json_data.get(nmj_key)
    if nmj_object is None:
        return ([], []) if to_muscle is None else (None, None)
    if not isinstance(nmj_object, dict):
        raise TypeError("'{}' must be a dictionary".format(nmj_key))
    weights_object = nmj_object.get("weights")
    if weights_object is None:
        return ([], []) if to_muscle is None else (None, None)
    if not isinstance(weights_object, dict):
        raise TypeError("'{}.weights' must be a dictionary".format(nmj_key))
    connections = weights_object.get("value")
    if connections is None:
        return ([], []) if to_muscle is None else (None, None)
    if not isinstance(connections, list):
        raise TypeError("'{}.weights.value' must be a list".format(nmj_key))

    matching_connections = []
    matching_indices = []
    for connection_index, connection in enumerate(connections):
        if not isinstance(connection, dict):
            raise TypeError("Each NMJ connection must be a dictionary")
        if connection.get("from_cell") != from_cell:
            continue
        if to_muscle is None:
            matching_connections.append(copy.deepcopy(connection))
            matching_indices.append(connection_index)
        elif connection.get("to_musc") == to_muscle:
            return copy.deepcopy(connection), connection_index
    if to_muscle is None:
        return matching_connections, matching_indices
    return None, None


def get_body_connection(json_data, body_side, from_muscle, to_segment=None):
    """Return body connection data for one muscle.

    With to_segment supplied, return (connection, index) for one connection. If
    to_segment is omitted, return (connections, indices) for all body segments
    that from_muscle connects to on the selected side.
    """
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(body_side, str):
        raise TypeError("body_side must be a string")
    body_side = body_side.lower()
    if body_side not in {"dorsal", "ventral"}:
        raise ValueError("body_side must be 'dorsal' or 'ventral'")

    body_key = "{}_body".format(body_side)
    body_object = json_data.get(body_key)
    if body_object is None:
        return ([], []) if to_segment is None else (None, None)
    if not isinstance(body_object, dict):
        raise TypeError("'{}' must be a dictionary".format(body_key))
    weights_object = body_object.get("weights")
    if weights_object is None:
        return ([], []) if to_segment is None else (None, None)
    if not isinstance(weights_object, dict):
        raise TypeError("'{}.weights' must be a dictionary".format(body_key))
    connections = weights_object.get("value")
    if connections is None:
        return ([], []) if to_segment is None else (None, None)
    if not isinstance(connections, list):
        raise TypeError("'{}.weights.value' must be a list".format(body_key))

    matching_connections = []
    matching_indices = []
    for connection_index, connection in enumerate(connections):
        if not isinstance(connection, dict):
            raise TypeError("Each body connection must be a dictionary")
        if connection.get("from_musc") != from_muscle:
            continue
        if to_segment is None:
            matching_connections.append(copy.deepcopy(connection))
            matching_indices.append(connection_index)
        elif connection.get("to_seg") == to_segment:
            return copy.deepcopy(connection), connection_index
    if to_segment is None:
        return matching_connections, matching_indices
    return None, None


def get_chemical_connections_for_cell(json_data, cell_name):
    """Return (connections, indices) for chemical connections involving a cell."""
    return _get_cell_connections_for_cell(json_data, cell_name, "chemical_conns")


def get_electrical_connections_for_cell(json_data, cell_name):
    """Return (connections, indices) for electrical connections involving a cell."""
    return _get_cell_connections_for_cell(json_data, cell_name, "electrical_conns")


def _set_connection_weight(json_data, connection_key, connection_index, weight):
    """Return a copy with one nervous-system connection weight changed."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if isinstance(connection_index, bool) or not isinstance(connection_index, int):
        raise TypeError("connection_index must be an integer")
    if isinstance(weight, bool) or not isinstance(weight, (int, float)):
        raise TypeError("weight must be a number")

    result = copy.deepcopy(json_data)
    nervous_system = result.get("nervous_system")
    if not isinstance(nervous_system, dict):
        raise KeyError("JSON does not contain a 'nervous_system' object")
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
    try:
        connection = connections[connection_index]
    except IndexError:
        raise IndexError(
            "{} connection index {} is out of range".format(
                connection_key, connection_index
            )
        )
    if not isinstance(connection, dict):
        raise TypeError(
            "Connection at index {} must be a dictionary".format(connection_index)
        )
    connection.setdefault("weight", {})
    if not isinstance(connection["weight"], dict):
        raise TypeError(
            "Connection weight at index {} must be a dictionary".format(
                connection_index
            )
        )
    connection["weight"]["value"] = float(weight)
    return result


def set_chemical_connection_weight(json_data, from_cell, to_cell, weight):
    """Return a copy with a directed chemical connection weight changed."""
    _, connection_index = get_chemical_connection(json_data, from_cell, to_cell)
    if connection_index is None:
        raise KeyError(
            "Chemical connection from {!r} to {!r} was not found".format(
                from_cell, to_cell
            )
        )
    return _set_connection_weight(json_data, "chemical_conns", connection_index, weight)


def set_electrical_connection_weight(json_data, first_cell, second_cell, weight):
    """Return a copy with an electrical connection weight changed."""
    _, connection_index = get_electrical_connection(json_data, first_cell, second_cell)
    if connection_index is None:
        raise KeyError(
            "Electrical connection between {!r} and {!r} was not found".format(
                first_cell, second_cell
            )
        )
    return _set_connection_weight(
        json_data, "electrical_conns", connection_index, weight
    )


def set_nmj_connection_weight(json_data, muscle_side, from_cell, to_muscle, weight):
    """Return a copy with one dorsal/ventral NMJ connection weight changed."""
    _, connection_index = get_nmj_connection(
        json_data, muscle_side, from_cell, to_muscle
    )
    if connection_index is None:
        raise KeyError(
            "{} NMJ connection from {!r} to muscle {!r} was not found".format(
                str(muscle_side).lower(), from_cell, to_muscle
            )
        )
    if isinstance(weight, bool) or not isinstance(weight, (int, float)):
        raise TypeError("weight must be a number")

    muscle_side = muscle_side.lower()
    nmj_key = "{}_nmj".format(muscle_side)
    result = copy.deepcopy(json_data)
    try:
        connection = result[nmj_key]["weights"]["value"][connection_index]
    except IndexError:
        raise IndexError(
            "{} connection index {} is out of range".format(nmj_key, connection_index)
        )
    if not isinstance(connection, dict):
        raise TypeError(
            "NMJ connection at index {} must be a dictionary".format(connection_index)
        )
    connection.setdefault("weight", {})
    if not isinstance(connection["weight"], dict):
        raise TypeError(
            "NMJ connection weight at index {} must be a dictionary".format(
                connection_index
            )
        )
    connection["weight"]["value"] = float(weight)
    return result


def set_body_connection_weight(json_data, body_side, from_musc, to_seg, weight):
    """Return a copy with one dorsal/ventral body connection weight changed."""
    _, connection_index = get_body_connection(json_data, body_side, from_musc, to_seg)
    if connection_index is None:
        raise KeyError(
            "{} body connection from muscle {!r} to segment {!r} was not found".format(
                str(body_side).lower(), from_musc, to_seg
            )
        )
    if isinstance(weight, bool) or not isinstance(weight, (int, float)):
        raise TypeError("weight must be a number")

    body_side = body_side.lower()
    body_key = "{}_body".format(body_side)
    result = copy.deepcopy(json_data)
    try:
        connection = result[body_key]["weights"]["value"][connection_index]
    except IndexError:
        raise IndexError(
            "{} connection index {} is out of range".format(body_key, connection_index)
        )
    if not isinstance(connection, dict):
        raise TypeError(
            "Body connection at index {} must be a dictionary".format(connection_index)
        )
    connection.setdefault("weight", {})
    if not isinstance(connection["weight"], dict):
        raise TypeError(
            "Body connection weight at index {} must be a dictionary".format(
                connection_index
            )
        )
    connection["weight"]["value"] = float(weight)
    return result


def set_body_connection(json_data, body_side, from_musc, to_seg, weight):
    """Return a copy with one dorsal/ventral body connection weight changed."""
    return set_body_connection_weight(json_data, body_side, from_musc, to_seg, weight)


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


def set_cell_parameter(json_data, cell_name, parameter_name, value):
    """Return a copy with one nervous-system cell parameter value changed."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    for argument_name, argument_value in (
        ("cell_name", cell_name),
        ("parameter_name", parameter_name),
    ):
        if not isinstance(argument_value, str) or not argument_value:
            raise ValueError("{} must be a non-empty string".format(argument_name))
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise TypeError("value must be a number")

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
    if isinstance(parameter["value"], bool) or not isinstance(
        parameter["value"], (int, float)
    ):
        raise TypeError(
            "{}.{} must currently have a numeric value".format(
                cell_name, parameter_name
            )
        )

    parameter["value"] = float(value)
    return result


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
    """Return a copy with the value at an existing JSON path replaced.

    If the path identifies a JSON parameter object with a ``value`` field, the
    contained value is replaced. For example, ``["nervous_system", "cells",
    "DA_0", "bias"]`` updates ``["bias"]["value"]``.
    """
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
        target = current[final_key]
        if final_key != "value" and isinstance(target, dict) and "value" in target:
            target["value"] = copy.deepcopy(new_value)
        else:
            current[final_key] = copy.deepcopy(new_value)
    elif isinstance(current, list):
        if isinstance(final_key, bool) or not isinstance(final_key, int):
            raise TypeError(
                "List path component at position {} must be an integer".format(
                    final_position
                )
            )
        try:
            target = current[final_key]
        except IndexError:
            raise IndexError(
                "List index {} is out of range at path position {}".format(
                    final_key, final_position
                )
            )
        if isinstance(target, dict) and "value" in target:
            target["value"] = copy.deepcopy(new_value)
        else:
            current[final_key] = copy.deepcopy(new_value)
    else:
        raise TypeError(
            "JSON path reaches a non-container at position {}".format(final_position)
        )

    return result


def set_evotag_value(json_data, evotag_name, new_value):
    """Return a copy with all values using an evotag replaced."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(evotag_name, str) or not evotag_name:
        raise ValueError("evotag_name must be a non-empty string")

    result = copy.deepcopy(json_data)
    matches = 0
    missing_value_paths = []

    def normalize_evotag(value):
        if isinstance(value, bool):
            return None
        if isinstance(value, int):
            return "evotag_{}".format(value)
        if isinstance(value, str):
            return value
        return None

    def path_string(path):
        result_path = ""
        for part in path:
            if isinstance(part, int):
                result_path += "[{}]".format(part)
            else:
                if result_path:
                    result_path += "."
                result_path += str(part)
        return result_path

    def update_matches(value, path=()):
        nonlocal matches
        if isinstance(value, dict):
            if normalize_evotag(value.get("evotag")) == evotag_name:
                if "value" not in value:
                    missing_value_paths.append(path_string(path))
                else:
                    value["value"] = copy.deepcopy(new_value)
                    matches += 1
            for key, child in value.items():
                update_matches(child, path + (key,))
        elif isinstance(value, list):
            for index, child in enumerate(value):
                update_matches(child, path + (index,))

    update_matches(result)

    if missing_value_paths:
        raise TypeError(
            "Evotag {!r} was found on objects without a 'value' field: {}".format(
                evotag_name, ", ".join(missing_value_paths)
            )
        )
    if matches == 0:
        raise KeyError("Evotag {!r} was not found in the JSON data".format(evotag_name))

    return result


def add_mfunc(json_data, keys, f_ind, args=None, **kwargs):
    """Return a copy with an mfunc added to an object containing 'value'."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(keys, (list, tuple)) or not keys:
        raise ValueError("keys must be a non-empty list or tuple")
    function_name = _mfunc_name(f_ind)
    if args is not None and not isinstance(args, dict):
        raise TypeError("args must be a dictionary or None")

    mfunc_args = {}
    if args is not None:
        mfunc_args.update(args)
    for key, value in kwargs.items():
        if key in mfunc_args:
            raise ValueError("mfunc argument {!r} was supplied twice".format(key))
        mfunc_args[key] = value
    for key in mfunc_args:
        if not isinstance(key, str) or not key:
            raise ValueError("All mfunc argument keys must be non-empty strings")

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
            parameter = parameter[key]
    else:
        parameter = current

    if not isinstance(parameter, dict) or "value" not in parameter:
        raise TypeError("The JSON path must identify an object containing 'value'")

    parameter["mfunc"] = {"f_ind": function_name, **copy.deepcopy(mfunc_args)}
    return result


def delete_mfunc(json_data, keys):
    """Return a copy with an mfunc removed from a JSON value object."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(keys, (list, tuple)) or not keys:
        raise ValueError("keys must be a non-empty list or tuple")

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
            parameter = parameter[key]
    else:
        parameter = current

    if not isinstance(parameter, dict) or "value" not in parameter:
        raise TypeError("The JSON path must identify an object containing 'value'")

    parameter.pop("mfunc", None)
    parameter.pop("m_func", None)
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


def set_evolvable_range(
    json_data,
    evotag_name,
    active=None,
    lower_limit=None,
    upper_limit=None,
    name=None,
):
    """Return a copy with selected fields changed for an evolvable range."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    if not isinstance(evotag_name, str) or not evotag_name:
        raise ValueError("evotag_name must be a non-empty string")
    if active is not None and not isinstance(active, bool):
        raise TypeError("active must be a bool or None")
    for parameter_name, value in (
        ("lower_limit", lower_limit),
        ("upper_limit", upper_limit),
    ):
        if value is not None and (
            isinstance(value, bool) or not isinstance(value, (int, float))
        ):
            raise TypeError("{} must be a number or None".format(parameter_name))
    if name is not None and (not isinstance(name, str) or not name):
        raise ValueError("name must be a non-empty string or None")

    result = copy.deepcopy(json_data)
    evolvable_ranges = result.get("evolvable_ranges")
    if not isinstance(evolvable_ranges, dict):
        raise KeyError("JSON does not contain an 'evolvable_ranges' dictionary")

    range_entry = None
    if evotag_name in evolvable_ranges and isinstance(
        evolvable_ranges[evotag_name], dict
    ):
        range_entry = evolvable_ranges[evotag_name]
    else:
        legacy_entries = evolvable_ranges.get("value")
        if isinstance(legacy_entries, list):
            for entry in legacy_entries:
                if not isinstance(entry, dict):
                    continue
                if entry.get("evotag") == evotag_name:
                    range_entry = entry
                    break
                if (
                    len(entry) == 1
                    and evotag_name in entry
                    and isinstance(entry[evotag_name], dict)
                ):
                    range_entry = entry[evotag_name]
                    break

    if range_entry is None:
        raise KeyError("Evotag {!r} is not in evolvable_ranges".format(evotag_name))

    if active is not None:
        range_entry["active"] = active
    if lower_limit is not None:
        range_entry["lower_limit"] = float(lower_limit)
    if upper_limit is not None:
        range_entry["upper_limit"] = float(upper_limit)
    if name is not None:
        range_entry["name"] = name

    return result


def delete_driving_input(json_data, input_num):
    """Return a copy with one driving input and its outgoing weights removed."""
    if not isinstance(json_data, dict):
        raise TypeError("json_data must be a dictionary")
    input_id = _driving_input_id(input_num)

    result = copy.deepcopy(json_data)
    driving_inputs = result.get("driving_inputs")
    if not isinstance(driving_inputs, dict):
        raise KeyError("JSON does not contain a 'driving_inputs' object")

    inputs_object = driving_inputs.get("inputs")
    if not isinstance(inputs_object, dict):
        raise KeyError("JSON does not contain 'driving_inputs.inputs'")
    inputs = inputs_object.get("value")
    if not isinstance(inputs, list):
        raise TypeError("'driving_inputs.inputs.value' must be a list")

    removed_evotags = set()
    remaining_inputs = []
    found_input = False
    for entry in inputs:
        if not isinstance(entry, dict):
            raise TypeError("Each driving input must be a dictionary")
        entry_id = _driving_input_id(entry.get("input_num"))
        if entry_id == input_id:
            found_input = True
            _collect_evotags(entry, removed_evotags)
            continue
        entry["input_num"] = entry_id
        remaining_inputs.append(entry)
    if not found_input:
        raise KeyError("Driving input {!r} does not exist".format(input_id))
    inputs_object["value"] = remaining_inputs

    weights_object = driving_inputs.setdefault("weights", {"value": []})
    if not isinstance(weights_object, dict):
        raise TypeError("'driving_inputs.weights' must be a dictionary")
    weights = weights_object.get("value")
    if weights is None:
        weights = []
    if not isinstance(weights, list):
        raise TypeError("'driving_inputs.weights.value' must be a list")

    remaining_weights = []
    for connection in weights:
        if not isinstance(connection, dict):
            raise TypeError("Each driving input weight must be a dictionary")
        from_input = _driving_input_id(connection.get("from_input"))
        if from_input == input_id:
            _collect_evotags(connection, removed_evotags)
            continue
        connection["from_input"] = from_input
        remaining_weights.append(connection)
    weights_object["value"] = remaining_weights

    input_switcher = result.get("input_switcher")
    if isinstance(input_switcher, dict):
        switcher_inputs = input_switcher.get("inputs", {}).get("value")
        if switcher_inputs is not None and not isinstance(switcher_inputs, list):
            raise TypeError("'input_switcher.inputs.value' must be a list")
        if isinstance(switcher_inputs, list):
            legacy_zero_based = False
            for pattern in switcher_inputs:
                if not isinstance(pattern, dict):
                    continue
                pattern_values = pattern.get("value")
                if not isinstance(pattern_values, list):
                    continue
                for entry in pattern_values:
                    if isinstance(entry, dict) and entry.get("input_num") == 0:
                        legacy_zero_based = True
            kept_patterns = []
            removed_pattern_ids = set()
            for pattern in switcher_inputs:
                if not isinstance(pattern, dict):
                    raise TypeError("Each input-switcher pattern must be a dictionary")
                pattern_id = _input_switcher_id(pattern.get("input_index"))
                pattern_values = pattern.get("value")
                if pattern_values is None:
                    kept_patterns.append(pattern)
                    continue
                if not isinstance(pattern_values, list):
                    raise TypeError("Each input-switcher pattern value must be a list")
                kept_values = []
                for entry in pattern_values:
                    if not isinstance(entry, dict):
                        raise TypeError(
                            "Each input-switcher pattern entry must be a dictionary"
                        )
                    entry_input_num = entry.get("input_num")
                    if legacy_zero_based and isinstance(entry_input_num, int):
                        entry_id = _driving_input_id(entry_input_num + 1)
                    else:
                        entry_id = _driving_input_id(entry_input_num)
                    if entry_id == input_id:
                        _collect_evotags(entry, removed_evotags)
                        continue
                    entry["input_num"] = entry_id
                    kept_values.append(entry)
                if kept_values:
                    pattern["input_index"] = pattern_id
                    pattern["value"] = kept_values
                    kept_patterns.append(pattern)
                else:
                    _collect_evotags(pattern, removed_evotags)
                    removed_pattern_ids.add(pattern_id)
            input_switcher["inputs"]["value"] = kept_patterns
            size_object = input_switcher.setdefault("size", {"value": 0})
            if not isinstance(size_object, dict):
                raise TypeError("'input_switcher.size' must be a dictionary")
            size_object["value"] = len(kept_patterns)

            if removed_pattern_ids:
                scheduled = input_switcher.get("input_indices", {}).get("value")
                periods = input_switcher.get("time_periods", {}).get("value")
                if scheduled is not None or periods is not None:
                    if not isinstance(scheduled, list):
                        raise TypeError(
                            "'input_switcher.input_indices.value' must be a list"
                        )
                    if not isinstance(periods, list):
                        raise TypeError(
                            "'input_switcher.time_periods.value' must be a list"
                        )
                    if len(scheduled) != len(periods):
                        raise ValueError(
                            "input_switcher.input_indices and time_periods "
                            "must have the same length"
                        )
                    kept_schedule = []
                    kept_periods = []
                    for scheduled_id, period in zip(scheduled, periods):
                        scheduled_pattern_id = _input_switcher_id(scheduled_id)
                        if scheduled_pattern_id in removed_pattern_ids:
                            continue
                        kept_schedule.append(scheduled_pattern_id)
                        kept_periods.append(period)
                    input_switcher["input_indices"]["value"] = kept_schedule
                    input_switcher["time_periods"]["value"] = kept_periods
            if not kept_patterns:
                result.pop("input_switcher", None)

    if not remaining_inputs and not remaining_weights:
        result.pop("driving_inputs", None)

    _remove_unused_evotags(result, removed_evotags)
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
        input_id = _driving_input_id(entry.get("input_num"))
        input_number = _driving_input_index(input_id) + 1
        if input_number not in removed_input_numbers:
            entry["input_num"] = input_id
            remaining_inputs.append(entry)

    remaining_inputs.sort(key=lambda entry: _driving_input_index(entry["input_num"]))
    input_id_map = {
        entry["input_num"]: entry["input_num"] for entry in remaining_inputs
    }
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
        input_id = _driving_input_id(connection.get("from_input"))
        input_number = _driving_input_index(input_id) + 1
        if input_number in removed_input_numbers:
            collect_evotags(connection)
            continue
        if input_id not in input_id_map:
            raise ValueError(
                "Connection refers to missing driving input {}".format(input_id)
            )
        connection["from_input"] = input_id_map[input_id]
        remaining_weights.append(connection)
    driving_inputs.setdefault("weights", {})["value"] = remaining_weights

    for remaining_sensor in sensors.values():
        if not isinstance(remaining_sensor, dict):
            raise TypeError("Each sensor must be a dictionary")
        for key in ("ext_inp_1", "ext_inp_2"):
            old_index = remaining_sensor.get(key, {}).get("value")
            old_input_number = old_index + 1
            old_input_id = _driving_input_id(old_input_number)
            if old_input_id not in input_id_map:
                raise ValueError(
                    "Sensor refers to missing driving input {}".format(old_input_number)
                )
            remaining_sensor[key]["value"] = _driving_input_index(
                input_id_map[old_input_id]
            )

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
    """Delete a direct child directory from a folder below the current directory."""
    if not isinstance(subfolder_name, str) or subfolder_name in ("", ".", ".."):
        raise ValueError(
            "The first argument must be a folder path below the current directory"
        )
    if os.path.isabs(subfolder_name):
        parent_path = os.path.abspath(subfolder_name)
    else:
        parent_path = os.path.abspath(os.path.join(os.curdir, subfolder_name))

    cwd_path = os.path.abspath(os.curdir)
    parent_realpath = os.path.realpath(parent_path)
    cwd_realpath = os.path.realpath(cwd_path)
    if (
        parent_realpath == cwd_realpath
        or os.path.commonpath([cwd_realpath, parent_realpath]) != cwd_realpath
    ):
        raise ValueError(
            "The first argument must be a path below the current directory"
        )

    name = subsubfolder_name
    if (
        not isinstance(name, str)
        or name in ("", ".", "..")
        or os.path.isabs(name)
        or os.path.basename(name) != name
        or os.sep in name
        or (os.altsep is not None and os.altsep in name)
    ):
        raise ValueError("The second argument must be a single folder name, not a path")

    if not os.path.isdir(parent_path):
        return False
    if os.path.islink(parent_path):
        raise ValueError("Refusing to use a symbolic link as the parent folder")

    target_path = os.path.abspath(os.path.join(parent_path, subsubfolder_name))
    target_realpath = os.path.realpath(target_path)
    if os.path.commonpath([parent_realpath, target_realpath]) != parent_realpath:
        raise ValueError("Target must be below the parent folder")
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
