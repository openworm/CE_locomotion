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

using json = nlohmann::json;
using std::vector;

json getJsonFromFile(const string & jsonfile_);



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
void getTVector(vector<T> & vec, TVector<T> & retvec)
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

vector<string> getCellNamesAll(const vector<string> & cell_names, int n_units);
void appendBodyToJson(json & j, WormBody& b);
void appendMuscleToJson(json & j, Muscles & m);
void appendAllNSJson(json & j, NervousSystem & n);
void appendAllNSJson(json & j, CTRNN & n);
void appendMatrixToJson(json & j, TMatrix<weightentry> & vec, TVector<int> & sizes, int tot_size);
//Params< vector<string> > getNervousSysCellNames(vector<string> & cell_names, int n_units);
//template<class T> void appendToJson(json & j, const Params<T> & par);
void appendCellNamesToJson(json & j, const vector<string> & cell_names, const int & num_reps);
