#pragma once
#include <vector>
#include "../VectorMatrix.h"
#include <nlohmann/json.hpp>
#include "../Muscles.h"
#include "../WormBody.h"
#include "../NervousSystem.h"
//#include "CTRNN.h"
#include "../utils.h"
#include "NSToMuscles.h"
#include "../TSearch.h"
#include <type_traits>

 

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
bool getJsonValTF(const json & j, const string & key, T & val, bool doValue = false)
{

if (j.contains(key)){
        if (doValue) {if (j[key].contains("value")) val = j[key]["value"];
        return true;}
        else {val = j[key]; return true;}
 }
 
return false;
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
vector<T> getVector(const TVector<T> & vec, int size)
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
void sortAsc(vector<weightentry> & entries);
std::vector<std::string> makeUnique(std::vector<std::string> v);
std::vector<std::string> removeSuffixIndices(std::vector<std::string> v);

bool same_values_unordered(
    std::vector<double> a,
    std::vector<double> b,
    double tol = 1e-12
);

void compare_numeric_values(
    const json& j1,
    const json& j2,
    const std::string& path = "",
    double tol = 1e-12
);
void collect_numeric_differences(
    const json& j1,
    const json& j2,
    std::vector<double>& diffs,
    const std::string& path = "",
    double tol = 1e-12
);
void from_json(const json & j, weightentry & w);
void to_json(json & j, const weightentry & w);
void to_json(json & j, const toFromWeight & w);
void from_json(const json& j, toFromWeight & w);
void to_json(json & j, const intPair & w);
void from_json(const json& j, intPair & w);
void to_json(json & j, const fromToInt & w);
void from_json(const json& j, fromToInt & w);
void to_json(json & j, const intDoubDoub & w);
void from_json(const json& j, intDoubDoub & w);
json toEvolvableRangesJson(const vector<intDoubDoub> & ranges);
json toEvolvableRangesJson(const vector<doubDoub> & ranges);
json toEvolvedUsedJson(const vector<intDoubDoub> & ranges);
json toEvolvedUsedJson(const vector<doubDoub> & ranges);
void to_json(json & j, const doubDoub & w);
void from_json(const json& j, doubDoub & w);
void splitWeightEntry(const vector<weightentry> & w, vector<int> & ind, vector<double> & weight);
void to_json(json & j, const fromToStr & w);
void from_json(const json & j, fromToStr & w);
void to_json(json & j, const intStr & w);
void from_json(const json & j, strDoubDoub & w);
void to_json(json & j, const strDoubDoub & w);
void from_json(const json & j, intStr & w);
json to_evo_json(const vector<intPair> & w);
vector<intPair> from_evo_json(const json& j);




/* template<class T>
bool checkType(const json & j, const string & name)
{
  //if (auto p = j.at("name").get_ptr<const vector<T>* >()) return true;
  //return false;
  return (j.at(name).get_ptr<const T*>()!=nullptr); 
}

template<class T>
bool checkType(const json & j)
{

  //if (auto p = j.get_ptr<const vector<T>* >()) return true;
  //return false;

  return (j.get_ptr<const T*>()!=nullptr); 
}
 */
vector<weightentry> makeWeightEntry(const vector<int> & ind, const vector<double> & weight);

template<class T>
vector<T> getEvoVecFromJ(const json & j, const string & name1_, const string & name2_)
{
return j[name1_][name2_]["evolvable"].template get< vector<T> >();
}

template<class T>
vector<T> getEvoVecFromJ(const json & j, const vector<string> & namevec_)
{
  json j2 = j;
  for (int i=0;i<namevec_.size(); i++) 
  {if (!j2.contains(namevec_[i])) {cout << "eer " << namevec_[i] << endl; assert(0);}
    j2 = j2[namevec_[i]];}
  return j2.template get< vector<T> >();
}

template<class T>
bool getEvoVecFromJ(const json & j, const vector<string> & namevec_, vector<T> & vec)
{
  json j2 = j;
  for (int i=0;i<namevec_.size(); i++) 
  {
    if (!j2.contains(namevec_[i])) return false;
    j2 = j2[namevec_[i]];
  }
  vector<T> jvec = j2.template get< vector<T> >();
  vec.swap(jvec);
  return true;
}

template<class T>
T getEvoValFromJ(const json & j, const vector<string> & namevec_)
{
  json j2 = j;
  for (int i=0;i<namevec_.size(); i++) 
  {
    if (!j2.contains(namevec_[i])) return false;
    j2 = j2[namevec_[i]];
  }
  return j2;
}

template<class T>
bool getEvoValFromJ(const json & j, const vector<string> & namevec_, T & val)
{
  json j2 = j;
  for (int i=0;i<namevec_.size(); i++) 
  {
    if (!j2.contains(namevec_[i])) return false;
    j2 = j2[namevec_[i]];
  }
  val = j2;
  return true;
}


void addMfuncTFI(json & j, const fromToInt & val, const vector<string> & cell_names_full, const json & j2);
void compareTFWV(const vector<toFromWeight> & vec1, const vector<toFromWeight> & vec2, const string & tag);
vector<toFromWeight> getToFromWeightVec(const json & j, const string & topar, 
  const string & frompar, const string & weightpar, const unordered_map<string, int> & name_index);
vector<toFromWeight> getToFromWeightVec(const json & j, const string & topar, 
  const string & frompar, const string & weightpar);
void appendNSToJsonByCell(json & j, NervousSystem& n);
void setCircuitSize(const json & j, NervousSystem& n);
int getMaxCounts(const json & j, const vector<string> & names, const string & topar);
void addToFromWeight(json & j, const vector<toFromWeight> & vec, const string & topar, 
  const string & frompar, const string & weightpar, const vector<string> & names);
void addToFromWeight(json & j, const vector<toFromWeight> & vec, const string & topar, 
  const string & frompar, const string & weightpar);
void addWeightentry(json & j, const vector<weightentry> & vec, const string & frompar, const string & weightpar);
void addEvolvableIP(json & j, vector<intPair> & vec, const string & parameter, 
  const vector<string> & cell_names_full);
void addEvolvableTFI(json & j, const vector<fromToInt> & vec, const vector<string> & cell_names_full,
  bool reciprocal = false);
vector<string> getCellNamesUnits(const vector<string> & cell_names, int n_units);
vector<toFromWeight> getNSToFromVec(TMatrix<weightentry> & vec, TVector<int> & sizes, int tot_size);
void appendNSToJsonByCell(json & j, NervousSystem& n, const vector<string> & cell_names,
  const vector<string> & section_names = vector<string>());
void appendNSCellClassesToJson(json & j, const vector<string> & section_names);
void set_nested_json(json & j, const vector<string> & keys, const json & value);
void setNSFromJsonNZ(const json & j, NervousSystem & n, const bool setStates = true);
void setNSFromJson(const json & j, NervousSystem & n, const bool setStates = true);
vector<string> getCellNamesAll(const vector<string> & cell_names, int n_units);
void appendBodyToJson(json & j, WormBody& b);
void appendMuscleToJson(json & j, Muscles & m);
void appendAllNSJson(json & j, NervousSystem & n);
//void appendAllNSJson(json & j, CTRNN & n);
void appendMatrixToJson(json & j, TMatrix<weightentry> & vec, TVector<int> & sizes, int tot_size);
//Params< vector<string> > getNervousSysCellNames(vector<string> & cell_names, int n_units);
//template<class T> void appendToJson(json & j, const Params<T> & par);
void appendCellNamesToJson(json & j, const vector<string> & cell_names, const int & num_reps);
void mergeJson(json & j1, const json & j2);
void appendNSToJson(json & j, NervousSystem& c);
void appendChemNSToJson(json & j, NervousSystem& c);
void appendElecNSToJson(json & j, NervousSystem& c);

bool parseValue(const std::string& s, double& v);
bool parseValue(const std::string& s, int&    v);
bool parseValue(const std::string& s, long&   v);
bool parseValue(const std::string& s, std::string& v);
bool parseValue(const std::string& s, bool& v);
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


    /* bool getArgValT(const string & str, string & val) const
    {
      
      const int arg = getArgVal(str);
      if (arg==-1) return false;
      val = args[arg+1].c_str();


      cout << "zospr " << str << " sd " << val << endl;
      return true;

    } */
 


  template<class T>
  bool getArgValT(const std::string& str, T& val) const
  {
    const int arg = getArgVal(str);
    if (arg == -1) return false;

    const std::size_t i = static_cast<std::size_t>(arg) + 1;
    if (i >= args.size()) return false;

    const std::string& s = args[i];

    /* if constexpr (!std::is_same_v<T, std::remove_reference_t<T>>) {
        // optional: normalize refs/cv if you pass those around
    } */

    // If no overload matches, you'll get a clear compile error.

    bool result = parseValue(s, val);

    //cout << "popil " << str << " sdss " << val << endl;
    return result;
  }


/* 
    template<class T>
    bool getArgValT(const std::string& str, T& val) const
    {

    const int arg = getArgVal(str);
    if (arg == -1) return false;

    const std::size_t i = static_cast<std::size_t>(arg) + 1;
    if (i >= args.size()) return false; // no value after the flag

    const std::string& s = args[i];

    using U = std::remove_cv_t<std::remove_reference_t<T>>;

    if constexpr (std::is_same_v<U, double>) {
        val = std::stod(s);                 // stod takes std::string
    } else if constexpr (std::is_same_v<U, std::string>) {
        val = s;                            // simplest
    } else if constexpr (std::is_same_v<U, int>) {
        val = std::stoi(s);
    } else if constexpr (std::is_same_v<U, long>) {
        val = std::stol(s);
    } else if constexpr (std::is_same_v<U, bool>) {
        // accept 0/1/true/false (case-insensitive)
        auto lower = [](unsigned char c){ return static_cast<char>(std::tolower(c)); };
        std::string t; t.reserve(s.size());
        for (unsigned char c : s) t.push_back(lower(c));

        if (t == "1" || t == "true" || t == "yes" || t == "on")      val = true;
        else if (t == "0" || t == "false" || t == "no" || t == "off") val = false;
        else return false; // not a valid bool
    } else {
        static_assert(std::is_same_v<U, void>,
                      "Unsupported T in getArgValT: use double, string, int, long, bool");
    }

    return true;
  }

 */

/* 
    template<class T>
    bool getArgValT(const string & str, T & val) const
    {
 

      const int arg = getArgVal(str);
      if (arg==-1) return false;
     
    using U = std::remove_cv_t<std::remove_reference_t<T>>;

    if constexpr (std::is_same_v<U, double>) {
      val = stod(args[arg+1].c_str());
        //std::cout << "double path: " << (x * 2.0) << "\n";
    } else if constexpr (std::is_same_v<U, std::string>) {
      //val = args[arg+1].c_str();
      val = args[arg+1];
        //std::cout << "string path: " << x.size() << " chars\n";
    }
    else if constexpr (std::is_same_v<U, std::int>) {
      val = stoi(args[arg+1].c_str());
    }
    else if constexpr (std::is_same_v<U, std::long>) {
      val = stol(args[arg+1].c_str());
    }
    else if constexpr (std::is_same_v<U, std::bool>) {
       val = stoi(args[arg+1].c_str());
    }
    else 
    { assert(0);}

    return true;

  }



    template<class T>
    bool getArgValT_v2(const string & str, T & val) const
    {

      const int arg = getArgVal(str);
      if (arg==-1) return false;


      //if (std::is_same<T, string>::value) val = args[arg+1];
      
      else if (std::is_same<T, double>::value) {
        val = stod(args[arg+1].c_str());
        cout << "doubl-" << str << " doublval-" << val << endl;
      }
      
      else if (std::is_same<T, int>::value) 
      {val = stoi(args[arg+1].c_str());
        cout << "int-" << str << " intval-" << val << endl;
      }
      
      else if (std::is_same<T, long>::value) val = stol(args[arg+1].c_str());
      
      else if (std::is_same<T, bool>::value) {
        val = stoi(args[arg+1].c_str());
        cout << "bool-" << str << " boolval-" << val << endl;
      }
      
      else {assert(0);}
    //cout << "popil " << str << " sd " << val << endl;

      return true;

    }
 */
    const string getArgVal(const string & str, const  string & val) const
    { 
      for (int i = 1; i<args.size(); i+=2)
      //for (int i =0;i<args.size();i++)
        if (args[i]==str) return args[i+1];
      return val;
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
