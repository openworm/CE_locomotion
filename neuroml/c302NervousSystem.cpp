#include "c302NervousSystem.h"
//#include "owSignalSimulator.h"

c302NervousSystem::c302NervousSystem(const std::string & simFileName):
simulation(nullptr)
{
setSimulator(simFileName);
}

void c302NervousSystem::setSimulator(const std::string & simFileName, 
const std::string & simClassName, 
float timeStep)
{   if (simulation!=nullptr) delete simulation;
    simulation = new SignalSimulator(simFileName, simClassName, timeStep);
}

void c302NervousSystem::SetNeuronExternalInput(int i, double value)
{
return;
}

double c302NervousSystem::NeuronOutput(int i)
{
return output_value[i-1];
}

void c302NervousSystem::EulerStep(double stepsize)
{    
     
    output_value = simulation->run();
    return;
}


c302NervousSystem::~c302NervousSystem()
{if (simulation!=nullptr) delete simulation;}