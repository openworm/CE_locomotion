#include "TSearchCO.h"
//#include "VectorMatrix.h"
#include "Evolution.h"
#include "WormAgent.h"
//#include "EvolutionRS18.h"

//const int	CircuitSize = 10;
//const double TransientDuration = 50.0;
//const double RunDuration = 100.0;
//const double EvalDuration = RunDuration - TransientDuration;

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

class EvolutionCO : virtual public Evolution
{
    protected:

    const double TauMin; // = 10*evoPars1.StepSize;
    const double MinDifSensor; // = 10*evoPars1.StepSize;
    const int CircuitSize;

   
    public:

    EvolutionCO(int argc, const char* argv[], double StepSize_, int circuitsize_):Evolution(argc,argv,
      {".", 1749493257, RANK_BASED, GENETIC_ALGORITHM, 
        26, 40, 0.05, 0.5, UNIFORM, 
        1.1, 0.1, 1, 1, 1, 1, 50, 50, StepSize_, 23, getVectSize(circuitsize_)},
        getVectSize(circuitsize_)),MinDifSensor(10*StepSize_),TauMin(10*StepSize_),
        CircuitSize(circuitsize_){}

    EvolutionCO(shared_ptr<const CmdArgs> cmd_, double StepSize_, int circuitsize_):Evolution(cmd_,
      {".", 1749493257, RANK_BASED, GENETIC_ALGORITHM, 
        26, 40, 0.05, 0.5, UNIFORM, 
        1.1, 0.1, 1, 1, 1, 1, 50, 50, StepSize_, 23, getVectSize(circuitsize_)},
        getVectSize(circuitsize_)),MinDifSensor(10*StepSize_),TauMin(10*StepSize_),
        CircuitSize(circuitsize_){ cout << "evol CO " << endl;}


    virtual void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
    virtual double EvaluationFunction(TVector<double> &v, RandomState &rs);
    void RunSimulation(TVector<double> &v, RandomState &rs){Behavior(v);}
    void RunSimulation(Worm2Dbase &w1, RandomState &rs){Behavior(w1);}
    //void RunStandardSimulation(Worm2Dm & w, RandomState &rs);
    
    virtual double Behavior(TVector<double> &v);
    double Behavior(Worm2Dbase & w1);
    void writeJson(TVector<double> &);

    protected:
    //EvolutionCO(double StepSize_, int circuitsize_):MinDifSensor(10*StepSize_),TauMin(10*StepSize_),
     //   CircuitSize(circuitsize_){}

    double Behavior(TVector<double> &v, WormAgent * Worm);
    double EvaluationFunction(TVector<double> &v, RandomState &rs, WormAgent * Worm);
    static int getVectSize(int circuitsize);

    void addExtraParsToJson(json & j);
    template<class T> double EvaluationFunction(TVector<double> &v, RandomState &rs);
    template<class T> double Behavior(TVector<double> &v);
    //template<class T> double Behavior<WormCO18Full>(TVector<double> &v);




};

template<class T>
double EvolutionCO::EvaluationFunction(TVector<double> &v, RandomState &rs)
{
  
	T * Worm = new T(CircuitSize);
	double val = EvaluationFunction(v, rs, Worm);
	delete Worm;
	return val;
}

template<class T>
double EvolutionCO::Behavior(TVector<double> &v)
{
T * Worm =  new T(CircuitSize);
double val = Behavior(v, Worm);
delete Worm;
return val;
}

/* class A
{
  public:
  A(int x_){x=x_;}

  int x;
};
class B : public A
{
  public:
  B():A(getVal()){}

  int getVal(){return 1;}

}; */
