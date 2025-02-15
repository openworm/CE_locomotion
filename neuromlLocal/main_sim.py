


def print_(msg):
    pre = "Python >> "
    print("%s %s" % (pre, msg.replace("\n", "\n" + pre)))


def announce(message):
    print_(
        "\n************************************************************************\n*"
    )
    print_("*  %s" % message.replace("\n", "\n*  "))
    print_(
        "*\n************************************************************************"
    )





class Worm2DNRNSimulation:
    max_ca = 4e-7
    max_ca_found = -1

    def __init__(self):
        self.tstop = 1
        print("Worm2DNRNSimulation init called")
        return

    def set_timestep(self, dt):
        print("Setting timestep to %s..." % dt)

        dt = float("{:0.1e}".format(dt)) * 1000.0  # memory issue fix

        import sys
        import sysconfig
        import os

        

        # sys.path.insert(0,sysconfig.get_paths()["purelib"])
        sys.path.append(sysconfig.get_paths()["purelib"])
        current = os.path.dirname(os.path.realpath(__file__))
        #print(current)
        #sys.exit()
        sim_dir = current
        #sim_dir = "neuromlLocal"
        #sys.path.append(current)
        # sys.path.append(current + '/' + sim_dir)
        print("sys path is : ", sys.path)
        # sys.exit()
        # run_dir = "./"
        # sim_dir = "simulations/C1_Muscles_2025-02-12_12-13-48"
        # sim_dir = "neuromlLocal"
        #sim_dir = "./"
        # run_dir = sim_dir
        #run_dir = "./"
        run_dir = sim_dir
        command = "nrnivmodl %s" % sim_dir
        # command = 'nrnivmodl .'
        announce("Compiling NMODL files for NEURON...")

        try:
            from pyneuroml import pynml

            pynml.execute_command_in_dir_with_realtime_output(
                command, run_dir, prefix="nrnivmodl >> "
            )
        except KeyboardInterrupt:
            print_("\nCaught CTRL+C\n")
            sys.exit()

        try:
            # import sys
            # import sysconfig;
            # sys.path.append(sysconfig.get_paths()["purelib"])
            # print('sys path is : ', sys.path)

            # sys.exit()
            # from simulations.LEMS_Worm2D_nrn import NeuronSimulation
            # from simulations.LEMS_c302_nrn import NeuronSimulation
            import importlib

            # nsp = importlib.import_module("simulations.C1_Muscles_2025-02-12_12-13-48.LEMS_c302_nrn")

            nsp = importlib.import_module("LEMS_Worm2D_nrn")

            # nsp = importlib.import_module("simulations.C1_Muscles_2025-02-04_14-04-08.LEMS_c302_nrn")
            # nsp = importlib.import_module("LEMS_c302_nrn")
            # from LEMS_c302_nrn import NeuronSimulation
            # from NeuronSimulation_package import NeuronSimulation
            # from simulations.C1_Muscles_2025-02-04_14-04-08.LEMS_c302_nrn import NeuronSimulation

            import neuron

            self.h = neuron.h
        except Exception as e:
            raise Exception("Python import error in Worm2DNRNSimulation: %s.." % e)

        # sys.exit()
        # self.ns = NeuronSimulation(self.tstop, dt)
        self.ns = nsp.NeuronSimulation(self.tstop, dt)
        self.ns.sim_time = 10
        
        #self.h.OneStim_0.weight = 0.0
        getattr(self.h, 'OneStim_0').weight = 0.0
        

        print_(
            "Initialised Worm2DNRNSimulation of length %s ms and dt = %s ms..."
            % (self.tstop, dt)
        )
    
    def set_neuron_input(self, i, weight):
        stim_pop_list = ['DA', 'DB', 'DD', 'VD', 'VA', 'VB']
        neurons_per_unit = 10
        stim_pop_num = int(i / neurons_per_unit)
        neuron_number = i % 10
        if stim_pop_num < len(stim_pop_list):    
           getattr(self.h, 'ExtStimPop' + stim_pop_list[stim_pop_num] + '_' + str(neuron_number)).weight = weight  
           return
        print('neuron number too large')
        import sys
        sys.exit()

    def set_input_weights(self, weight):

        stim_pop_list = ['DA', 'DB', 'DD', 'VD', 'VA', 'VB']
        for stim_pop in stim_pop_list:
            for i in range(10):
                getattr(self.h, 'ExtStimPop' + stim_pop + '_' + str(i)).weight = weight
        

    def run(self, skip_to_time=-1):
        print_("> Current NEURON time: %s ms" % self.h.t)

        self.ns.advance()

        print_("< Current NEURON time: %s ms" % self.h.t)
       
        # values = []
        pop_list = ['m_DA_PopDA', 'm_DB_PopDB', 'm_DD_PopDD', 'm_VD_PopVD', 'm_VA_PopVA', 'm_VB_PopVB']

        values = []
        vars_read = []
        for pop in pop_list:
            for i in range(10):
                #var = 'm_DB_PopDB'
                #var = "a_MDR%s" % (i + 1 if i > 8 else ("0%i" % (i + 1)))
                try:
                    #val = getattr(self.h, var)[0].soma.cai
                    val = getattr(self.h, pop)[i].output
                except AttributeError as e:
                    print(
                        "Problem passing neuronal output of %s to muscle in Sibernetic: %s"
                        % (pop, e)
                    )
                    continue
                    #val = 0
                #scaled_val = self._scale(val)
                values.append(val)
                vars_read.append(pop + '_' + str(i))

        #values = [6, 7, 8]
        #print(values)
        return values

    def save_results(self):
        print_("> Saving results at time: %s" % self.h.t)

        self.ns.save_results()

    def _scale(self, ca, print_it=False):
        self.max_ca_found = max(ca, self.max_ca_found)
        scaled = min(1, (ca / self.max_ca))
        if print_it:
            print_(
                "- Scaling %s to %s (max found: %s)" % (ca, scaled, self.max_ca_found)
            )
        return scaled

def listToStr(list_val):
    out_str = ''
    for val in list_val:
        out_str = out_str + ' ' + str(val)
    return out_str  

if __name__ == "__main__":
    w = Worm2DNRNSimulation()
    w.set_timestep(0.005)
    w.set_input_weights(0.0)
    #w.ns.sim_time = 10
    #out_vals = []
    #time_vals = []
    fout = open('Worm2D.outputs-test.dat', 'w')
    for i in range(0,10000):
        out_str = str(i) + listToStr(w.run())
        if i==5000:
           #w.set_input_weights(1.0)
           w.set_neuron_input(53, 1.0)
        fout.write(out_str)
        fout.write('\n')
    #import numpy as np
    #out_arr = np.array(out_vals)

    fout.close()    
    w.save_results()