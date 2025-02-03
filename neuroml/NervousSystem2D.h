#include "../NervousSystem.h"
#include "owSignalSimulator.h"



class NervousSystem2D : public NervousSystem
{

public:

NervousSystem2D(bool runNeuroML_ = 1);

void setSimulator(
const std::string & simFileName = "main_sim", 
const std::string & simClassName = "Worm2DNRNSimulation", 
float timeStep=0.005);
void SetNeuronExternalInput(int i, double value);
double NeuronOutput(int i);
void EulerStep(double );
//void setNervousSystem(NervousSystem &n_);
~NervousSystem2D();

private:

const bool runNeuroML;
SignalSimulator *simulation;
//NervousSystem *n;
bool doNeuroMLSim;
std::vector<float> output_value;
std::vector<float> input_value;

};