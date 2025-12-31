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
void to_json(json & j, const fromToInt & w);
void from_json(const json& j, fromToInt & w);
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
   
  void addParsToJson(json &j) const;
  const doubIntParamsHead getParams() const;


  //void setFromArgs(int argc, const char* argv[]);
  //string rename_file(string filename);

string rename_file(string filename);
void setFromArgs(shared_ptr<const CmdArgs> cmd);
void setFromArgs(int argc, const char* argv[]);


};


double getParameterDouble(int argc, const char* argv[], string parName, const string defaultval);
long getParameterLong(int argc, const char* argv[], string parName, const string defaultval);
int getParameterInt(int argc, const char* argv[], string parName, const string defaultval);
string getParameterString(int argc, const char* argv[], string parName, const string defaultval);

string rename_file(const string & filename, const string & directoryName, const string & fileprefix = "");
bool directoryExists(const string & directoryName);



//const char* getParameter(int argc, const char* argv[], string parName, const char* defaultval);