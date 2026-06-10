//#include "TSearch.h"
//#include "VectorMatrix.h"
#include "Evolution.h"



class EvolutionCE:public Evolution
{
    public:
    EvolutionCE(int argc, const char* argv[])
    :Evolution(argc,argv, {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        96, 10, 0.05, 0.5, UNIFORM, 
        1.1, 0.02, 1, 1, 0, 10, 24, 8.0, 0.005, 23, 17}, 17){}

    EvolutionCE(shared_ptr<const CmdArgs> cmd_)
    :Evolution(cmd_, {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        96, 10, 0.05, 0.5, UNIFORM, 
        1.1, 0.02, 1, 1, 0, 10, 24, 8.0, 0.005, 23, 17}, 17){}    

    void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
    double EvaluationFunction(TVector<double> &v, RandomState &rs);
    double Evaluation(TVector<double> &v, RandomState &rs, int direction);
    //evoPars getEvoPars(const SuppliedArgs & sa);
    double save_traces(TVector<double> &v, RandomState &rs);
    void RunSimulation(TVector<double> &v, RandomState &rs);
    void RunSimulation(Worm2Dbase & w, RandomState &rs);
    void writeJson(TVector<double> &v);

    protected:
    void addExtraParsToJson(json & j);
  
    private:
    
    
    // Fitness traj
    const double    AvgSpeed = 0.0001; //0.00022;              // Average speed of the worm in meters per seconds
    const double    BBCfit = AvgSpeed*evoPars1.Duration;
    
    // Genotype -> Phenotype Mapping Ranges
    const double    BiasRange               = 16.0; //15.0;
    const double    SCRange                 = 16.0; //15.0;
    const double    CSRange                 = 16.0; //15.0;
    const double    ESRange                 = 2.0;
    const double    SRmax                   = 200.0;
    const double    NMJmax                  = 0.8; //1.2;
    const double    NMJmin                  = 0.0;
    
    const int SR_A = 1;
    const int SR_B = 2;
    
   
};
