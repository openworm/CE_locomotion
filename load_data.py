#####
# Script to load generated data on worm motion/cell activity & generate graphical output
#####


import numpy as np
from matplotlib import pyplot as plt
import sys
import random

# import argparse
import os
import neuromlLocal.utils as utils
from matplotlib.ticker import MaxNLocator
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


def normalize_evolvable_range_entries(evolvable_ranges):
    entries = []
    if evolvable_ranges is None:
        return entries

    def evotag_number(evotag):
        if isinstance(evotag, int):
            return evotag
        if isinstance(evotag, str) and evotag.startswith("evotag_"):
            return int(evotag.replace("evotag_", "", 1))
        return evotag

    for entry in evolvable_ranges.get("value", []):
        if "evotag" in entry:
            entry = dict(entry)
            entry["evotag"] = evotag_number(entry["evotag"])
            entries.append(entry)
            continue

        if len(entry) != 1:
            continue

        evotag_key, attrs = next(iter(entry.items()))

        attrs = dict(attrs)
        attrs["evotag"] = evotag_number(evotag_key)
        entries.append(attrs)

    for evotag_key, attrs in evolvable_ranges.items():
        if evotag_key == "value" or not isinstance(attrs, dict):
            continue

        attrs = dict(attrs)
        attrs["evotag"] = evotag_number(evotag_key)
        entries.append(attrs)

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
    evol_data_diff_1 = evol_data / evol_data[0]
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
        get_evolvable_ranges(network_json_data)
    )
    if evolvables and "name" in evolvables[0]:
        phen_names = []
        phen_nums = []
        for val in evolvables:
            if not (("active" in val) & (not val["active"])):
                name = val["name"]
                for key, val2 in short_phen_names.items():
                    name = name.replace(key, val2)
                phen_names.append(name)
                phen_nums.append(val["evotag"])

    elif "PhenoNames" in network_json_data:
        phen_names = network_json_data["PhenoNames"]["value"]
        phen_nums = network_json_data["PhenoNamesNums"]["value"]
    else:
        print("PhenoNames needed for pheno plot")
        return

    # print("checkDict")
    # print(phen_names)

    if a.modelName == "CO18" or a.modelName == "CO18Full":
        network_json_data_RS18 = utils.getJsonFile(hf.dir_name + "/RS18_worm_data.json")
        phen_names += network_json_data_RS18["PhenoNames"]["value"]
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

    evol_data_full_diff0 = evol_data / evol_data[0]
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
    axs2[row_num, col_num].set_xticklabels(phen_names, rotation="vertical")
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
        get_evolvable_ranges(network_json_data)
    )
    if evolvables and "name" in evolvables[0]:
        phen_names = []
        phen_nums = []
        for val in evolvables:
            if not (("active" in val) & (not val["active"])):
                name = val["name"]
                for key, val2 in short_phen_names.items():
                    name = name.replace(key, val2)
                phen_names.append(name)
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
        phen_names,
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

    if a.modelName == "COW2DSR":
        plot_format = utils.getPlotFormat(network_json_data)
    else:
        plot_format = utils.plot_formats[a.modelName]

    def imshow_time_extent(t_values, row_count):
        if len(t_values) == 0:
            return [0, 0, 0, row_count]
        return [t_values[0], t_values[-1], 0, row_count]

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

    if a.modelName == "CO18" or a.modelName == "CO18Full":
        # network_json_data = utils.getJsonFile(hf.rename_file("worm_data.json"))
        CO18_size = utils.getNervousSystemSize(network_json_data)
        plot_format["data_sizes"] = [CO18_size, 2]
        plot_format["plot_cell_names"] = ["N" + str(i) for i in range(CO18_size)] + [
            "S" + str(i) for i in range(2)
        ]
        plot_format["plot_col_divs"] = [CO18_size, 2]

    def makeFigure(data_offset, data_size, title, label, plot_num):
        axs[plot_num, 0].set_title(title, fontsize=title_font_size)
        axs[plot_num, 1].set_title(title, fontsize=title_font_size)

        for i in range(data_offset, data_size + data_offset):
            axs[plot_num, 0].plot(
                t_data[data_seg],
                act_data[i][data_seg],
                label=label + " %i" % (i - data_offset),
                linewidth=0.5,
            )
            # axs[plot_num, 0].xaxis.set_ticklabels([])
        # plt.legend()

        data_list = act_data[data_offset : data_size + data_offset, data_seg]
        t_plot = t_data[data_seg]
        # axs[plot_num, 1].set_title("Body curvature", fontsize=title_font_size)
        axs[plot_num, 1].imshow(
            data_list,
            aspect="auto",
            interpolation="nearest",
            extent=imshow_time_extent(t_plot, data_list.shape[0]),
        )

        # axs[plot_num, 1].imshow(data_list, aspect="auto", interpolation="nearest")
        # axs[plot_num, 1].xaxis.set_ticklabels([])
        axs[plot_num, 1].yaxis.set_major_locator(MaxNLocator(integer=True))

    plot_rows = len(plot_format["fig_titles"])
    if plot_format["do_curv_plot"] or plot_format["do_body_plot"]:
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
    for val in zip(
        plot_format["data_sizes"], plot_format["fig_titles"], plot_format["fig_labels"]
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
    if plot_format["do_curv_plot"]:
        if os.path.isfile(hf.rename_file("curv_t.dat")):
            curv_file = hf.rename_file("curv_t.dat")
        else:
            curv_file = hf.rename_file("curv.dat")

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

    if plot_format["do_body_plot"]:
        if a.modelName == "CO" or a.modelName == "W2DCO":
            body_file = hf.rename_file("bodypos.dat")
        else:
            body_file = hf.rename_file("body.dat")
            """ if os.path.isfile(hf.rename_file("body_mm.dat")):
                body_file = hf.rename_file("body_mm.dat")
            else:
                body_file = hf.rename_file("body.dat") """

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
