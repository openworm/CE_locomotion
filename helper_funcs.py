import numpy as np
from matplotlib import pyplot as plt
import argparse
import os
import math
from scipy.stats import binned_statistic
import sys
from functools import partial

dir_name = None
file_prefix = None

title_font_size = 16
label_font_size = 14


DEFAULTS = {"modelName": None, "showPlot": True, "folderName": None, "verbose": False}


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
    window= np.ones(int(window_size))/float(window_size)
    return np.convolve(interval, window, 'same')


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





def plot_orients(body_data, plot_list = 
                  ["body orientation", "direction to peak", 
                      "distance to peak", "bearing from peak direction"]
                 ):

    num_cols = 2
    num_rows = math.ceil(len(plot_list)/num_cols)
    fig_orient, ax_orient = plt.subplots(num_rows, num_cols, figsize=(num_cols*4, num_rows*4))

    tmax = body_data.shape[1]
    trange = body_data[0,:]
    t_offset = 0
    t_start = t_offset
    t_end = tmax - t_offset
    trange_inds = (trange >= t_start) & (trange < t_end) 
    trange  = trange[trange_inds]
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
    
    plottables = { "bearing from peak direction" :  bearing_mid,
                   "distance to peak" : distToOrigin,
                   "orientation variation" : dOrientation,
                   "body orientation" : orientation,
                    "direction to peak" : dir_to_origin,
                    "head trajectory variation" : trajectory_diff_u,
                    "head trajectory" : trajectory }
    
    for key, val in plottables.items():
        newval = {}
        newval["value"] = val
        if key == "distance to peak":
            newval["y_label"] = "distance (cm)"
        else:
            newval["y_label"] = "angle (rad)"
        plottables[key] = newval 

    print(plottables)
    sys.exit

    mark_size = 0.2
    tav_window = 1
    plot_func = partial(movingaverage, window_size = tav_window)

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
        print(val, t_start_ind, t_end_ind, r_diff)
        ax_orient[row_num, col_num].plot(plot_func(trange[t_start_ind:t_end_ind]), 
                                         plot_func(plottables[val]["value"]), 
        'o', markersize = mark_size)
        ax_orient[row_num, col_num].set_title(val, fontsize=title_font_size)
        ax_orient[row_num, col_num].set_ylabel(plottables[val]["y_label"], fontsize=label_font_size)
        if row_num == num_rows - 1:
            ax_orient[row_num, col_num].set_xlabel("Time (s)", fontsize=label_font_size)



    if False:
        trange_av = movingaverage(trange, tav_window)
        ax_orient[0, 0].plot(trange, orientation, 'o', markersize = mark_size)  # body orientation
        ax_orient[0, 0].set_title("body orientation", fontsize=title_font_size)
        ax_orient[1, 0].plot(trange, distToOrigin, 'o', markersize = mark_size)
        ax_orient[1, 0].set_title("distance to peak", fontsize=title_font_size)
        ax_orient[2, 0].plot(trange, dir_to_origin, 'o', markersize = mark_size)
        ax_orient[2, 0].set_title("direction to origin", fontsize=title_font_size)
        ax_orient[3, 0].plot(movingaverage(trange[:-1], tav_window), 
                            movingaverage(dOrientation,tav_window),  'o', markersize = mark_size)
        ax_orient[3, 0].set_title("orientation variation", fontsize=title_font_size)
        ax_orient[4, 0].plot(movingaverage(trange[1:-1], tav_window), 
                            movingaverage(trajectory_diff_u, tav_window), 'o', markersize = mark_size)
        ax_orient[4, 0].set_title("head trajectory variation", fontsize=title_font_size)


        ax_orient[0, 1].plot(movingaverage(trange[:-1], tav_window), 
                            movingaverage(bearing_mid, tav_window), 'o', markersize = mark_size)
        ax_orient[0, 1].set_title("bearing from origin direction", fontsize=title_font_size)
        ax_orient[1, 1].plot(movingaverage(trange[:-1], tav_window), 
                            movingaverage(trajectory, tav_window), 'o', markersize = mark_size)
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
    #fig_orient.close()


def angle_diff(a, b):
    """Return the signed smallest difference between two angles (in radians)."""
    d = (a - b + math.pi) % (2 * math.pi) - math.pi
    # Optional: map -pi to +pi for symmetry
    # if d == -math.pi:
    #    return math.pi
    return d


def plotHist(ax, x, y):
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
