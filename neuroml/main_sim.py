def print_(msg):
    pre = "Python >> "
    print('%s %s'%(pre,msg.replace('\n','\n'+pre)))


class Worm2DNRNSimulation():

    max_ca = 4e-7
    max_ca_found = -1

    def __init__(self):
        self.tstop = 1
        return

    def set_timestep(self, dt):

        print('Setting timestep to %s...'%dt)

        dt = float('{:0.1e}'.format(dt)) * 1000.0 # memory issue fix
        
        try:
            import sys
            import sysconfig; 
            sys.path.append(sysconfig.get_paths()["purelib"])
            #from simulations.LEMS_Worm2D_nrn import NeuronSimulation
            #from simulations.LEMS_c302_nrn import NeuronSimulation
            import importlib  
            nsp = importlib.import_module("simulations.C1_Muscles_2025-02-04_14-04-08.LEMS_c302_nrn")
            #from NeuronSimulation_package import NeuronSimulation
            #from simulations.C1_Muscles_2025-02-04_14-04-08.LEMS_c302_nrn import NeuronSimulation
            import neuron
            self.h = neuron.h
        except Exception as e:
            raise Exception('Python import error in Worm2DNRNSimulation: %s..'%e)

        self.ns = nsp.NeuronSimulation(self.tstop, dt)
        print_("Initialised Worm2DNRNSimulation of length %s ms and dt = %s ms..."%(self.tstop,dt))

    def run(self, skip_to_time=-1):

        print_("> Current NEURON time: %s ms"%self.h.t)

        self.ns.advance()

        print_("< Current NEURON time: %s ms"%self.h.t)
        
        values = []
        vars_read = []
        for i in range(24):
            var = "a_MDR%s"%(i+1 if i>8 else ("0%i"%(i+1)))
            try:
                val = getattr(self.h, var)[0].soma.cai
            except AttributeError as e:
                print("Problem passing neuronal output of %s to muscle in Sibernetic: %s"%(var, e))
                continue
                val = 0
            scaled_val = self._scale(val)
            values.append(scaled_val)
            vars_read.append(var)
        for i in range(24):
            var = "a_MVR%s"%(i+1 if i>8 else ("0%i"%(i+1)))
            if i == 23:
                var = "a_MVR23"
            try:
                val = getattr(self.h, var)[0].soma.cai
            except AttributeError as e:
                print("Problem passing neuronal output of %s to muscle in Sibernetic: %s"%(var, e))
                val = 0
                continue
            scaled_val = self._scale(val)
            values.append(scaled_val)
            vars_read.append(var)
        for i in range(24):
            var = "a_MVL%s"%(i+1 if i>8 else ("0%i"%(i+1)))
            try:
                val = getattr(self.h, var)[0].soma.cai
            except AttributeError as e:
                if var=='a_MVL24':
                    extra = "Note: not an issue as no muscle MVL24 in the real C. elegans"
                else:
                    extra = ''
                print("Problem passing output of %s to muscle in Sibernetic: %s %s"%(var, e, extra))

                val = 0
                continue
            scaled_val = self._scale(val)
            values.append(scaled_val)
            vars_read.append(var)
        for i in range(24):
            var = "a_MDL%s"%(i+1 if i>8 else ("0%i"%(i+1)))
            try:
                val = getattr(self.h, var)[0].soma.cai
            except AttributeError as e:
                print("Problem passing neuronal output of %s to muscle in Sibernetic: %s"%(var, e))
                val = 0
                continue
            scaled_val = self._scale(val)
            values.append(scaled_val)
            vars_read.append(var)


        print(values[0], values[1], values[2])
        #values = [6,7,8]
        return values
    

    def _scale(self,ca,print_it=False):

        self.max_ca_found = max(ca,self.max_ca_found)
        scaled = min(1,(ca/self.max_ca))
        if print_it:
            print_("- Scaling %s to %s (max found: %s)"%(ca,scaled,self.max_ca_found))
        return scaled

if __name__ == "__main__":   
    w = Worm2DNRNSimulation()
    w.set_timestep(0.005)
    for _ in range(0,10):
       w.run()