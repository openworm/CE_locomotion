#include "c302NervousSystem.h"
//#include "owSignalSimulator.h"

c302NervousSystem::c302NervousSystem(const std::string & simFileName):
simulation(nullptr)
{
setSimulator(simFileName);
}

c302NervousSystem::c302NervousSystem():
simulation(nullptr)
{
setSimulator();
}

void c302NervousSystem::setSimulator(const std::string & simFileName, 
const std::string & simClassName, 
float timeStep)
{   if (simulation!=nullptr) delete simulation;
    simulation = new SignalSimulatorForWorm2D(simFileName, simClassName, timeStep);
}

void c302NervousSystem::SetNeuronExternalInput(int i, double value)
{
simulation->setNeuronInput(i-1,value);
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

void c302NervousSystem::SetChemicalSynapseWeight(int from, int to, double value)
{
    simulation->SetChemicalSynapseWeight(from-1, to-1, value);
}

c302NervousSystem::~c302NervousSystem()
{if (simulation!=nullptr) delete simulation;}