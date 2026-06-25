#pragma once
#include <string>
#include <vector>
#include <sys/stat.h>
#include <iostream>
//#include <string.h>
//#include "VectorMatrix.h"
#include <iomanip> 
#include <fstream>
#include <assert.h>

using namespace std;





/* template<class T>
class TVecRef
{
    public:
    TVecRef(TVector<T> & v, int offset_):ptr(&v),offset(offset_){}

    T &operator()(int index){return ptr->(index + offset);}

    private:
    const int offset;
    shared_ptr<TVector<T> > ptr;
    //const TVector<T> & ref;
};


template<class T>
class TVecROff : private TVector<T>
{
    public:
    TVecROff(TVector<T> v, int offset_):offset(offset_),lb(v.lb),ub(v.ub),Vector(v.Vector){}

    T &operator()(int index){return (*this)(index + offset);}

    private:
    const int offset;
}; */

template<class T>
void getVecFromFile(const string & filename_, vector<T> & vec)
{

    ifstream ifs;
    ifs.open(filename_);
    T val;
    //vector<double> bestgenvec;
    while (ifs >> val) vec.push_back(val);
    ifs.close();

}

template<class T>
vector<T> fileGetCol(string name, int cols, int col_num = 0)
{
    ifstream file(name);

    vector<T> values;
    T x;

    while (file >> x) {
        values.push_back(x);
    }
    
    file.close();

    vector<T> colvalues;
    int colind = col_num;
    while(colind<values.size()){
    colvalues.push_back(values[colind]);
    colind += cols;
    }

return colvalues;

}

template<class T>
void fileDropLines(string name, int rows, int cols)
{
    vector<vector<T> > filevec;
  
    {ifstream file(name);
    
    for (int i = 0; i < rows; i++) 
    {
        vector<T> v;
        for (int j = 0; j < cols; j++) {
            T val;
            file >> val;
            v.push_back(val);
    }
    filevec.push_back(v);
    }
    file.close();
    }

    {ofstream file(name);
    file << setprecision(10);
    for (int i = 0; i < filevec.size(); i++) 
    {
        for (int j = 0; j < filevec[i].size(); j++) 
            file << filevec[i][j] << " ";
    file << endl;
    }
    file.close();}

}

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
ParamsHead():Params<T>(){head = "NULL";}
string head;
};

struct doubIntParamsHead
{
ParamsHead<double> parDoub;
ParamsHead<long> parInt;
};

// An entry in a sparse weight matrix

//struct jsonNamed {string nametag = "evotag";};
struct weightentry {int from; double weight;};
/* struct intPair : public jsonNamed {
    intPair(int ind_, int val_){ind=ind_;val=val_;}
    int ind; int val;}; */
struct intPair {int ind; int val;};
struct fromToInt {int from; int to; int val;};
struct intDoubDoub {int ind; double val1; double val2; string tag;};
struct stringPair {string s1; string s2;};
struct doubDoub {double val1; double val2;};
template<class T> struct namedVal {string name; T val;};
struct fromToStr {int from; int to; string evotag;};
struct strDoubDoub {string evotag; double val1; double val2;};
struct intStr {string evotag; int val;};
//struct intPairStr {int ind; int val;};

void push_back_double(const fromToInt & val, vector<fromToInt> & vec);




vector<intDoubDoub> toIntDoubDoub(const vector<doubDoub> & vec);
vector<doubDoub> todoubDoub(const vector<intDoubDoub> & vec);

template<class T> 
class namedValVec 
{

    public:
    bool setVal(const string & name, const T & val);
    const T & getVal(const string & name) const;
    T & getVal(const string & name);
    void show() const;

    private:
    vector<namedVal<T> > vec;
    
};

template<class T>
void namedValVec<T>::show() const
{
for (int i=0;i<vec.size();i++) cout << "nvv " << vec[i].name << " " << vec[i].val << endl;

}


template<class T>
const T & namedValVec<T>::getVal(const string & name) const
{
    for (int i=0;i<vec.size();i++)
        if (vec[i].name == name) return vec[i].val;
    cout << "getVal " << name << endl;
    assert(0);

}

template<class T>
T & namedValVec<T>::getVal(const string & name)
{
    for (int i=0;i<vec.size();i++)
        if (vec[i].name == name) return vec[i].val;
    cout << "getVal " << name << endl;
    assert(0);
}

template<class T>
bool namedValVec<T>::setVal(const string & name, const T & val)
{
    for (int i=0;i<vec.size();i++)
        if (vec[i].name == name) {vec[i].val = val;return true;}
    namedVal<T> nv;
    nv.name = name;
    nv.val = val;
    vec.push_back(nv);
    return false;
}


struct toFromWeight{

    toFromWeight(int to_val, int from_val, double weight){w.weight=weight;w.from=from_val;to=to_val;}
    toFromWeight(weightentry w_val, int to_val){w=w_val;to=to_val;}
    toFromWeight(const toFromWeight & tfw){w.weight=tfw.w.weight;w.from=tfw.w.from;to=tfw.to;}
    toFromWeight(){}
    weightentry w;
    int to;
};

struct toFromWeightLD{

    toFromWeightLD(int to_val, int from_val, long double weight_){weight=weight_;from=from_val;to=to_val;}
    toFromWeightLD(const toFromWeightLD & tfw){weight=tfw.weight;from=tfw.from;to=tfw.to;}
    
    //toFromWeight(){}

    long double weight;
    int to, from;
};


double angle_diff(double a, double b);




 bool check123456(const double & val, const double & val2);
 bool check123456(const double & val);
