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
import copy

# import neuromlLocal.utils as utils
import matplotlib as mpl


sys.path.append("..")

# import random
import helper_funcs as hf

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
plot_formats["Net21"]["plot_cell_names"] = ["AS", "DA", "DB", "DD", "VA", "VB", "VD"]
plot_formats["Net21"]["plot_col_divs"] = [4, 3]
plot_formats["Net21"]["plot_time"] = 10
plot_formats["Net21"]["worm_plot_time"] = 2
plot_formats["Net21"]["do_body_plot"] = True
plot_formats["Net21"]["do_curv_plot"] = True


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
plot_formats["W2Dosc21"] = copy.deepcopy(plot_formats["Net21"])
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
plot_formats["W2DSR"] = plot_formats["W2DCE"]
plot_formats["W2D18"] = plot_formats["RS18"]
plot_formats["W2DCO"] = plot_formats["CO"]


def run_main(args=None):
    if args is None:
        args = hf.process_args()
    reload_single_run(a=args)


title_font_size = 16
label_font_size = 14


def getRowsCols(plot_num, plot_cols):
    return int(plot_num / plot_cols), plot_num % plot_cols


def sign(val):
    return (val > 0) * 2.0 - 1.0


def plot_evols(a=None, **kwargs):
    a = hf.build_namespace(hf.DEFAULTS, a, **kwargs)

    hf.setFolder(a)

    mpl.rcParams["xtick.labelsize"] = 12
    mpl.rcParams["ytick.labelsize"] = 12

    file = hf.rename_file("genhistory.dat")
    if not os.path.isfile(file):
        hf.file_prefix = None
    file = hf.rename_file("genhistory.dat")
    if not os.path.isfile(file):
        print(
            "doPlotEvol is True, but genhistory.dat file is necessary for evolution plots."
        )
        return

    evol_data_1 = np.loadtxt(hf.rename_file("genhistory.dat"))
    worm_file = hf.rename_file("worm_data_evo.json")
    if not os.path.isfile(worm_file):
        worm_file = hf.rename_file("worm_data_worm.json")
    if not os.path.isfile(worm_file):
        worm_file = hf.rename_file("worm_data.json")
    network_json_data = utils.getJsonFile(worm_file)
    vectsize = network_json_data["Evolutionary Optimization Parameters"]["VectSize"][
        "value"
    ]

    doPhenNames = False
    if "PhenoNames" in network_json_data:
        phen_names = network_json_data["PhenoNames"]["value"]
        phen_nums = network_json_data["PhenoNamesNums"]["value"]
        doPhenNames = True

    if a.modelName == "CO18" or a.modelName == "CO18Full":
        network_json_data_RS18 = utils.getJsonFile(hf.dir_name + "/RS18_worm_data.json")
        phen_names += network_json_data_RS18["PhenoNames"]["value"]
        phen_nums += network_json_data_RS18["PhenoNamesNums"]["value"]

    # print(phen_names)
    # print(phen_nums)

    print(vectsize)

    phen_offset = vectsize * 2
    phen_size = vectsize

    # evol_data_1 = evol_data_orig #[:,1+phen_offset:]
    #

    if evol_data_1.ndim == 1:
        evol_data_1 = evol_data_1[np.newaxis, :]
    avlen = evol_data_1.shape[0] - 1
    if avlen > 5:
        avlen = 5
    if avlen == 0:
        avlen = 1
    print(evol_data_1.shape)
    evol_data = np.zeros((evol_data_1.shape[0] - avlen + 1, evol_data_1.shape[1]))
    for phen in range(evol_data_1.shape[1]):
        evol_data[:, phen] = np.convolve(
            evol_data_1[:, phen], np.ones(avlen) / avlen, mode="valid"
        )

    gen_index_orig = evol_data[
        :, 0
    ]  # generation number, phenotype number (first is gen index)
    evol_data = evol_data[:, 1 + phen_offset :]

    plot_cols = 2

    evol_data_full_diff = (evol_data[-1] - evol_data[0]) / evol_data[0]
    evol_data_full_diff = sign(evol_data_full_diff) * np.log(
        np.abs(evol_data_full_diff)
    )
    evol_data_full_diff2 = evol_data[-1] - evol_data[0]
    evol_data_full_diff2 = sign(evol_data_full_diff2) * np.log(
        np.abs(evol_data_full_diff2)
    )
    # evol_data_full_diff_abs = (evol_data[-1] - evol_data[0]) / np.abs(evol_data[0])
    evol_data_fin = sign(evol_data[-1]) * np.log(np.abs(evol_data[-1]))

    evol_data_fin_actual = evol_data[-1]
    evol_data_init = sign(evol_data[0]) * np.log(np.abs(evol_data[0]))
    evol_data_init_actual = evol_data[0]

    evol_data_list = [
        evol_data_full_diff,
        evol_data_full_diff2,
        evol_data_init,
        evol_data_fin,
        evol_data_init_actual,
        evol_data_fin_actual,
    ]

    evol_data_list_inds = [0, 1, 2, 2, 3, 3]

    for data_val in evol_data_list:
        data_val[np.isnan(data_val)] = 0
        data_val[np.isinf(data_val)] = 0

    evol_data_avs_titles = [
        "Relative variation",
        "Variation",
        "Inital and Final value",
        # "Initial value",
        "Actual Final value",
    ]

    if doPhenNames:
        phen_names_set = sorted(set(phen_names))
        phen_name_list = []
        evol_data_avs = [[] for x in range(len(evol_data_list))]
        for phen_name in phen_names_set:
            phen_name_list.append(phen_name)
            indices = [
                phen_nums[ind] - 1
                for ind, val in enumerate(phen_names)
                if val == phen_name
            ]
            for av_val, val in zip(evol_data_avs, evol_data_list):
                av_val.append(np.mean(val[indices]))

        # phen_name_list = sorted(phen_name_list_1)
        # evol_data_av = [evol_data_av_1[phen_name_list_1.index(phen_name)] for phen_name in phen_name_list]
        # evol_data_av = evol_data_av_1[sorted_indices]

        print(phen_name_list)
        # print(evol_data_avs[0])

    evol_data_diff = evol_data[1:] - evol_data[0:-1]

    evol_data_diff_sign = sign(evol_data_diff)

    evol_data_diff_2 = (evol_data_diff_sign * np.log(np.abs(evol_data_diff))) - np.log(
        np.abs(evol_data[0:-1])
    )
    evol_data_diff_abs = evol_data_diff
    gen_index_diff = gen_index_orig[1:]

    plot_data = [evol_data, evol_data_diff_2, evol_data_diff_abs]
    # evol_data = evol_data[..., np.newaxis]
    gen_indices = [gen_index_orig, gen_index_diff, gen_index_diff]

    plot_rows = math.ceil((len(plot_data) + 1) / plot_cols)
    if plot_rows > 1:
        fig, axs = plt.subplots(
            plot_rows, plot_cols, figsize=(plot_rows * 4, 10), squeeze=False
        )
    else:
        fig, axs = plt.subplots(plot_rows, plot_cols, figsize=(10, 5), squeeze=False)

    phen_range = range(0, phen_size)
    phen_label = range(1, phen_size + 1)

    titles = ["Gen History", "Gen History diff", "Gen History diff abs"]

    initial_gen = 0
    final_gen = 1000

    plot_num = 0
    for plot_data_1, title, gen_index in zip(plot_data, titles, gen_indices):
        row_num = int(plot_num / plot_cols)
        col_num = plot_num % plot_cols
        axs[row_num, col_num].set_title(title, fontsize=title_font_size)
        for phen in phen_range:
            gen_seg = (gen_index >= initial_gen) & (gen_index < final_gen)
            axs[row_num, col_num].plot(
                gen_index[gen_seg],
                plot_data_1[gen_seg, phen],
                label="%i" % (phen),
                linewidth=0.5,
            )

        axs[row_num, col_num].set_xlabel("Generation", fontsize=label_font_size)
        plot_num += 1

    row_num = int(plot_num / plot_cols)
    col_num = plot_num % plot_cols
    axs[row_num, col_num].set_title("Total variation", fontsize=title_font_size)
    # axs[row_num, col_num].plot(phen_label, evol_data_full_diff)
    axs[row_num, col_num].plot(phen_label, evol_data_fin, label="final")
    axs[row_num, col_num].plot(phen_label, evol_data_init, label="initial")
    axs[row_num, col_num].set_xlabel("Phenotype #", fontsize=label_font_size)
    axs[row_num, col_num].legend()
    fig.tight_layout()

    # fig.subplots_adjust(hspace=0.5)

    filename = hf.rename_file("Evolution.png")
    plt.savefig(filename, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename)

    if doPhenNames:
        plot_cols = 1
        plot_rows = len(evol_data_avs_titles)
        fig, axs = plt.subplots(plot_rows, plot_cols, figsize=(20, 20), squeeze=False)
        # for ind, (val, title) in enumerate(zip(evol_data_avs, evol_data_avs_titles)):
        for ind, val in zip(evol_data_list_inds, evol_data_avs):
            row_num, col_num = getRowsCols(ind, plot_cols)
            axs[row_num, col_num].plot(range(len(val)), val)
            if axs[row_num, col_num].get_title() == "":
                axs[row_num, col_num].set_title(
                    evol_data_avs_titles[ind], fontsize=title_font_size
                )
                # axs[row_num, col_num].set_xlabel("Phenotype #", fontsize=label_font_size)
                # axs[row_num, col_num].xticks(range(len(evol_data_av), phen_name_list))
                axs[row_num, col_num].set_xticks(range(len(val)))
                # axs[row_num, col_num].set_xticklabels(phen_name_list, rotation='vertical')
                axs[row_num, col_num].grid(axis="x")
                axs[row_num, col_num].grid(axis="y")

        axs[row_num, col_num].set_xlabel("Phenotype #", fontsize=label_font_size)
        # axs[row_num, col_num].xticks(range(len(evol_data_av), phen_name_list))
        axs[row_num, col_num].set_xticklabels(phen_name_list, rotation="vertical")

        fig.tight_layout()
        # fig.subplots_adjust(hspace=0.5)

        filename = hf.rename_file("Evolution_averages.png")
        plt.savefig(filename, bbox_inches="tight", dpi=300)
        print("Saved plot image to: %s" % filename)

        if False:
            fig, axs = plt.subplots(1, 1, figsize=(20, 10), squeeze=False)
            axs[0, 0].set_title("Actual final value", fontsize=title_font_size)
            val = evol_data_fin_actual
            axs[0, 0].plot(range(len(val)), val)
            axs[0, 0].set_xticks(range(len(val)))
            axs[0, 0].grid(axis="x")
            axs[0, 0].grid(axis="y")
            axs[0, 0].set_xlabel("Phenotype #", fontsize=label_font_size)
            # axs[row_num, col_num].xticks(range(len(evol_data_av), phen_name_list))
            axs[0, 0].set_xticklabels(phen_name_list, rotation="vertical")
            fig.tight_layout()
            # fig.subplots_adjust(hspace=0.5)

            filename = hf.rename_file("Actual_values.png")
            plt.savefig(filename, bbox_inches="tight", dpi=300)
            print("Saved plot image to: %s" % filename)


