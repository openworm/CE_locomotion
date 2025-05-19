#pragma once
#include <string>
#include <vector>


using std::string;
using std::vector;

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


struct toFromWeight{
    
    toFromWeight(weightentry w_val, int to_val){w=w_val;to=to_val;}
    toFromWeight(){}
    weightentry w;
    int to;
};




