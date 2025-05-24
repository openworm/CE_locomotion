#include "c302ForW2D.h"
//#include "owSignalSimulator.h"

//const bool skipCalc = 1;
const std::string defaultSimClassName = "Worm2DNRNSimulation";

c302ForW2D::c302ForW2D(const std::string & simFileName):
set_neuron_input_func("set_neuron_input"), 
get_states_func("get_states"), get_output_func("get_outpus"),
simulation(new SignalSimulatorForWorm2D(simFileName,defaultSimClassName,0.005))
{SetPopStructure("DA DB DD VD VA VB", 10);}

//c302NervousSystem::c302NervousSystem():
//simulation(new SignalSimulatorForWorm2D("neuromlLocal.main_sim",defaultSimClassName,0.005)){}

c302ForW2D::c302ForW2D():
set_neuron_input_func("set_neuron_input_j"), 
get_states_func("get_states_j"), get_output_func("get_outputs_j"),
simulation(new SignalSimulatorForWorm2D("main_sim",defaultSimClassName,"neuromlLocal",0.005))
{SetPopStructure();}

c302ForW2D::c302ForW2D(const std::string & popStruct, const int & popSize):
set_neuron_input_func("set_neuron_input"), 
get_states_func("get_states"), get_output_func("get_outputs"),
simulation(new SignalSimulatorForWorm2D("main_sim",defaultSimClassName,"neuromlLocal",0.005))
{SetPopStructure(popStruct, popSize);}

c302ForW2D::c302ForW2D(const std::string & simFileName, 
const std::string & simDirName):
set_neuron_input_func("set_neuron_input"), 
get_states_func("get_states"), get_output_func("get_outputs"),
simulation(new SignalSimulatorForWorm2D(simFileName,
defaultSimClassName,simDirName,0.005)){SetPopStructure("DA DB DD VD VA VB", 10);}

void c302ForW2D::SetPopStructure()
{
    simulation->oneValFunc("set_up_j");
}


void c302ForW2D::SetPopStructure(const std::string & popStruct, int popSize)
{
    simulation->strOneValFunc("set_up", popStruct, popSize);
}

void c302ForW2D::SetNeuronExternalInput(int i, double value)
{
simulation->oneValFunc(set_neuron_input_func,i-1,value);
}


double c302ForW2D::NeuronState(int i)
{
    if (newstep2){
    state_value =  simulation->vecValFunc(get_states_func);
    newstep2 = false;
    }
return state_value[i-1];
}

double c302ForW2D::NeuronOutput(int i)
{
    if (newstep){
    output_value =  simulation->vecValFunc(get_output_func);
    newstep = false;
    }
return output_value[i-1];
}

void c302ForW2D::EulerStep(double stepsize)
{       
    simulation->run();
    newstep=true;
    newstep2=true;
}

double c302mForW2D::DorsalMuscleOutput(int muscle){}
double c302mForW2D::VentralMuscleOutput(int muscle){}
