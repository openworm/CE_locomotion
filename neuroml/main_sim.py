def print_(msg):
    pre = "Python >> "
    print('%s %s'%(pre,msg.replace('\n','\n'+pre)))


class Worm2DNRNSimulation():

    def __init__(self):
        return

    def set_timestep(self, dt):

        print('Setting timestep to %s...'%dt)

        dt = float('{:0.1e}'.format(dt)) * 1000.0 # memory issue fix
        
        try:
            import sys
            print('Python path is: ', sys.path)
            from simulations.LEMS_Worm2D_nrn import NeuronSimulation
            import neuron
            self.h = neuron.h
        except Exception as e:
            raise Exception('Python import error in Worm2DNRNSimulation: %s..'%e)

        self.ns = NeuronSimulation(self.tstop, dt)
        print_("Initialised Worm2DNRNSimulation of length %s ms and dt = %s ms..."%(self.tstop,dt))

    def run(self, skip_to_time=-1):

        print_("> Current NEURON time: %s ms"%self.h.t)

        #self.ns.advance()

        print_("< Current NEURON time: %s ms"%self.h.t)

        values = [1,2,3]
        return values    