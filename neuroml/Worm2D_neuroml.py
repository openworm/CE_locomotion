
from pyneuroml import pynml
import argparse
import re
import os
import sys
import time
import math
import utils

def run_main(args=None):
    if args is None:
        args = utils.process_args()
    run(a=args)

def run(a=None, **kwargs):
    a = utils.build_namespace(utils.DEFAULTS, a, **kwargs)
    
    
    current_time = time.strftime("%Y-%m-%d_%H-%M-%S", time.gmtime())
     
    sim_ref = "%s_%s_%s" % ('C1', 'Worm2D', current_time)
    out_dir = 'simulations'
    sim_dir = os.path.join(out_dir, sim_ref)
    #os.mkdir(sim_dir)
    sim_dir = out_dir

    lems_file = os.path.join(sim_dir, 'LEMS_Worm2D.xml')

    pynml.run_lems_with_jneuroml_neuron(lems_file,
                                        only_generate_scripts=True,
                                        nogui=True,
                                        load_saved_data=False,
                                        verbose=True,
                                        realtime_output=True)


    main_nrn_py = open('%s/LEMS_Worm2D_nrn.py'%(sim_dir),'r')
    updated =''
    for line in main_nrn_py:
        line = line.replace('GenericCell.hoc','%s/GenericCell.hoc' % sim_dir)
        line = line.replace('GenericNeuronCell.hoc','%s/GenericNeuronCell.hoc' % sim_dir)
        line = line.replace('GenericMuscleCell.hoc','%s/GenericMuscleCell.hoc' % sim_dir)
        line = line.replace("open('time.dat","open('%s/time.dat" % sim_dir)
        line = line.replace("open('c302_","open('%s/c302_" % sim_dir)
        updated += line
    main_nrn_py.close()

    main_nrn_py = open('%s/LEMS_Worm2D_nrn.py'%(sim_dir),'w')
    main_nrn_py.write(updated)
    main_nrn_py.close()

    command = 'nrnivmodl %s' % sim_dir
    #command = 'nrnivmodl .'
    run_dir = '.'
    try:
        pynml.execute_command_in_dir_with_realtime_output(command, run_dir, prefix="nrnivmodl >> ")
    except KeyboardInterrupt:
        #print_("\nCaught CTRL+C\n")
        sys.exit()

if __name__ == '__main__':
    run_main()