# def reload_single_run(show_plot=True, verbose=False, plot_format_name=None):
def reload_single_run(a=None, **kwargs):
    a = hf.build_namespace(hf.DEFAULTS, a, **kwargs)

    hf.setFolder(a)

    plot_format = plot_formats[a.modelName]

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

    if not os.path.isfile(act_file):
        hf.file_prefix = None
    act_data = np.loadtxt(hf.rename_file("act.dat")).T
    t_data = act_data[0]

    if a.modelName == "CO18" or a.modelName == "CO18Full":
        network_json_data = utils.getJsonFile(hf.rename_file("worm_data.json"))
        CO18_size = network_json_data["Nervous system"]["size"]["value"]
        plot_formats[a.modelName]["data_sizes"] = [CO18_size, 2]
        plot_formats[a.modelName]["plot_cell_names"] = [
            "N" + str(i) for i in range(CO18_size)
        ] + ["S" + str(i) for i in range(2)]
        plot_formats[a.modelName]["plot_col_divs"] = [CO18_size, 2]

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
        axs[plot_num, 1].imshow(data_list, aspect="auto", interpolation="nearest")
        axs[plot_num, 1].xaxis.set_ticklabels([])
        axs[plot_num, 1].yaxis.set_major_locator(MaxNLocator(integer=True))

    plot_rows = len(plot_format["fig_titles"])
    if plot_format["do_curv_plot"] or plot_format["do_body_plot"]:
        plot_rows += 1
    if plot_rows > 1:
        fig, axs = plt.subplots(plot_rows, 2, figsize=(plot_rows * 4, 10))
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
        if count_num < len(plot_format["data_sizes"]) - 1:
            axs[count_num, 0].xaxis.set_ticklabels([])
        else:
            axs[count_num, 0].set_xlabel("Time (s)", fontsize=label_font_size)
        count_num += 1
        offset += val[0]

    ###  Worm body curvature
    if plot_format["do_curv_plot"]:
        curv_data = np.loadtxt(hf.rename_file("curv.dat")).T
        t_data = curv_data[0]
        data_seg = (t_data >= t_start) & (t_data < t_end)
        curv_data_less_time = curv_data[1:, data_seg]
        t_data = t_data[data_seg]

        axs[count_num, 1].set_title("Body curvature", fontsize=title_font_size)
        axs[count_num, 1].imshow(curv_data_less_time, aspect="auto")
        axs[count_num, 1].set_xticks(np.linspace(0, len(data_seg), 8))
        # axs[count_num, 1].set_xticks(np.linspace(t_data[0]/t_inc,t_data[-1]/t_inc, 8))
        axs[count_num, 1].xaxis.set_ticklabels(
            np.around(np.linspace(t_data[0], t_data[-1], 8), 2)
        )
        axs[count_num, 1].set_xlabel("Time (s)", fontsize=label_font_size)

        ###  Body position

    if plot_format["do_body_plot"]:
        if a.modelName == "CO" or a.modelName == "W2DCO":
            body_data = np.loadtxt(hf.rename_file("bodypos.dat")).T
        else:
            body_data = np.loadtxt(hf.rename_file("body.dat")).T

        # tmax = 1520
        tmax = body_data.shape[1]
        # if tmax >= body_data.shape[1]:
        #    tmax = body_data.shape[1]
        num = 60.0

        if not (a.modelName == "CO" or a.modelName == "W2DCO"):
            hf.plot_orients(body_data)

        # title = axs[count_num, 0].set_title("2D worm motion", fontsize=title_font_size, loc='right')
        axs[count_num, 0].set_title(
            "2D worm motion",
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

            dd["t"].append(t)

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
                x = body_data[i * 3 + 1][t]
                # xs.append(x * 1000)
                xs.append(x * 10)
                y = body_data[i * 3 + 2][t]
                # ys.append(y * 1000)
                ys.append(y * 10)
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

        fig_body.tight_layout()
        filename = hf.rename_file("Motion.png")
        fig_body.savefig(filename, bbox_inches="tight", dpi=300)
        # fig_body.close()

    fig.tight_layout()
    # fig.subplots_adjust(hspace=0.5)

    filename = hf.rename_file("ExampleActivity.png")
    fig.savefig(filename, bbox_inches="tight", dpi=300)
    print("Saved plot image to: %s" % filename)
    plt.close()

    if a.showPlot:
        print("Showing plot")
        plt.show()
    plt.close()

    from F2_fig_behavior import make_fig

    if not (
        a.modelName == "CO"
        or a.modelName == "W2DCO"
        or a.modelName == "W2Dosc"
        or a.modelName == "W2Dosc21"
        or a.modelName == "CO18Full"
    ):
        make_fig(plot_format=plot_format)


if __name__ == "__main__":
    import sys

    reload_single_run(showPlot=False, modelName="Net21", folderName=sys.argv[1])
