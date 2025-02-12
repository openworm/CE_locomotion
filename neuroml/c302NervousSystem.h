#include "../NervousSystemBase.h"
#include "owSignalSimulator.h"



class c302NervousSystem : public NervousSystemBase
{

public:

c302NervousSystem(const std::string & simFileName);
c302NervousSystem();

void setSimulator(
const std::string & simFileName = "neuroml.main_sim", 
const std::string & simClassName = "Worm2DNRNSimulation", 
float timeStep=0.005);

void SetNeuronExternalInput(int i, double value);
double NeuronOutput(int i);
void EulerStep(double );

~c302NervousSystem();



// int CircuitSize(void)  {}
 void SetCircuitSize(int newsize, int maxchemconns, int maxelecconns) {}
 double NeuronState(int i) {}
// void SetNeuronState(int i, double value) {}
 
 void SetNeuronOutput(int i, double value) {}
 double NeuronBias(int i) {}
 void SetNeuronBias(int i, double value) {}
// double NeuronGain(int i) {}
 void SetNeuronGain(int i, double value) {}
 double NeuronTimeConstant(int i) {}
 void SetNeuronTimeConstant(int i, double value) {}
// double NeuronExternalInput(int i) {}
 double ChemicalSynapseWeight(int from, int to) {}
 void SetChemicalSynapseWeight(int from, int to, double value) {}
 double ElectricalSynapseWeight(int from, int to) {}
// void InternalSetElectricalSynapseWeight(int from, int to, double value) {}
 void SetElectricalSynapseWeight(int n1, int n2, double value) {}
 

private:

SignalSimulator *simulation;
std::vector<float> output_value;
std::vector<float> input_value;

};