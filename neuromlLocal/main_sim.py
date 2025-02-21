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


def get_pop_number(i):
    stim_pop_list = ["DA", "DB", "DD", "VD", "VA", "VB"]
    pop_num = int(i % 6)
    neuron_num = int(i / 6)
    return stim_pop_list[pop_num], neuron_num


def get_neuron_number(pop, i):
    stim_pop_list = {"DA": 0, "DB": 1, "DD": 2, "VD": 3, "VA": 4, "VB": 5}
    return stim_pop_list[pop] + i * 6


# fout_weights = open('Worm2D.weights-test.dat', 'w')


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
        current = os.path.dirname(os.path.realpath(__file__))  # location of this file!
        sys.path.append(current)
        # print(current)
        # sys.exit()
        sim_dir = current
        # sim_dir = "neuromlLocal"
        # sys.path.append(current)
        # sys.path.append(current + '/' + sim_dir)
        print("sys path is : ", sys.path)
        # sys.exit()
        # run_dir = "./"
        # sim_dir = "simulations/C1_Muscles_2025-02-12_12-13-48"
        # sim_dir = "neuromlLocal"
        # sim_dir = "./"
        # run_dir = sim_dir
        run_dir = "./"
        # run_dir = sim_dir
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

        # self.h.OneStim_0.weight = 0.0
        getattr(self.h, "OneStim_0").weight = 0.0

        print_(
            "Initialised Worm2DNRNSimulation of length %s ms and dt = %s ms..."
            % (self.tstop, dt)
        )

    def set_neuron_input(self, i, weight):
        pop_name, nn = get_pop_number(i)
        getattr(self.h, "ExtStimPop" + pop_name + "_" + str(nn)).weight = weight
        # fout_weights.write(pop_name + ' ' + str(nn) + ' ' + str(weight) + '\n')
        return

    def set_gapJunction_weight(self, pre, post, weight):
        # syn_NC_PopVB_PopVB_gapJunction0_gapJunction0_A[8].weight
        pass

    def set_synaptic_weight(self, pre, post, weight):
        pre_pop, pre_neuron_number = get_pop_number(pre)
        post_pop, post_neuron_number = get_pop_number(post)
        syn_str = "syn_NC_Pop" + pre_pop + "_Pop" + post_pop + "_silentSyn_"
        try:
            getattr(self.h, syn_str + "silentSyn_pre")[
                pre_neuron_number
            ].weight = weight
            getattr(self.h, syn_str + "neuron_to_neuron_syn_x_post")[
                post_neuron_number
            ].weight = weight
        except AttributeError as e:
            print("No such connection: %s " % e)

    def get_synaptic_weight(self, pre, post):
        pre_pop, pre_neuron_number = get_pop_number(pre)
        post_pop, post_neuron_number = get_pop_number(post)
        syn_str = "syn_NC_Pop" + pre_pop + "_Pop" + post_pop + "_silentSyn_"
        try:
            return getattr(self.h, syn_str + "silentSyn_pre")[pre_neuron_number].weight
            # getattr(self.h, syn_str + 'neuron_to_neuron_syn_x_post')[post_neuron_number].weight = weight
        except AttributeError as e:
            print("No such connection: %s " % e)

        # syn_NC_PopDD_PopDA_silentSyn_silentSyn_pre[4].weight
        # syn_NC_PopDD_PopDA_silentSyn_neuron_to_neuron_syn_x_post[4].weight

    def set_neuron_inputs(self, weight):
        stim_pop_list = ["DA", "DB", "DD", "VD", "VA", "VB"]
        for stim_pop in stim_pop_list:
            for i in range(10):
                getattr(self.h, "ExtStimPop" + stim_pop + "_" + str(i)).weight = weight

    def get_neuron_parameter(self, parameter, i):
        pop_name, nn = get_pop_number(i)
        pop_full_name = "m_" + pop_name + "_Pop" + pop_name
        try:
            if parameter == "bias":
                return getattr(self.h, pop_full_name)[nn].bias
            elif parameter == "gain":
                return getattr(self.h, pop_full_name)[nn].gain
            elif parameter == "tau":
                return getattr(self.h, pop_full_name)[nn].tau
            elif parameter == "state":
                return getattr(self.h, pop_full_name)[nn].state
        except AttributeError as e:
            print("Problem setting neuron parameter: %s" % e)

    def set_neuron_parameter(self, parameter, i, val):
        pop_name, nn = get_pop_number(i)
        pop_full_name = "m_" + pop_name + "_Pop" + pop_name
        try:
            if parameter == "bias":
                getattr(self.h, pop_full_name)[nn].bias = val
            elif parameter == "gain":
                getattr(self.h, pop_full_name)[nn].gain = val
            elif parameter == "tau":
                getattr(self.h, pop_full_name)[nn].tau = val
                getattr(self.h, pop_full_name)[nn].C = 1.0 / val
        except AttributeError as e:
            print("Problem setting neuron parameter: %s" % e)

    def run(self, skip_to_time=-1):
        # print_("> Current NEURON time: %s ms" % self.h.t)

        self.ns.advance()

        # print_("< Current NEURON time: %s ms" % self.h.t)

        # values = []
        pop_list = [
            "m_DA_PopDA",
            "m_DB_PopDB",
            "m_DD_PopDD",
            "m_VD_PopVD",
            "m_VA_PopVA",
            "m_VB_PopVB",
        ]

        values = []
        vars_read = []
        for i in range(10):
            for pop in pop_list:
                try:
                    # val = getattr(self.h, var)[0].soma.cai
                    val = getattr(self.h, pop)[i].output
                except AttributeError as e:
                    print("Problem passing neuronal output of %s, %s" % (pop, e))
                    continue
                    # val = 0
                # scaled_val = self._scale(val)
                values.append(val)
                vars_read.append(pop + "_" + str(i))

        # values = [6, 7, 8]
        # print(values)
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
    out_str = ""
    for val in list_val:
        out_str = out_str + " " + str(val)
    return out_str


if __name__ == "__main__":
    w = Worm2DNRNSimulation()
    w.set_timestep(0.005)
    w.set_neuron_inputs(0.0)
    # w.ns.sim_time = 10
    # out_vals = []
    # time_vals = []
    fout = open("Worm2D.outputs-test.dat", "w")
    for i in range(0, 10000):
        out_str = str(i) + listToStr(w.run())
        if i == 5000:
            pre_pop = "DD"
            post_pop = "DA"
            pre = get_neuron_number(pre_pop, 5)
            post = get_neuron_number(post_pop, 5)
            w.set_synaptic_weight(pre, post, 3)
            # w.set_input_weights(1.0)
            # w.set_neuron_input(53, 1.0)
        fout.write(out_str)
        fout.write("\n")
    # import numpy as np
    # out_arr = np.array(out_vals)

    fout.close()
    w.save_results()
