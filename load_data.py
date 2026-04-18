#####
# Script to load generated data on worm motion/cell activity & generate graphical output
#####


import numpy as np
from matplotlib import pyplot as plt
import sys

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


colors30 = distinct_30_colors()
linestyles = ["-", "--", "-.", ":"]
style_cycle = cycler(color=colors30) + cycler(
    linestyle=list(itertools.islice(itertools.cycle(linestyles), len(colors30)))
)


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
    evol_data_diff_11 = sign(evol_data_diff_1) * np.log(np.abs(evol_data_diff_1))
    evol_data_diff_13 = evol_data - evol_data[0]
    evol_data_diff_131 = sign(evol_data_diff_13[1:]) * np.log(
        np.abs(evol_data_diff_13[1:])
    )

    return [evol_data_diff_13, evol_data_diff_131, evol_data_diff_1, evol_data_diff_11]


def plot_phenonames(
    plot_list=["rel_var", "var", ["initial_log", "final_log"], ["initial", "final"]],
    a=None,
):
    file = hf.rename_file("genhistory.dat")
    if not os.path.isfile(file): return
    evol_data_all = hf.load_nonragged_arrays(file)
    evol_data_1 = evol_data_all[len(evol_data_all) - 1]  # use only the last array

    # evol_data_1 = np.loadtxt(hf.rename_file("genhistory.dat"))

    worm_file = hf.get_worm_file()

    network_json_data = utils.getJsonFile(worm_file)
    vectsize = network_json_data["Evolutionary Optimization Parameters"]["VectSize"][
        "value"
    ]

    if hf.checkDictName(network_json_data, ["Evolvable", "value", 0, "name"]):
        evolvables = network_json_data["Evolvable"]["value"]
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

    print("checkDict")
    print(phen_names)

    if a.modelName == "CO18" or a.modelName == "CO18Full":
        network_json_data_RS18 = utils.getJsonFile(hf.dir_name + "/RS18_worm_data.json")
        phen_names += network_json_data_RS18["PhenoNames"]["value"]
        phen_nums += network_json_data_RS18["PhenoNamesNums"]["value"]

    phen_offset = vectsize * 2
    # phen_size = vectsize

    #avlentop = 1
    #evol_data = getAvData_1(evol_data_1, avlentop=avlentop)
    evol_data = evol_data_1

    # gen_index_orig = evol_data[:, 0]

    # generation number, phenotype number (first is gen index)

    evol_data = evol_data[:, 1 + phen_offset :]

    # evol_data_full_diff = (evol_data[-1] - evol_data[0]) / evol_data[0]

    evol_data_full_diff0 = evol_data / evol_data[0]
    evol_data_full_diff = sign(evol_data_full_diff0) * np.log(
        np.abs(evol_data_full_diff0)
    )

    avlentop = 5
    if hasattr(a, 'evoAvLen'):
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

    evol_data_log = sign(evol_data) * np.log(np.abs(evol_data))
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
        },
        "var": {
            "value": evol_data_full_diff,
            "title": "Signed log perc var",
            "color": "black",
        },
        "final_log": {
            "value": evol_data_fin,
            "title": "Signed log final value",
            "color": "red",
        },
        "initial_log": {
            "value": evol_data_init,
            "title": "Signed log initial value",
            "color": "black",
        },
        "final": {
            "value": evol_data_fin_actual,
            "title": "Final value",
            "color": "red",
        },
        "initial": {
            "value": evol_data_init_actual,
            "title": "Initial value",
            "color": "black",
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

    print(phen_name_list)

    fsize = 10
    plot_cols = 1
    plot_rows = len(plot_list)
    fig, axs = plt.subplots(plot_rows, plot_cols, figsize=(fsize, fsize), squeeze=False)
    fig2, axs2 = plt.subplots(
        plot_rows, plot_cols, figsize=(fsize, fsize), squeeze=False
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
                )
                val3 = evol_data_dict[val2]["value"]
                axs2[row_num, col_num].plot(
                    range(len(val3)),
                    val3,
                    label=evol_data_dict[val2]["title"],
                    color=evol_data_dict[val2]["color"],
                )
        else:
            val3 = evol_data_dict[val]["pheno_avs"]
            axs[row_num, col_num].plot(
                range(len(val3)),
                val3,
                label=evol_data_dict[val]["title"],
                color=evol_data_dict[val]["color"],
            )
            val3 = evol_data_dict[val]["value"]
            axs2[row_num, col_num].plot(
                range(len(val3)),
                val3,
                label=evol_data_dict[val]["title"],
                color=evol_data_dict[val]["color"],
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

    axs[row_num, col_num].set_xlabel("Phenotype #", fontsize=label_font_size)
    axs[row_num, col_num].set_xticklabels(phen_name_list, rotation="vertical")
    axs2[row_num, col_num].set_xlabel("Phenotype #", fontsize=label_font_size)
    axs2[row_num, col_num].set_xticklabels(phen_names, rotation="vertical")
    for tick, color in zip(axs[row_num, col_num].get_xticklabels(), colors30):
        tick.set_color(color)
    for tick, color in zip(axs2[row_num, col_num].get_xticklabels(), colors30):
        tick.set_color(color)

    fig.tight_layout()
    # fig.subplots_adjust(hspace=0.5)

    filename = hf.rename_file("Evolution_averages_redo.png")
    fig.savefig(filename, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename)
    plt.close(fig)

    fig2.tight_layout()
    # fig.subplots_adjust(hspace=0.5)

    filename = hf.rename_file("Evolution_averages_redo_2.png")
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

    print(evol_data_1.shape)
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


def plot_fig_g(
    fit_data, plot_data_3, gen_index_orig, titles, gen_indices, phen_names, filename1
):
    fig_g = plt.figure(figsize=(12, 12))
    gs = fig_g.add_gridspec(3, 2)

    # 5 plot axes + 1 legend axes
    ax1 = fig_g.add_subplot(gs[0, 0])
    ax2 = fig_g.add_subplot(gs[1, 0])
    ax3 = fig_g.add_subplot(gs[2, 0])
    ax4 = fig_g.add_subplot(gs[0, 1])
    ax5 = fig_g.add_subplot(gs[1, 1])
    ax_leg = fig_g.add_subplot(gs[2, 1])

    plot_axes = [ax2, ax3, ax4, ax5]

    initial_gen = 0
    final_gen = 1000

    gen_index = gen_index_orig
    gen_seg = (gen_index >= initial_gen) & (gen_index < final_gen)

    ax1.plot(gen_index[gen_seg], fit_data[gen_seg, 0], label="Best phenotype")
    ax1.plot(gen_index[gen_seg], fit_data[gen_seg, 1], label="Population fitness")
    ax1.set_title("Log Fitness", fontsize=title_font_size)
    ax1.legend()
    plot_cols_fig_2(plot_axes, plot_data_3, titles, gen_indices, phen_names)
    ax3.set_xlabel("Generation", fontsize=label_font_size)
    ax5.set_xlabel("Generation", fontsize=label_font_size)

    handles, labels = ax2.get_legend_handles_labels()

    ax_leg.axis("off")
    ax_leg.legend(handles, labels, loc="best", ncol=3, frameon=True)

    fig_g.savefig(filename1, bbox_inches="tight", dpi=300)
    # print("Saved plot image to: %s" % filename)
    plt.close()


def plot_hist(a=None):
    file = getFileName("genhistory.dat")
    if file is None:
        return
    evol_data = getAvData(file)
    fit_data = getAvData(getFileName("fitness.dat"))
    fit_data = np.log(fit_data[:, (1, 2)])

    # evol_data_all = hf.load_nonragged_arrays(hf.rename_file("genhistory.dat"))
    # evol_data_1 = evol_data_all[len(evol_data_all) - 1]  # use only the last array

    worm_file = hf.get_worm_file()
    network_json_data = utils.getJsonFile(worm_file)
    vectsize = network_json_data["Evolutionary Optimization Parameters"]["VectSize"][
        "value"
    ]

    if hf.checkDictName(network_json_data, ["Evolvable", "value", 0, "name"]):
        evolvables = network_json_data["Evolvable"]["value"]
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
        print("Evolvable names not found for plot_hist")
        return
    # phen_nums[:] , phen_names[:] = map(list, zip(*sorted(zip(phen_nums, phen_names))))
    # phen names should already be ordered correctly for gene

    phen_offset = vectsize * 2
    phen_size = vectsize

    avlentop = 5
    if hasattr(a, 'evoAvLen'):
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

    print("phen names ", phen_names)

    plot_hist_1(evol_data, phen_size, phen_offset, phen_names, "EvolutionHistory.png")
    plot_hist_1(evol_data, phen_size, phen_size, phen_names, "EvolutionHistoryMax.png")

    plot_fig_g(
        fit_data,
        plot_data_3,
        gen_index_orig,
        titles,
        gen_indices,
        phen_names,
        hf.rename_file("Gridfig.png"),
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
        fit_data,
        plot_data_3_avs,
        gen_index_orig,
        titles,
        gen_indices,
        phen_names_set,
        hf.rename_file("Gridfig_av.png"),
    )

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
        fig, axs = plt.subplots(plot_rows, plot_cols, figsize=(10, 5), squeeze=False)

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
    if (network_json_data is not None) and (
        "Main model name" in network_json_data["Worm"]
    ):
        main_model_name = network_json_data["Worm"]["Main model name"]["value"]

    if a.modelName == "W2DSR":
        json_model_name = network_json_data["Nervous system"]["Model name"]["value"]
        a.modelName = json_model_name

    if (main_model_name is not None) and (main_model_name == "COW2DSR"):
        plot_format = utils.getPlotFormat(network_json_data)
    else:
        plot_format = utils.plot_formats[a.modelName]

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
        CO18_size = network_json_data["Nervous system"]["size"]["value"]
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
                label=label + " %i" % (i - offset),
                linewidth=0.5,
            )
            # axs[plot_num, 0].xaxis.set_ticklabels([])
        # plt.legend()

        data_list = act_data[data_offset : data_size + data_offset, data_seg]
        dy = 1
        dx = t_data[1] - t_data[0]
        # axs[plot_num, 1].set_title("Body curvature", fontsize=title_font_size)
        axs[plot_num, 1].imshow(
            data_list,
            aspect="auto",
            interpolation="nearest",
            extent=[
                0,
                data_list.shape[1] * dx,
                0,
                data_list.shape[0] * dy,
            ],
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

        dy = 1
        dx = t_data[1] - t_data[0]
        axs[count_num, 1].set_title("Body curvature", fontsize=title_font_size)
        axs[count_num, 1].imshow(
            curv_data_less_time,
            aspect="auto",
            interpolation="nearest",
            extent=[
                0,
                curv_data_less_time.shape[1] * dx,
                0,
                curv_data_less_time.shape[0] * dy,
            ],
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

        for t in range(1, tmax, int(tmax / num)):
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
