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
   string evoType;
   
  void addParsToJson(json &j) const
  {

    {vector<string> names = {"MutationVariance", "CrossoverProbability", 
         "MaxExpectedOffspring", "ElitistFraction",
         "Duration", "Transient", "StepSize"};
    vector<double> vals = {MutationVariance, CrossoverProbability, 
         MaxExpectedOffspring, ElitistFraction,
         Duration, Transient, StepSize};
    for (int i=0;i<names.size();i++) j[names[i]]["value"]=vals[i];}

    {vector<string> names = {"randomseed", "SelectionMode", "ReproductionMode", 
         "PopulationSize", "MaxGenerations", "CrossoverMode", "SearchConstraint", 
         "CheckpointInterval", "ReEvaluationFlag", "skip_steps", "N_curvs", "VectSize"};
    vector<int> vals = {(int) randomseed, SelectionMode, ReproductionMode, 
         PopulationSize, MaxGenerations, CrossoverMode, SearchConstraint, CheckpointInterval, 
         ReEvaluationFlag, skip_steps, N_curvs, VectSize};
    for (int i=0;i<names.size();i++) j[names[i]]["value"]=vals[i];}

      {vector<string> names = {"fileprefix", "evoType"};
      vector<string> vals = {fileprefix, evoType};
       for (int i=0;i<names.size();i++) j[names[i]]["value"]=vals[i];}


  }

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
     {
      cout << "The arguments are not configured correctly." << endl;
      exit(1);
    }
    
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
    if (strcmp(argv[arg],"--evoType")==0) evoType = argv[arg+1];
    

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

class W2Dparameters
{
public:
virtual ~W2Dparameters(){}
virtual void setParsFromJson(json & j){}
virtual void addParsToJson(json & j){}
};

class Evolparameters : virtual public W2Dparameters
{
public:
int dbunit;
int vbunit;
void setParsFromJson(json & j){dbunit = j["dbunit"]["value"]; vbunit = j["vbunit"]["value"]; }
void addParsToJson(json & j){j["dbunit"]["value"] = dbunit; j["vbunit"]["value"] = vbunit;}
};

struct W2DCEpars : virtual public W2Dparameters
{
public:
W2DCEpars(){}
W2DCEpars(int argc, const char* argv[]);

string sr_type = "None";
double AVA_output = 0, AVB_output = 0;
double AB_output_level = 1;

void show(){cout << "srtype " << sr_type << 
  " AVA_output_level "  << AB_output_level << " AVA_output " << 
  AVA_output << " AVB_output " << AVB_output << endl;}


void setParsFromJson(json & j){
  sr_type = j["SRType"]["value"]; 
  AB_output_level = j["AB_output_level"]["value"];
  AVA_output = j["AVA_output"]["value"]; AVB_output = j["AVB_output"]["value"]; 
}
void addParsToJson(json & j){
  j["SRType"]["value"] = sr_type; 
  j["AB_output_level"]["value"] = AB_output_level;
  j["AVB_output"]["value"] = AVB_output; 
  j["AVB_output"]["value"] = AVB_output;
}

};



class EvolparametersCE : public W2DCEpars
{
public:
EvolparametersCE(int argc, const char* argv[]);
EvolparametersCE(){}
int doReverse = 0;
//double SR_B_gain = 0, SR_A_gain = 0;
//bool doAlternateEvo;
//string sr_type = "None";

void setParsFromJson(json & j){
  //AVA_output = j["AVA_output"]["value"]; AVB_output = j["AVB_output"]["value"]; 
  doReverse =  j["doReverse"]["value"];
  //sr_type = j["SRType"]["value"];
  //doAlternateEvo = j["doAlternateEvo"]["value"];
}

void addParsToJson(json & j){
  //j["AVB_output"]["value"] = AVB_output; j["AVB_output"]["value"] = AVB_output;
  j["doReverse"]["value"] = doReverse;
  //j["SRType"]["value"] = sr_type;
  //j["doAlternateEvo"]["value"] = doAlternateEvo;
}

};

class EvolvableS
{
  public:
  
  virtual void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) = 0;
  virtual int getVectSize() = 0;
  //static int getVectSize();
  virtual ~EvolvableS(){}
  virtual void setParsFromPheno(TVector<double> &pheno) = 0;
  void setParsFromFile(const string & genofilename_);
  void setParsFromGeno(TVector<double> &geno);
  virtual void setEvolPars(W2Dparameters & w2par_, string evotype_) = 0;
  //virtual W2Dparameters & getWormPars() {return;}
  virtual void setWormPars(W2Dparameters & w2par_) {}

  //shared_ptr<W2Dparameters> evolvable_w2par_ptr;
  //Evolparameters & Epars1;
  //friend class EvolutionFull;
  //protected:
  //EvolvableS(shared_ptr<W2Dparameters> w2par_ptr_);

  
};



const char* getParameter(int argc, const char* argv[], string parName, const char* defaultval);
