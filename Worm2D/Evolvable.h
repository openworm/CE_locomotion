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
   string fileprefix ;
   


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


  //void setFromArgs(int argc, const char* argv[]);
  //string rename_file(string filename);



string rename_file(string filename){return directoryName + "/" + 
    fileprefix + filename;}

void setFromArgs(int argc, const char* argv[])
{

  if (((argc-1) % 2) != 0)
     {cout << "The arguments are not configured correctly." << endl;exit(1);}
    
    bool seed_flag = 1;

    for (int arg = 1; arg<argc; arg+=2)
    { 
    if (strcmp(argv[arg],"--maxgens")==0) MaxGenerations = atoi(argv[arg+1]);
    //if (strcmp(argv[arg],"--doevol")==0) do_evol = atoi(argv[arg+1]);
    //if (strcmp(argv[arg],"--dorandinit")==0) simRandomInit = atoi(argv[arg+1]);
    //if (strcmp(argv[arg],"--skipOrigSim")==0) skipOrigSim = atoi(argv[arg+1]);
    //if (strcmp(argv[arg],"--donml")==0) do_nml = atoi(argv[arg+1]);


    if (strcmp(argv[arg],"--folder")==0) {
      directoryName= argv[arg+1];
      struct stat sb;
      if (stat(directoryName.c_str(), &sb) != 0) 
      {cout << "Directory doesn't exist." << endl;exit(1);}
    }

    if (seed_flag){ 
    if (strcmp(argv[arg],"-R")==0) randomseed = atoi(argv[arg+1]);
    if (strcmp(argv[arg],"-r")==0) 
    {long randomseed1 = static_cast<long>(time(NULL));
           randomseed = randomseed1 + atoi(argv[arg+1]);
    }
    seed_flag = 0;
    }
    //if (strcmp(argv[arg], "--modelname")==0) model_name = argv[arg+1];
    if (strcmp(argv[arg],"-p")==0) PopulationSize = atoi(argv[arg+1]);
    if (strcmp(argv[arg],"-d")==0) Duration = atoi(argv[arg+1]);
    if (strcmp(argv[arg],"-t")==0) Transient = atoi(argv[arg+1]);
    if (strcmp(argv[arg],"-cpt")==0) CheckpointInterval = atoi(argv[arg+1]);

    

    //cout << "doCPT " << doCPT << endl;
    //if (strcmp(argv[arg],"--nervous")==0) nervousSystemNameForSim = argv[arg+1];
    
    }

   

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
  virtual ~Evolvable(){}

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
  virtual ~EvolvableS(){}

  friend class EvolutionFull;
  protected:
  
};