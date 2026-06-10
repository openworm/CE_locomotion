#include "c302NervousSystem.h"

void c302NervousSystem::SetChemicalSynapseWeight(int from, int to, double value)
{
    if (skipCalc) return;
    simulation->twoValFunc("set_synaptic_weight",from-1,to-1,value);
}

void c302NervousSystem::SetNeuronBias(int i, double value)
{
    if (skipCalc) return;
    simulation->strValFunc("set_neuron_parameter", "bias", i-1, value);
}

void c302NervousSystem::SetNeuronGain(int i, double value)
{   
    if (skipCalc) return;
    simulation->strValFunc("set_neuron_parameter", "gain", i-1, value);
}

void c302NervousSystem::SetNeuronTimeConstant(int i, double value)
{   
    if (skipCalc) return;
    simulation->strValFunc("set_neuron_parameter", "tau", i-1, value);
}

const double & c302NervousSystem::NeuronTimeConstant(int i)
{
    if (skipCalc) return zero_value;
    cached_value = simulation->getValFunc("get_neuron_parameter", "tau", i-1);
    return cached_value;
}

const double & c302NervousSystem::NeuronBias(int i)
{
    if (skipCalc) return zero_value;
    cached_value = simulation->getValFunc("get_neuron_parameter", "bias", i-1);
    return cached_value;
}

const double & c302NervousSystem::NeuronState(int i)
{
    if (skipCalc) return zero_value;
    cached_value = simulation->getValFunc("get_neuron_parameter", "state", i-1);
    return cached_value;
}
const double & c302NervousSystem::ChemicalSynapseWeight(int from, int to)
{
    if (skipCalc) return zero_value;
    cached_value = simulation->getTwoValFunc("get_synaptic_weight", from-1, to-1);
    return cached_value;

}

void c302NervousSystem::RandomizeCircuitState(double lb, double ub) {return;}
void c302NervousSystem::RandomizeCircuitState(double lb, double ub, RandomState &rs) {return;}
void c302NervousSystem::RandomizeCircuitOutput(double lb, double ub) {return;}
void c302NervousSystem::RandomizeCircuitOutput(double lb, double ub, RandomState &rs) {return;}

void c302NervousSystem::SetCircuitSize(int newsize, int maxchemconns, int maxelecconns) {return;}
void c302NervousSystem::SetNeuronOutput(int i, double value) {return;}
const double & c302NervousSystem::ElectricalSynapseWeight(int from, int to) {return zero_value;}
void c302NervousSystem::SetElectricalSynapseWeight(int n1, int n2, double value) {return;} 
