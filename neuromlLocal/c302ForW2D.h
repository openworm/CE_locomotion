//#include "NervousSystemBase.h"
//#include "../NervousSystem.h"
#include "NSBaseForW2D.h"
#include "owSignalSimulatorForWorm2D.h"
//#include "neuroml_utils.h"
//#include "../random.h"
#include <string>
#include <memory>

class c302ForW2D : virtual public NSForW2D
{

    public:
    c302ForW2D(const std::string & popStruct, const int & popSize);
    c302ForW2D(const std::string & simFileName);
    c302ForW2D();
    c302ForW2D(const std::string & simFileName, const std::string & simDirName);

    void SetNeuronExternalInput(int i, double value);
    double NeuronOutput(int i);
    void EulerStep(double );
    
    double NeuronState(int i);
    virtual ~c302ForW2D(){}
    //virtual ~c302ForW2D(){if (simulation) delete simulation;}
    //std::shared_ptr<SignalSimulatorForWorm2D>  getSimulator(){return simulation;}
    //const std::shared_ptr<bool> get_v_newstep(){return v_newstep;}

    void SetPopStructure(const std::string & popStruct, int popSize);
    
    friend class c302muscForW2D;

    protected:
    void SetPopStructure();
    const std::shared_ptr<SignalSimulatorForWorm2D> simulation;
    //SignalSimulatorForWorm2D *simulation = 0;
    std::vector<float> output_value;
    std::vector<float> state_value;
    bool newstep  = true;
    bool newstep2  = true;
    const std::shared_ptr<bool> v_newstep;
    const std::shared_ptr<bool> d_newstep;
    //bool v_newstep  = true;
    //bool d_newstep  = true;

    const std::string set_neuron_input_func, get_states_func, get_output_func;

};

class c302muscForW2D :  public muscForW2D
{

public:
c302muscForW2D(c302ForW2D & c_);


double DorsalMuscleOutput(int muscle);
double VentralMuscleOutput(int muscle);
virtual ~c302muscForW2D(){}

protected:

const std::shared_ptr<SignalSimulatorForWorm2D> simulation;
//SignalSimulatorForWorm2D *simulation = 0;
std::vector<float> v_output_value;
std::vector<float> d_output_value;
const std::shared_ptr<bool> v_newstep;
const std::shared_ptr<bool> d_newstep;

const std::string get_v_output_func, get_d_output_func;
};