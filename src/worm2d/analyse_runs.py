from . import helper_funcs as hf
import numpy as np
from matplotlib import pyplot as plt


def plot_orients(a=None, **kwargs):
    a = hf.build_namespace(hf.DEFAULTS, a, **kwargs)

    if a.folderName is None:
        print("Folder name is required for data.")
        return

    bearing_mid_all = []
    trajectory_diff_all = []
    dir_name_1 = a.folderName
    dir_names = [dir_name_1]
    path_list = hf.get_path_list(dir_names)
    print(path_list)
    # sys.exit(1)

    fig_body, ax_body = plt.subplots(figsize=(5, 5))

    for dir in path_list:
        body_data = np.loadtxt(dir + "/body.dat").T
        bearing_mid, trajectory_diff = hf.make_orients(
            body_data, t_start_off=1000, t_end_off=1000
        )
        bearing_mid_all.extend(bearing_mid[:-1])
        trajectory_diff_all.extend(trajectory_diff)

        hf.plot_path(body_data, ax_body)

    trajectory_diff_all = np.array(trajectory_diff_all)
    bearing_mid_all = np.array(bearing_mid_all)
    mark_size = 1
    fig_orient, ax_orient = plt.subplots(3, figsize=(5, 10))
    ax_orient[0].scatter(bearing_mid_all, trajectory_diff_all, s=mark_size)

    heatmap, xedges, yedges = np.histogram2d(
        bearing_mid_all, trajectory_diff_all * 10, bins=50
    )
    extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
    ax_orient[1].imshow(heatmap.T, extent=extent, origin="lower")

    hf.plotHist(ax_orient[2], bearing_mid_all, trajectory_diff_all)

    fig_orient.tight_layout()
    filename = dir_names[0] + "/Orient.png"
    # fig_orient.show()
    fig_orient.savefig(filename, bbox_inches="tight", dpi=300)
    # fig_orient.close()

    fig_body.tight_layout()
    filename = dir_names[0] + "/Motion.png"
    fig_body.savefig(filename, bbox_inches="tight", dpi=300)
    plt.close()


if __name__ == "__main__":
    import sys

    plot_orients(folderName=sys.argv[1])
