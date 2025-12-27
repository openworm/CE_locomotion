#pragma once
#include <vector>
#include "../VectorMatrix.h"
#include <nlohmann/json.hpp>
#include "../Muscles.h"
#include "../WormBody.h"
#include "../NervousSystem.h"
#include "CTRNN.h"
#include "../utils.h"
#include "NSToMuscles.h"
#include "../TSearch.h"


using json = nlohmann::json;
using std::vector;

json getJsonFromFile(const string & jsonfile_);

//#include <vector>
//#include <algorithm> // for std::transform

template<class T>
vector<T> multiply(const vector<T>& v, T scalar) {
    vector<T> result(v.size());
    transform(v.begin(), v.end(), result.begin(),
                   [scalar](T x) { return x * scalar; });
    return result;
}


template<class T>
T getJsonVal(const json & j, const string & key, const T & default_, bool doValue = false)
{

if (j.contains(key)){

        if (doValue) if (j[key].contains("value")) return j[key]["value"];
        return j[key];
 }
return default_;


}

template<class T>
vector<T> & append(vector<T> & v1, const vector<T> & v2)
{
v1.insert(v1.end(), v2.begin(), v2.end());
return v1;
}    

template<class T> 
vector<T> getVector(TVector<T> & vec)
{ 
const int size =  vec.Size();
vector<T> retvec;    
for (int i = 1; i <= size; i++)
        retvec.push_back(vec[i]);   
return retvec;    
}

template<class T> 
vector<T> getVector(TVector<T> & vec, int size)
{ 
vector<T> retvec;    
for (int i = 1; i <= size; i++)
        retvec.push_back(vec[i]);   
return retvec;    
}

template<class T> 
TVector<T> getTVector(vector<T> & vec)
{ 
TVector<T> retvec;
retvec.SetBounds(1,vec.size());    
for (int i = 0; i < vec.size(); i++) retvec[i+1]=vec[i];
return retvec;    
} 

template<class T> 
vector<T> TVectorRatio(const TVector<T> & top, const TVector<T> & bottom)
{
assert(top.Size() == bottom.Size() && "Vectors not same size");
vector<T> retvec;
for (int i = 1; i <= top.Size(); i++) retvec.push_back((top(i)-bottom(i))/bottom(i));
return retvec;    
}

template<class T> 
void getTVector(const vector<T> & vec, TVector<T> & retvec)
{ 
retvec.SetBounds(1,vec.size());    
for (int i = 0; i < vec.size(); i++) retvec[i+1]=vec[i];
} 

template<class T>
void appendVectorToJson(json & j, const vector<T> & vec)
{

     j["value"] = vec;

}


template<class T>
void appendToJson(json & j, const Params<T> & par)
{
    size_t mess_ind = 0;
    for (size_t i=0;i<par.names.size(); i++) {
        if (par.messages_inds.size()>mess_ind && par.messages_inds[mess_ind]==static_cast<int>(i)) 
        {j[par.names[i]]["message"] = par.messages[i];mess_ind++;}
        j[par.names[i]]["value"] = par.vals[i];
        }
               
}

void to_json(json & j, const weightentry & w);
void to_json(json & j, const toFromWeight & w);
void from_json(const json& j, toFromWeight & w);
void to_json(json & j, const intPair & w);
void from_json(const json& j, intPair & w);
void to_json(json & j, const toFromInt & w);
void from_json(const json& j, toFromInt & w);
void to_json(json & j, const intDoubDoub & w);
void from_json(const json& j, intDoubDoub & w);
void to_json(json & j, const doubDoub & w);
void from_json(const json& j, doubDoub & w);

template<class T>
vector<T> getEvoVecFromJ(const json & j, const string & name1_, const string & name2_)
{
return j[name1_][name2_]["evolvable"].template get< vector<T> >();
}

