#pragma once
#include "../TSearch.h"
#include "../VectorMatrix.h"
//#include "../argUtils.h"
#include <functional>
#include <iomanip> 
#include <string.h>
#include "jsonUtils.h"
#include "Worm2D.h"

template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)> {
   template <typename... Args> 
   static Ret callback(Args... args) {                    
      return func(args...);  
   }
   static std::function<Ret(Params...)> func; 
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret(Params...)>::func;

struct evoPars;


struct evoPars{
   string directoryName;
   long randomseed;
   TSelectionMode SelectionMode;
   TReproductionMode ReproductionMode;
   int PopulationSize;
   int MaxGenerations;
   double MutationVariance;
   double CrossoverProbability;
   TCrossoverMode CrossoverMode;
   double MaxExpectedOffspring;
   double ElitistFraction;
   int SearchConstraint;
   int CheckpointInterval;
   bool ReEvaluationFlag;
   int skip_steps;
   // Integration parameters
   double Duration;       //
   double Transient;       //
   double StepSize;
   int N_curvs;
   int VectSize;


   const doubIntParamsHead getParams() const
   {
       doubIntParamsHead var1;
       var1.parDoub.head = "Evolutionary Optimization Parameters";
       var1.parInt.head = "Evolutionary Optimization Parameters";
       var1.parDoub.names = 
       {"MutationVariance", "CrossoverProbability", 
         "MaxExpectedOffspring", "ElitistFraction",
         "Duration", "Transient", "StepSize"};
       var1.parDoub.vals = {MutationVariance, CrossoverProbability, 
         MaxExpectedOffspring, ElitistFraction,
         Duration, Transient, StepSize};

       var1.parInt.names = {"randomseed", "SelectionMode", "ReproductionMode", 
         "PopulationSize", "MaxGenerations", "CrossoverMode", "SearchConstraint", 
         "CheckpointInterval", "ReEvaluationFlag", "skip_steps", "N_curvs", "VectSize"};
       var1.parInt.vals = {randomseed, SelectionMode, ReproductionMode, 
         PopulationSize, MaxGenerations, CrossoverMode, SearchConstraint, CheckpointInterval, 
         ReEvaluationFlag, skip_steps, N_curvs, VectSize};

       return var1;
   }

};

/* template<class T> 
T getParameter(int argc, const char* argv[], string parName, T defaultval){
    
    T retval = defaultval;
    if (((argc-1) % 2) != 0)
    {cout << "The arguments are not configured correctly." << endl;exit(1);}
    for (int arg = 1; arg<argc; arg+=2) 
    if (strcmp(argv[arg],parName.c_str())==0) {retval = atoi(argv[arg+1]);break;}
    return retval;
} */

const char* getParameter(int argc, const char* argv[], string parName, const char* defaultval);





class Evolution
{
    public:
    virtual void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) = 0;
    virtual double EvaluationFunction(TVector<double> &v, RandomState &rs) = 0;
    virtual void RunSimulation(TVector<double> &v, RandomState &rs) = 0;
    virtual void RunSimulation(Worm2Dm & w, RandomState &rs) = 0;
    void RunStandardSimulation(Worm2Dm & w, RandomState &rs);

    void addParsToJson(json & j);
    
    virtual void configure();
    
    
    const evoPars & itsEvoPars() const {return evoPars1;}

    virtual ~Evolution()
    {
      evolfile.close();
      if (s) delete s;
    }


    string rename_file(string filename){return evoPars1.directoryName + "/" + filename;}

    protected:
    evoPars setPars(int argc, const char* argv[], evoPars ep1);

    
    virtual void configure_p1();
    virtual void configure_p2();
    void EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar);
    void ResultsDisplay(TSearch &s);
    
    
    Evolution(int argc, const char* argv[], evoPars ep1, int VectSize_)
    :evoPars1(setPars(argc,argv,ep1)),s(new TSearch(VectSize_))
    {
      evolfile.open(rename_file("fitness.dat"));
      evolfile << setprecision(10);
    }

    virtual void addExtraParsToJson(json & j) = 0;
    TSearch* const s; 
    const evoPars evoPars1;
    
    private:
 
    ofstream evolfile;
   

};

