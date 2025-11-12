import helper_funcs as hf
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
    for dir in path_list:

        body_data = np.loadtxt(dir + "/body.dat").T
        bearing_mid, trajectory_diff =  hf.make_orients(body_data, t_start_off = 1000, t_end_off = 1000)
        bearing_mid_all.extend(bearing_mid[:-1])
        trajectory_diff_all.extend(trajectory_diff)
    
    mark_size = 1
    fig_orient, ax_orient = plt.subplots(2, 1, figsize=(5, 15))
    ax_orient[0].scatter(bearing_mid_all, trajectory_diff_all, s=mark_size)

    heatmap, xedges, yedges = np.histogram2d(bearing_mid_all, trajectory_diff_all*10.0, bins=50)
    extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
    ax_orient[1].imshow(heatmap.T, extent=extent, origin='lower')
    fig_orient.tight_layout()
    filename = dir_names[0] + "/Orient.png"
    #fig_orient.show()
    fig_orient.savefig(filename, bbox_inches="tight", dpi=300)