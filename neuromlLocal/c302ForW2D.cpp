#include "c302ForW2D.h"
//#include "owSignalSimulator.h"

//const bool skipCalc = 1;
const std::string defaultSimClassName = "Worm2DNRNSimulation";

c302ForW2D::c302ForW2D(const std::string & simFileName):
set_neuron_input_func("set_neuron_input"), v_newstep(new bool(true)), d_newstep(new bool(true)), 
get_states_func("get_states"), get_output_func("get_outpus"),
simulation(new SignalSimulatorForWorm2D(simFileName,defaultSimClassName,0.005))
{SetPopStructure("DA DB DD VD VA VB", 10);}

//c302NervousSystem::c302NervousSystem():
//simulation(new SignalSimulatorForWorm2D("neuromlLocal.main_sim",defaultSimClassName,0.005)){}

c302ForW2D::c302ForW2D():
set_neuron_input_func("set_neuron_input_j"), v_newstep(new bool(true)), d_newstep(new bool(true)), 
get_states_func("get_states_j"), get_output_func("get_outputs_j"),
simulation(new SignalSimulatorForWorm2D("main_sim",defaultSimClassName,"neuromlLocal",0.005))
{SetPopStructure(); std::cout << "c302ForW2D" << std::endl;}

c302ForW2D::c302ForW2D(const std::string & popStruct, const int & popSize):
set_neuron_input_func("set_neuron_input"), v_newstep(new bool(true)), d_newstep(new bool(true)), 
get_states_func("get_states"), get_output_func("get_outputs"),
simulation(new SignalSimulatorForWorm2D("main_sim",defaultSimClassName,"neuromlLocal",0.005))
{SetPopStructure(popStruct, popSize);}

c302ForW2D::c302ForW2D(const std::string & simFileName, 
const std::string & simDirName):v_newstep(new bool(true)), d_newstep(new bool(true)), 
set_neuron_input_func("set_neuron_input"), 
get_states_func("get_states"), get_output_func("get_outputs"),
simulation(new SignalSimulatorForWorm2D(simFileName,
defaultSimClassName,simDirName,0.005)){SetPopStructure("DA DB DD VD VA VB", 10);}

void c302ForW2D::SetPopStructure()
{
    std::cout << "set_up_j" << std::endl;
    simulation->oneValFunc("set_up_j");
}


void c302ForW2D::SetPopStructure(const std::string & popStruct, int popSize)
{
    std::cout << "set_up" << std::endl;
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

{     //   std::cout << "euler" << std::endl;
    simulation->run();
    newstep=true;
    newstep2=true;
    *v_newstep = true;
    *d_newstep = true;
}


c302muscForW2D::c302muscForW2D(c302ForW2D & c_):simulation(c_.simulation),v_newstep(c_.v_newstep),
d_newstep(c_.d_newstep),
get_d_output_func("get_dorsal_musc_states"),get_v_output_func("get_ventral_musc_states")
{

    std::cout << "c302muscForW2D" << std::endl;
}

double c302muscForW2D::DorsalMuscleOutput(int muscle){

if (*d_newstep){
    d_output_value =  simulation->vecValFunc(get_d_output_func);
    *d_newstep = false;
    }
return d_output_value[muscle-1];
}

double c302muscForW2D::VentralMuscleOutput(int muscle)
{

if (*v_newstep){
    v_output_value =  simulation->vecValFunc(get_v_output_func);
    *v_newstep = false;
    }
return v_output_value[muscle-1];
}
