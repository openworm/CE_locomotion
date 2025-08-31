#include "../TSearch.h"
#include "../VectorMatrix.h"
#include "jsonUtils.h"


struct evoParsNonConst{
string filePrefix;

};

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
   string fileprefix;
   


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




/* class EvaluationFcn
{
public:
virtual double EvaluationFunction(TVector<double> &v, RandomState &rs) = 0;
virtual evoPars getDefaultEvoPars() = 0;
const evoPars * ep_ptr;
};

template<class T>
class Evaluation21 : EvaluationFcn
{
public:
double EvaluationFunction(TVector<double> &v, RandomState &rs);
evoPars getDefaultEvoPars();
}; */

template<typename Derived>
class Evolvable
{
  public:
  //Evolvable():ep_ptr(nullptr){}
  void itsGenPhenMapping(TVector<double> &gen, TVector<double> &phen) 
  {return Derived::GenPhenMapping(gen,phen);}
  
  //virtual void writeJson(TVector<double> &) = 0;
  //virtual evoPars getDefaultEvoPars() = 0;
  int itsVectSize() {return Derived::getVectSize();};
  //virtual ~Evolvable(){}

  friend class EvolutionFull;
  protected:
  //virtual double EvaluationFunction(TVector<double> &v, RandomState &rs) = 0;
  //virtual double EvaluationFunction(TVector<double> &v, RandomState &rs) = 0;
  //const evoPars * ep_ptr;
};


class EvolvableS
{
  public:
  
  virtual void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) = 0;
  virtual int getVectSize() = 0;
  

  friend class EvolutionFull;
  protected:
  
};