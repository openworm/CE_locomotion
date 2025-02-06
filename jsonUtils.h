#include "Worm.h"
#include <vector>
#include <string>
#include "utils.h"

#pragma once

using namespace std;

template <class T>
struct Params {
Params(){}    
vector<string> names;
vector<T> vals;
vector<int> messages_inds;
vector<string> messages;
};

template <class T>
struct ParamsHead : Params<T> {
ParamsHead(string head_val, Params<T> par_val):Params<T>(par_val){head=head_val;}
ParamsHead():Params<T>(){}
string head;
};


ifstream & setParamsFromDump(ifstream &ifs, Worm &);
ostream& writeNSysToFile(ostream& os, NervousSystem& c);
istream& readNSysFromFile(istream& is, NervousSystem& c);
ostream& writeWSysToFile(ostream& os, Worm& w);
ostream& writeMuscSysToFile(ostream& os, Muscles& m);
ostream& writeGlobalParsToFile(ostream& os);
ostream& writeStretchSysToFile(ostream& os, StretchReceptor& s);
ostream& writeBodySysToFile(ostream& os, WormBody& b);
ofstream& writeNSJson(ofstream & ofs, NervousSystem & n);
//NervousSystem getNSFromJsonFile(ifstream & ifs);
void setNSFromJsonFile(ifstream & ifs, NervousSystem & n);


void writeWormParams(Worm & w);

void writeParsToJson(Worm &, string);
void writeParsToJson(Worm &);
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
Params<double> getWormParams(Worm & w);
vector<ParamsHead<double> > getGlobalParamsDouble();
vector<ParamsHead<int> > getGlobalParamsInt();
Params<double> getStretchReceptorParams(StretchReceptor& s);




