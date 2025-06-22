#include "TSearchCO.h"
//#include "VectorMatrix.h"
#include "Evolution.h"
#include "WormAgent.h"


const int	CircuitSize = 10;
const double TransientDuration = 50.0;
const double RunDuration = 100.0;
const double EvalDuration = RunDuration - TransientDuration;

//const double	HST				=	4.2;

const double BiasRange = 15.0;
const double SensorWeightRange = 1500.0;
const double InterneuronWeightRange = 15.0;
const double StretchReceptorRange = 15.0;

const double MaxDifSensor = HST;

const double TauMax = HST;

const double MinNeckTurnGain = 1.0;
const double MaxNeckTurnGain = 2.0;

//const double	Pi				=	3.1415926;
//const double	MaxDist			=	4.5;	
//using namespace TSCO;

class EvolutionCO : public Evolution
{
    public:
    

    EvolutionCO(int argc, const char* argv[], double StepSize_, int circuitsize_):Evolution(argc,argv,
      {".", 1749493257, RANK_BASED, GENETIC_ALGORITHM, 
        26, 40, 0.05, 0.5, UNIFORM, 
        1.1, 0.1, 1, 0, 1, 1, 50, 50, StepSize_, 23, getVectSize(circuitsize_)},
        getVectSize(circuitsize_)),MinDifSensor(10*StepSize_),TauMin(10*StepSize_)
    {}

    void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
    double EvaluationFunction(TVector<double> &v, RandomState &rs);
    void RunSimulation(TVector<double> &v, RandomState &rs){Behavior(v);}
    void RunSimulation(Worm2Dbase &w1, RandomState &rs){Behavior(w1);}
    //void RunStandardSimulation(Worm2Dm & w, RandomState &rs);
   
    double Behavior(TVector<double> &v);
    double Behavior(Worm2Dbase & w1);


    protected:
   
    int getVectSize(int circuitsize);

    void addExtraParsToJson(json & j);
    



const double TauMin;// = 10*evoPars1.StepSize;
const double MinDifSensor;// = 10*evoPars1.StepSize;

};