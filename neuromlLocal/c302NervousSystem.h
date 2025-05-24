#include "NervousSystemBase.h"
//#include "../NervousSystem.h"
//#include "owSignalSimulatorForWorm2D.h"
#include "neuroml_utils.h"
//#include "../random.h"
#include "c302ForW2D.h"

class c302NervousSystem : public NervousSystemBase, public c302ForW2D
{

public:

c302NervousSystem(const std::string & simFileName):c302ForW2D(simFileName){}
c302NervousSystem():c302ForW2D(){}
c302NervousSystem(const std::string & simFileName, const std::string & simDirName)
:c302ForW2D(simFileName,simDirName){}
c302NervousSystem(const std::string & popStruct, const int & popSize):c302ForW2D(popStruct,popSize){}

//required for nervous system setup

void SetChemicalSynapseWeight(int from, int to, double value);
void SetNeuronBias(int i, double value);
double NeuronBias(int i);
void SetNeuronGain(int i, double value);
void SetNeuronTimeConstant(int i, double value);
double NeuronTimeConstant(int i);
double NeuronState(int i);
double ChemicalSynapseWeight(int from, int to);


void SetCircuitSize(int newsize, int maxchemconns, int maxelecconns) ;
void SetNeuronOutput(int i, double value);
double ElectricalSynapseWeight(int from, int to);
void SetElectricalSynapseWeight(int n1, int n2, double value);


    void RandomizeCircuitState(double lb, double ub);
    void RandomizeCircuitState(double lb, double ub, RandomState &rs);
    void RandomizeCircuitOutput(double lb, double ub);
    void RandomizeCircuitOutput(double lb, double ub, RandomState &rs);
 

 const std::vector<float> & getOutputValues() const {return output_value;}
 ostream & writeOutputValues(ostream & os) {return writeVector(os,output_value);}
 bool skipCalc = 1;



};