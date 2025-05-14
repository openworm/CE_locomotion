//#include "NervousSystemBase.h"
//#include "../NervousSystem.h"
#include "NSBaseForW2D.h"
#include "owSignalSimulatorForWorm2D.h"
//#include "neuroml_utils.h"
//#include "../random.h"

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
    void SetPopStructure(const std::string & popStruct, int popSize);
    double NeuronState(int i);
    virtual ~c302ForW2D(){if (simulation) delete simulation;}

    protected:

    SignalSimulatorForWorm2D *simulation = 0;
    std::vector<float> output_value;
    std::vector<float> state_value;
    bool newstep  = true;
    bool newstep2  = true;

};