//#include "NervousSystemBase.h"
//#include "../NervousSystem.h"
#include "NSBaseForW2D.h"
#include "owSignalSimulatorForWorm2D.h"
//#include "neuroml_utils.h"
//#include "../random.h"
#include <string>

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
    virtual ~c302ForW2D(){if (simulation) delete simulation;}

    

    void SetPopStructure(const std::string & popStruct, int popSize);
    

    protected:
    void SetPopStructure();
    SignalSimulatorForWorm2D *simulation = 0;
    std::vector<float> output_value;
    std::vector<float> state_value;
    bool newstep  = true;
    bool newstep2  = true;
    const std::string set_neuron_input_func, get_states_func, get_output_func;

};

class c302mForW2D :  public c302ForW2D, virtual public muscForW2D
{

double DorsalMuscleOutput(int muscle);
double VentralMuscleOutput(int muscle);



};