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
//simulation->setNeuronInput(i-1,value);
simulation->oneValFunc("set_neuron_input",i-1,value);
}

double c302NervousSystem::NeuronOutput(int i)
{
return output_value[i-1];
}

void c302NervousSystem::EulerStep(double stepsize)
{       
    output_value = simulation->run();
}

void c302NervousSystem::SetChemicalSynapseWeight(int from, int to, double value)
{
    simulation->twoValFunc("set_synaptic_weight",from-1,to-1,value);
}

void c302NervousSystem::SetNeuronBias(int i, double value)
{
    simulation->strValFunc("set_neuron_parameter", "bias", i-1, value);
}

void c302NervousSystem::SetNeuronGain(int i, double value)
{
    simulation->strValFunc("set_neuron_parameter", "gain", i-1, value);
}

void c302NervousSystem::SetNeuronTimeConstant(int i, double value)
{
    simulation->strValFunc("set_neuron_parameter", "tau", i-1, value);
}

double c302NervousSystem::NeuronBias(int i)
{

    return simulation->getValFunc("get_neuron_parameter", "bias", i-1);
}

c302NervousSystem::~c302NervousSystem()
{if (simulation!=nullptr) delete simulation;}