template<class T>
vector<T> getEvoVecFromJ(const json & j, const vector<string> & namevec_)
{
  json j2 = j;
  for (int i=0;i<namevec_.size(); i++) j2 = j2[namevec_[i]];
  return j2.template get< vector<T> >();
}

vector<string> getCellNamesAll(const vector<string> & cell_names, int n_units);
void appendBodyToJson(json & j, WormBody& b);
void appendMuscleToJson(json & j, Muscles & m);
void appendAllNSJson(json & j, NervousSystem & n);
void appendAllNSJson(json & j, CTRNN & n);
void appendMatrixToJson(json & j, TMatrix<weightentry> & vec, TVector<int> & sizes, int tot_size);
//Params< vector<string> > getNervousSysCellNames(vector<string> & cell_names, int n_units);
//template<class T> void appendToJson(json & j, const Params<T> & par);
void appendCellNamesToJson(json & j, const vector<string> & cell_names, const int & num_reps);
void setNSFromJson(const json & j, NervousSystem & n);



class CmdArgs {
    vector<string> args;
public:
    CmdArgs(int argc, const char* argv[]) 
        : args(argv, argv + argc) 
        {
          if (((argc-1) % 2) != 0)
         {cout << "The arguments are not configured correctly." << endl;exit(1);}
        }

    //int size() const { return static_cast<int>(args.size()); }

    //const string& operator[](int i) const { return args[i]; }

    //const vector<string>& all() const { return args; }

    const string getArgVal(const string & str, const  string & defaultstr) const
    { 
      for (int i = 1; i<args.size(); i+=2)
      //for (int i =0;i<args.size();i++)
        if (args[i]==str) return args[i+1];
      return defaultstr;
    }

    const int getArgVal(const string & str) const
    {
      for (int i = 1; i<args.size(); i+=2)
      //for (int i=0;i<args.size();i++)
        if (args[i]==str) return i;
      return -1;
    }

    const double getArgValDoub(const string & str, const double & val) const
    {
      const int arg = getArgVal(str);
      if (arg==-1) return val;
      return stod(args[arg+1].c_str());
    }

    const int getArgValInt(const string & str, const int & val) const
    {
      const int arg = getArgVal(str);
      if (arg==-1) return val;
      return stoi(args[arg+1].c_str());
    }

    const long getArgValLong(const string & str, const long & val) const
    {
      const int arg = getArgVal(str);
      if (arg==-1) return val;
      return stol(args[arg+1].c_str());
    }


    /* const double getArgValLong(const string & str, const string & defaultstr) const
    {
      return stol(getArgVal(str,defaultstr).c_str());
    }

    const double getArgValInt(const string & str, const string & defaultstr) const
    {
      return stoi(getArgVal(str,defaultstr).c_str());
    } */

};


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
   double Transient;    //
   double StepSize;
   int N_curvs ;
   int VectSize_temo ;
   string fileprefix  ;
   string evoType ;
   
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
         "CheckpointInterval", "ReEvaluationFlag", "skip_steps", "N_curvs", "VectSize_temo"};
    vector<int> vals = {(int) randomseed, SelectionMode, ReproductionMode, 
         PopulationSize, MaxGenerations, CrossoverMode, SearchConstraint, CheckpointInterval, 
         ReEvaluationFlag, skip_steps, N_curvs, VectSize_temo};
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
         "CheckpointInterval", "ReEvaluationFlag", "skip_steps", "N_curvs", "VectSize_temo"};
       var1.parInt.vals = {randomseed, SelectionMode, ReproductionMode, 
         PopulationSize, MaxGenerations, CrossoverMode, SearchConstraint, CheckpointInterval, 
         ReEvaluationFlag, skip_steps, N_curvs, VectSize_temo};

       return var1;
   }


  //void setFromArgs(int argc, const char* argv[]);
  //string rename_file(string filename);



string rename_file(string filename){return directoryName + "/" + 
    fileprefix + filename;}


