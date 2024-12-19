#####
# Script to load generated data on worm motion/cell activity & generate graphical output
#####


import numpy as np
from matplotlib import pyplot as plt
import random
import helper_funcs as hf

def reload_single_run(show_plot=True):
        
    N_muscles = 24;               # Number of muscles alongside the body
    N_units = 10;                 # Number of neural units in VNC
    N_neuronsperunit = 6;         # Number of neurons in a VNC neural unit (6 neurons)
    N_stretchrec = 10;            # Number of stretch receptors

    N_neurons = N_neuronsperunit*N_units

    fig, axs = plt.subplots(4, 2, figsize=(14,8))

    title_font_size = 10

    ###  Worm neuron/muscle activation

    act_data = np.loadtxt(hf.rename_file('act.dat')).T

    offset=1

    sr = act_data[offset:N_stretchrec+offset]
    axs[0, 0].set_title('Stretch receptors', fontsize=title_font_size)
    axs[0, 1].set_title('Stretch receptors', fontsize=title_font_size)

    for i in range(offset,N_stretchrec+offset):
        axs[0, 0].plot(act_data[0], act_data[i],label='SR %i'%(i-offset), linewidth=0.5)
        axs[0, 0].xaxis.set_ticklabels([])
    plt.legend()

    axs[0, 1].imshow(sr, aspect='auto', interpolation='nearest')
    axs[0, 1].xaxis.set_ticklabels([])


    offset+=N_stretchrec
    axs[1, 0].set_title('Neurons', fontsize=title_font_size)
    axs[1, 1].set_title('Neurons', fontsize=title_font_size)
    for i in range(offset,N_neurons+offset):
        axs[1, 0].plot(act_data[0], act_data[i],label='Neu %i'%(i-offset), linewidth=0.5)
        axs[1, 0].xaxis.set_ticklabels([])
    plt.legend()

    neu = act_data[offset:N_neurons+offset]
    axs[1, 1].imshow(neu, aspect='auto', interpolation='nearest')
    axs[1, 1].xaxis.set_ticklabels([])

    offset+=N_neurons
    axs[2, 0].set_title('Muscles', fontsize=title_font_size)
    axs[2, 1].set_title('Muscles', fontsize=title_font_size)
    for i in range(offset,N_muscles+offset):
        axs[2, 0].plot(act_data[0], act_data[i],label='Mu %i'%(i-offset), linewidth=0.5)
        axs[2, 0].xaxis.set_ticklabels([])
    plt.legend()

    mus = act_data[offset:N_muscles+offset]
    axs[2, 1].imshow(mus, aspect='auto', interpolation='nearest')
    axs[2, 1].xaxis.set_ticklabels([])


    ###  Worm body curvature

    curv_data = np.loadtxt(hf.rename_file('curv.dat'))
    curv_data_less_time = curv_data.T[1:,:]

    axs[3, 1].set_title('Body curvature', fontsize=title_font_size)
    axs[3, 1].imshow(curv_data_less_time, aspect='auto')

    ###  Body position


    body_data = np.loadtxt(hf.rename_file('body.dat')).T

    tmax = 1520
    if tmax >= body_data.shape[1]:
        tmax = body_data.shape[1]
    num = 60.

    axs[3,0].set_title('2D worm motion', fontsize=title_font_size)

    for t in range(1,tmax,int(tmax/num)):
        f = float(t)/tmax

        color = "#%02x%02x00" % (int(0xFF*(f)),int(0xFF*(1-f)*0.8))
        color2 = "#%06x" % random.randint(0, 0xFFFFFF)

        point_start = 1
        for i in range(point_start,50):
            x= body_data[i*3+1][t]
            y=body_data[i*3+2][t]
            y1=body_data[i*3+2][t]
            print("%s + Plotting %i at t=%s (%s,%s), %s"%('\n' if i==point_start else '', i, t,x,y, color))

            axs[3, 0].plot([x],[y],'.',color=color,markersize=3 if t==1 else 0.4)

            #print("%s - Plotting %i at t=%s (%s,%s), %s"%('\n' if i==point_start else '', i, t,x,y1, color))
            #plt.plot([x],[y1],'.',color=color)

    axs[3, 0].set_aspect('equal')

    filename = hf.rename_file("ExampleActivity.png")
    plt.savefig(filename, bbox_inches="tight", dpi=300)
    print('Saved plot image to: %s'%filename)

    if show_plot:
        plt.show()


if __name__ == "__main__": 

    import sys
    reload_single_run(show_plot='-nogui' not in sys.argv)
