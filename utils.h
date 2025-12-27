#pragma once
#include <string>
#include <vector>
#include <sys/stat.h>
#include <iostream>
//#include <string.h>
//#include "VectorMatrix.h"
#include <iomanip> 
#include <fstream>


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

struct weightentry {int from; double weight;};
struct intPair {int ind; int val;};
struct toFromInt {int to; int from; int val;};

struct toFromWeight{
    
    toFromWeight(weightentry w_val, int to_val){w=w_val;to=to_val;}
    toFromWeight(){}
    weightentry w;
    int to;
};

double angle_diff(double a, double b);