void setFromArgs(shared_ptr<const CmdArgs> cmd)
{

bool seed_flag = 1;

MaxGenerations = cmd->getArgValInt("--maxgens", MaxGenerations);
MutationVariance = cmd->getArgValDoub("--MutVar", MutationVariance);
CrossoverProbability = cmd->getArgValDoub("--CrossProb", CrossoverProbability);
directoryName = cmd->getArgVal("--folder","HJUYGYT");
struct stat sb;
if (stat(directoryName.c_str(), &sb) != 0) 
{cout << "Directory doesn't exist." << endl;exit(1);}

PopulationSize = cmd->getArgValInt("-p",PopulationSize);
CheckpointInterval = cmd->getArgValInt("-cpt", CheckpointInterval);
Duration = cmd->getArgValDoub("-d", Duration);
Transient = cmd->getArgValDoub("-t", Transient);
evoType = cmd->getArgVal("--evoType", evoType);

if (seed_flag){ 
  long randomseed1 = cmd->getArgValLong("-R",-1);
  if (randomseed1!=-1)
  {seed_flag = 0;randomseed = randomseed1;}
}

if (seed_flag){ 
  long randomseed1 = cmd->getArgValLong("-r",-1);
  if (randomseed1!=-1)
  {seed_flag = 0;randomseed = randomseed1 + static_cast<long>(time(NULL));}
}


}

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
    if (strcmp(argv[arg],"--maxgens")==0) MaxGenerations = stoi(argv[arg+1]);

    if (strcmp(argv[arg],"--MutVar")==0) MutationVariance = stod(argv[arg+1]);
    if (strcmp(argv[arg],"--CrossProb")==0) CrossoverProbability = stod(argv[arg+1]);

    //if (strcmp(argv[arg],"--dorandinit")==0) simRandomInit = stol(argv[arg+1]);
    //if (strcmp(argv[arg],"--skipOrigSim")==0) skipOrigSim = stol(argv[arg+1]);
    //if (strcmp(argv[arg],"--donml")==0) do_nml = stol(argv[arg+1]);


    if (strcmp(argv[arg],"--folder")==0) {
      directoryName= argv[arg+1];
      struct stat sb;
      if (stat(directoryName.c_str(), &sb) != 0) 
      {cout << "Directory doesn't exist." << endl;exit(1);}
    }

    if (seed_flag){ 
    if (strcmp(argv[arg],"-R")==0) randomseed = stol(argv[arg+1]);
    if (strcmp(argv[arg],"-r")==0) 
    {long randomseed1 = static_cast<long>(time(NULL));
           randomseed = randomseed1 + stol(argv[arg+1]);
    }
    seed_flag = 0;
    }
    //if (strcmp(argv[arg], "--modelname")==0) model_name = argv[arg+1];
    if (strcmp(argv[arg],"-p")==0) PopulationSize = stoi(argv[arg+1]);
    if (strcmp(argv[arg],"-d")==0) Duration = stod(argv[arg+1]);
    if (strcmp(argv[arg],"-t")==0) Transient = stod(argv[arg+1]);
    if (strcmp(argv[arg],"-cpt")==0) CheckpointInterval = stoi(argv[arg+1]);
    if (strcmp(argv[arg],"--evoType")==0) evoType = (string) argv[arg+1];
    

    //cout << "doCPT " << doCPT << endl;
    //if (strcmp(argv[arg],"--nervous")==0) nervousSystemNameForSim = argv[arg+1];
    
    }

   

}

};


double getParameterDouble(int argc, const char* argv[], string parName, const string defaultval);
long getParameterLong(int argc, const char* argv[], string parName, const string defaultval);
int getParameterInt(int argc, const char* argv[], string parName, const string defaultval);
string getParameterString(int argc, const char* argv[], string parName, const string defaultval);

string rename_file(const string & filename, const string & directoryName, const string & fileprefix = "");
bool directoryExists(const string & directoryName);



//const char* getParameter(int argc, const char* argv[], string parName, const char* defaultval);