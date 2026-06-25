import matplotlib.pylab as plt
import matplotlib.gridspec as gridspec
import numpy as np
from . import helper_funcs as hf
import matplotlib as mpl
import os

from .neuromlLocal import utils


def make_fig(model_name):
    plot_format = utils.plot_formats[model_name]

    file_prefix = "sim_"
    if not os.path.isfile(hf.rename_file(file_prefix + "ns.dat")):
        file_prefix = ""
        if not os.path.isfile(hf.rename_file(file_prefix + "ns.dat")):
            return

    nrods = 51
    mpl.rcParams["xtick.labelsize"] = 24
    mpl.rcParams["ytick.labelsize"] = 24
    # sel = [4, 5, 9, 21, 23, 30, 37, 42, 45, 53, 63, 66, 68, 101, 103]
    #################     PLOT   ################
    ############## neural activity  ################
    plt.close("all")
    fig = plt.figure(figsize=[34, 12])
    gm = gridspec.GridSpec(68, 150)
    ax0 = [
        plt.subplot(
            gm[
                5 + (i % 3) * 20 : 20 + (i % 3) * 20,
                65 + (i % 2) * 43 : 105 + (i % 2) * 43,
            ]
        )
        for i in [0, 4, 2, 3, 1, 5]
    ]  # body posture
    ax1 = plt.subplot(gm[4:17, :58])  # curvature
    ax2 = plt.subplot(gm[19:32, :58])  # velocity
    ax3 = plt.subplot(gm[34:47, :58])  # curvature
    ax4 = plt.subplot(gm[49:62, :58])  # velocity
    ################################################
    ################################################
    ###################### CURVATURE  ################

    body = np.loadtxt(
        hf.rename_file(file_prefix + "body.dat")
    )  ## first 50 seconds of simulation
    curv = np.loadtxt(hf.rename_file(file_prefix + "curv.dat"))
    act_data = np.loadtxt(hf.rename_file(file_prefix + "ns.dat")).T

    worm_file = hf.get_worm_file()
    if False:
        worm_file = hf.rename_file("worm_data_evo.json")
        if not os.path.isfile(worm_file):
            worm_file = hf.rename_file("worm_data.json")
        if not os.path.isfile(worm_file):
            worm_file = hf.rename_file("worm_data_worm.json")

    network_json_data = utils.getJsonFile(worm_file)
    # pop_names = utils.getPopNames(network_json_data)
    pop_plot_names = plot_format["plot_cell_names"]
    plot_cell_unit = 0
    if "plot_cell_unit" in plot_format:
        plot_cell_unit = plot_format["plot_cell_unit"]
    plot_col_divs = plot_format["plot_col_divs"]
    # cell_names = utils.getCellNames(network_json_data)
    cell_names = utils.default_cells[model_name]["names"]

    if hf.checkDictName(network_json_data, ["Simulation"]):
        step_size = network_json_data["Simulation"]["StepSize"]["value"]
        skip_steps = network_json_data["Simulation"]["skip_steps"]["value"]
    else:
        step_size = network_json_data["Evolutionary Optimization Parameters"][
            "StepSize"
        ]["value"]
        skip_steps = network_json_data["Evolutionary Optimization Parameters"][
            "skip_steps"
        ]["value"]

    plot_transient = act_data[0, 0]
    plot_end = min(act_data[0, -1], body[-1, 0], curv[-1, 0])
    plot_time = min(10.0, max(0.0, plot_end - plot_transient))

    if "Evolutionary Optimization Parameters" in network_json_data:
        evo_pars = network_json_data["Evolutionary Optimization Parameters"]
        avg_speed_key = "avg_speed" if "avg_speed" in evo_pars else "AvgSpeed"
        AvgSpeed = evo_pars[avg_speed_key]["value"] * 1000.0
    elif "AvgSpeed" in plot_format:
        AvgSpeed = plot_format["AvgSpeed"]

    # plot_velocity = True
    # if plot_velocity:
    #    vel = np.loadtxt(hf.rename_file("sim_vel.dat")).T
    x = body[:, range(1, 154, 3)]
    y = body[:, range(2, 154, 3)]
    xc = np.sum(x, axis=1) / nrods
    yc = np.sum(y, axis=1) / nrods
    a = body[:, range(3, 154, 3)]

    ################################################
    ############ Trayectory #######
    ###############################
    def plot_worm(ax, k, snapshot_time):
        dx, dy = x[k] - xc[k], y[k] - yc[k]
        dx = dx  # / 100.0
        dy = dy  # / 100.0
        #    b = np.arctan((dx[0]-dx[-1])/(dy[0]-dy[-1]))
        b = 3 * np.pi / 2 + np.arctan((dx[0] - dx[-1]) / (dy[0] - dy[-1]))
        tx = np.cos(b) * dx - np.sin(b) * dy
        ty = np.cos(b) * dy + np.sin(b) * dx
        ta = a[k] + b
        ax.plot(tx, ty, "k", lw=1)
        ax.plot(tx[0], ty[0], ".g", ms=18)
        ax.plot(tx - r / 2.0 * np.cos(ta), ty - r / 2.0 * np.sin(ta), "k", lw=1)
        ax.plot(tx + r / 2.0 * np.cos(ta), ty + r / 2.0 * np.sin(ta), "k", lw=1)
        ax.set_xlim(-0.0005, 0.0005)
        ax.set_ylim(-0.00015, 0.00015)
        # ax.set_xlim(-0.05, 0.05)
        # ax.set_ylim(-0.015, 0.015)
        ax.text(
            -0.0004,
            0.0001,
            "t = %.2f" % snapshot_time,
            ha="center",
            va="center",
            fontsize=26,
        )
        ax.set_xticks([])
        ax.set_yticks([])
        ax.set_aspect("equal")

    r = (
        2 * 40 * 10**-6 * np.abs(np.sin(np.arccos((np.arange(51) - 25) / (25 + 0.2))))
    )  # body radius, from BBC model
    dt = step_size * skip_steps
    available_snapshot_time = max(0.0, (len(body) - 1) * dt)
    snapshot_times = np.linspace(0.0, min(2.0, available_snapshot_time), 6)
    for k, t in enumerate(snapshot_times):
        plot_worm(ax0[k], min(int(round(t / dt)), len(body) - 1), t)

    ################################################
    fzl = 26
    ############ Curvature  ######
    ###############################
    curv_times = curv[:, 0]
    low_lim = np.searchsorted(curv_times, plot_transient, side="left")
    hi_lim = np.searchsorted(curv_times, plot_transient + plot_time, side="right")
    hi_lim = min(max(low_lim + 1, hi_lim), len(curv_times))
    # print("lsls ", plot_transient, plot_time, low_lim, hi_lim)
    # sys.exit(0)
    imcurv = ax1.imshow(
        # curv.T[1:, :],
        curv.T[1:, low_lim:hi_lim],
        cmap=plt.get_cmap("seismic"),
        aspect="auto",
        vmin=-10,
        vmax=10,
        origin="lower",
        extent=[curv_times[low_lim], curv_times[hi_lim - 1], 0, curv.shape[1] - 1],
    )
    # ax1.set_xlim(low_lim, hi_lim + low_lim)
    # ax1.set_xlim(int(plot_transient / (step_size * skip_steps)),
    #             int((plot_transient + plot_time) / (step_size * skip_steps)))
    # ax1.set_xticks([0, 40, 80, 120])
    # ax1.set_xticks(np.linspace(low_lim, hi_lim + low_lim, 4))
    # ax1.set_xticks(np.linspace(int(plot_transient / (step_size * skip_steps)),
    #                           int((plot_transient + plot_time) / (step_size * skip_steps)), 4))
    ax1.set_xticklabels([])
    ax1.set_yticks([1, 21])
    ax1.set_yticklabels(["", ""])
    ax1.text(-1, 1, "Head", va="center", ha="right", fontsize=fzl)
    ax1.text(-1, 21, "Tail", va="center", ha="right", fontsize=fzl)
    plt.colorbar(imcurv, location="top", shrink=0.4)

    ############ Velocity #######
    ###############################
    # s = np.where(np.array(sel) == 23)[0]

    plot_velocity = True

    if plot_velocity:
        vel = np.loadtxt(hf.rename_file(file_prefix + "vel.dat")).T
        # ax2.plot(np.linspace(0, 10, len(vel[s][0])), 1000*vel[s][0], 'k', linewidth = 3)
        ax2.plot(vel[0][1:], 1000 * vel[1][1:], "k", linewidth=3)
        ax2.axhline(y=AvgSpeed, linestyle="--", color="r")
        ax2.set_ylim(AvgSpeed * 0.5, AvgSpeed * 1.5)
        ax2.set_xticklabels([])
        ax2.set_xlim(plot_transient, plot_transient + plot_time)
        # ax2.set_xlim(0, plot_time)
        # ax2.set_yticks([0.1, 0.2, 0.3])
        ax2.set_yticks(np.linspace(AvgSpeed * 0.5, AvgSpeed * 1.5, 3))
        ax2.set_ylabel("Velocity (mm/s)", fontsize=fzl, labelpad=24)
        # ax2.set_xlabel('Time (s)', fontsize = fzl, labelpad = 22)
    ###############################

    fz = 26
    cols = ["k", "r", "b", "g", "c", "m", "y", "tab:orange", "tab:brown", "tab:gray"]
    # cell_list = ["AS", "DA", "DB", "DD"]

    text_space = 0.03
    for ind, (cell, col) in enumerate(
        zip(pop_plot_names[: int(plot_col_divs[0])], cols)
    ):
        ind1 = utils.getIndOfNthVal(cell, cell_names, plot_cell_unit)
        if ind1 is None:
            print(cell_names)
            print(cell)
            print(plot_cell_unit)
            print("Index error")
            exit()
        # print("cell ind is ", ind1)
        # ind1 = cell_names.index(cell)
        ax3.plot(act_data[0], act_data[1 + ind1], col, linewidth=3)
        ax3.set_xlim(plot_transient, plot_transient + plot_time)
        # ax3.set_xlim(0, plot_time)
        ax3.set_ylim(-0.1, 1.1)
        ax3.set_xticklabels([])
        # ax3.set_ylabel('Activity', fontsize = fzl, labelpad = 24)
        # ax3.set_xlabel('Time (s)', fontsize = fzl, labelpad = 22)
        plt.figtext(
            0.043,
            0.47 - ind * text_space,
            cell,
            fontsize=fz,
            ha="center",
            va="center",
            color=col,
        )

    # cols = ["red", "blue", "green"]
    # cell_list = ["VA", "VB", "VD"]
    for ind, (cell, col) in enumerate(
        zip(
            pop_plot_names[int(plot_col_divs[0]) :],
            cols[int(plot_col_divs[0] - plot_col_divs[1]) :],
        )
    ):
        # ind1 = cell_names.index(cell)
        ind1 = utils.getIndOfNthVal(cell, cell_names, plot_cell_unit)
        if ind1 is None:
            print("Index error")
            exit()
        # print("cell ind is ", ind1)
        # ax4.plot(act_data[0] - plot_transient, act_data[1 + ind1], col, linewidth=3)
        ax4.plot(act_data[0], act_data[1 + ind1], col, linewidth=3)
        ax4.set_xlim(plot_transient, plot_transient + plot_time)
        ax4.set_ylim(-0.1, 1.1)
        # ax4.set_xticks(np.linspace(int(plot_transient / (step_size * skip_steps)),
        #                       int((plot_transient + plot_time) / (step_size * skip_steps)), 4))
        # ax4.set_ylabel('Activity', fontsize = fzl, labelpad = 24)
        ax4.set_xlabel("Time (s)", fontsize=fzl, labelpad=22)
        plt.figtext(
            0.043,
            0.25 - ind * text_space,
            cell,
            fontsize=fz,
            ha="center",
            va="center",
            color=col,
        )

    fz = 36
    # plt.figtext(0.047, 0.45, 'AS', fontsize = fz, ha = 'center', va = 'center', color = )
    # plt.figtext(0.047, 0.4, 'DA', fontsize = fz, ha = 'center', va = 'center')
    # plt.figtext(0.047, 0.35, 'DB', fontsize = fz, ha = 'center', va = 'center')
    # plt.figtext(0.047, 0.3, 'DD', fontsize = fz, ha = 'center', va = 'center')
    plt.figtext(0.018, 0.95, "A", fontsize=fz, ha="center", va="center")
    plt.figtext(0.018, 0.46, "B", fontsize=fz, ha="center", va="center")
    plt.figtext(0.460, 0.95, "C", fontsize=fz, ha="center", va="center")
    ###############################
    fig.subplots_adjust(left=0.08, bottom=0.02, right=1.0, top=0.98)
    plt.savefig(hf.rename_file("behavior.png"), dpi=100)
    plt.savefig(hf.rename_file("behavior.pdf"))
    plt.close()
