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
        return

    def set_timestep(self, dt):
        print("Setting timestep to %s..." % dt)

        dt = float("{:0.1e}".format(dt)) * 1000.0  # memory issue fix

        import sys
        import sysconfig
        import os
        #sys.path.insert(0,sysconfig.get_paths()["purelib"])
        sys.path.append(sysconfig.get_paths()["purelib"])
        current = os.path.dirname(os.path.realpath(__file__))
        sim_dir = "neuromlLocal"
        sys.path.append(current) 
        #sys.path.append(current + '/' + sim_dir)
        print("sys path is : ", sys.path)
        #sys.exit()  
        #run_dir = "./"
        #sim_dir = "simulations/C1_Muscles_2025-02-12_12-13-48"
        #sim_dir = "neuromlLocal"
        #sim_dir = "./"
        #run_dir = sim_dir
        run_dir = "./"
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

            #nsp = importlib.import_module("simulations.C1_Muscles_2025-02-12_12-13-48.LEMS_c302_nrn")
            
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
        print_(
            "Initialised Worm2DNRNSimulation of length %s ms and dt = %s ms..."
            % (self.tstop, dt)
        )

    def run(self, skip_to_time=-1):
        print_("> Current NEURON time: %s ms" % self.h.t)

        self.ns.advance()

        print_("< Current NEURON time: %s ms" % self.h.t)

        #values = []
        
        values = [6,7,8]
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


if __name__ == "__main__":
    w = Worm2DNRNSimulation()
    w.set_timestep(0.005)
    # for _ in range(0,10):
    #   w.run()
