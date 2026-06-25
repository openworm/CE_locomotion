#pragma once
#include "Worm.h"
#include <vector>
#include <string>
//#include "argUtils.h"
//#include <nlohmann/json.hpp>
#include "utils.h"
//extern SuppliedArgs supArgs1;

//using json = nlohmann::json;
using namespace std;
using wormForJson = Worm;

template<class T>
vector<T> & append(vector<T> & v1, const vector<T> & v2)
{
v1.insert(v1.end(), v2.begin(), v2.end());
return v1;
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


//ifstream & setParamsFromDump(ifstream &ifs, Worm<NervousSystem> &);
ifstream & setParamsFromDump(ifstream &ifs, wormForJson &);
ostream& writeNSysToFile(ostream& os, NervousSystem& c);
istream& readNSysFromFile(istream& is, NervousSystem& c);
ostream& writeWSysToFile(ostream& os,  wormForJson &);
ostream& writeMuscSysToFile(ostream& os, Muscles& m);
ostream& writeGlobalParsToFile(ostream& os);
ostream& writeStretchSysToFile(ostream& os, StretchReceptor& s);
ostream& writeBodySysToFile(ostream& os, WormBody& b);
ofstream& writeNSJson(ofstream & ofs, NervousSystem & n);
//NervousSystem getNSFromJsonFile(ifstream & ifs);
void setNSFromJsonFile(ifstream & ifs, NervousSystem & n);


void writeWormParams(wormForJson & w);
void writeParsToJson(wormForJson & w, string file_name, vector<doubIntParamsHead> & parvec);
//void writeParsToJson(json & j, wormForJson & w, string file_name);
//void writeParsToJson(wormForJson & w, long & randomseed);
void writeParsToJson(wormForJson &, string);
void writeParsToJson(wormForJson &);
void testNervousSystemJson(string, NervousSystem &);
Params<int> getNervousSysParamsIntNH(NervousSystem& c);
ParamsHead<int> getNervousSysParamsInt(NervousSystem& c);
Params< vector<int> > getNervousSysCellGroups(NervousSystem& c);
Params< vector<double> > getNervousSysParamsDoubleNH(NervousSystem& c);
Params< vector<int> > getNervousSysVecInt(NervousSystem& c);
Params< vector<string> > getNervousSysCellNames(NervousSystem& c);
ParamsHead< vector<double> > getNervousSysParamsDouble(NervousSystem& c);
Params<double> getMusclesParamsDouble(Muscles & m);
Params<int> getMusclesParamsInt(Muscles & m);
Params<double> getWormParams(wormForJson & w);
vector<ParamsHead<double> > getGlobalParamsDouble();
vector<ParamsHead<int> > getGlobalParamsInt();
Params<double> getStretchReceptorParams(StretchReceptor& s);




