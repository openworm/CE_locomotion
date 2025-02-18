#include "NervousSystem2D.h"
//#include "owSignalSimulator.h"

NervousSystem2D::NervousSystem2D(bool runNeuroML_):
NervousSystem(),runNeuroML(runNeuroML_),doNeuroMLSim(0),simulation(0)
{}

void NervousSystem2D::setSimulator(const std::string & simFileName, 
const std::string & simClassName, 
float timeStep)
{
    simulation = new SignalSimulator(simFileName, simClassName, timeStep);
    if (runNeuroML) doNeuroMLSim = 1;
}

void NervousSystem2D::SetNeuronExternalInput(int i, double value)
{
if (doNeuroMLSim) {return;}
NervousSystem::SetNeuronExternalInput(i,value);
return;
}


double NervousSystem2D::NeuronOutput(int i)
{
if (doNeuroMLSim) {return output_value[i-1];}
return NervousSystem::NeuronOutput(i);
}

void NervousSystem2D::EulerStep(double stepsize)
{    
     
    if (doNeuroMLSim) {output_value = simulation->run(); return;}
    NervousSystem::EulerStep(stepsize);
    return;
}

/* void NervousSystem2D::setNervousSystem(NervousSystem & n_)
{
    n = &n_;
    if (runNeuroML) doNeuroMLSim = 1;
} */

NervousSystem2D::~NervousSystem2D()
{if (simulation) delete simulation;}