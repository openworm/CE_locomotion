//#include "TSearch.h"
//#include "VectorMatrix.h"
#include "Evolution.h"


class Evolution21:public Evolution
{
    public:
    

    Evolution21(int argc, const char* argv[]):Evolution(argc,argv,
      {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        100, 2000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 1, 0, 10, 40.0, 10.0, 0.005, 23, 44},44
    ){}

    Evolution21(shared_ptr<const CmdArgs> cmd_):Evolution(cmd_,
      {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        100, 2000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 1, 0, 10, 40.0, 10.0, 0.005, 23, 44},44
    ){}

    void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
    double EvaluationFunction(TVector<double> &v, RandomState &rs);
    double EvaluationFunction1(TVector<double> &v, RandomState &rs);
    double EvaluationFunction2(TVector<double> &v, RandomState &rs);
    double EvaluationFunction2Output(TVector<double> &v, RandomState &rs);
    void RunSimulation(TVector<double> &v, RandomState &rs);
    void RunSimulation(Worm2Dbase &w1, RandomState &rs);
    int finish_Bosc(int Generation,double BestPerf,double AvgPerf,double PerfVar);
    void writeJson(TVector<double> &v);

    
    protected:
    

    void addExtraParsToJson(json & j);
    
     
     const double OSCT = 0.25 * evoPars1.Duration; // Cap for oscillation evaluation
     const double agarfreq = 0.44;
     
     // Genotype -> Phenotype Mapping (Ventral cord)
     const double	BiasRange				= 15.0;
     const double    SCRange                 = 15.0;
     const double    CSRange                 = 15.0;
     const double    TauMin                 = 0.1;
     const double    TauMax                 = 2.5;
     const double    ESRange                 = 2.0;
     const double    NMJmax                  = 1.2;
     const double    IIRange                 = 15.0;
     
     // Fitness
     const double    AvgSpeed = 0.00022;             // Average speed of the worm in meters per seconds
     const double    BBCfit = AvgSpeed*evoPars1.Duration;

     
     void configure_p2();
   
};
