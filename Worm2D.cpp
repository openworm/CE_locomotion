
#include <iostream>
#include <sstream>
#include <string>
#include <functional>
#include <vector>
//#include <bits/stdc++.h>
#include "Worm2D.h"
#include "Mainvars.h"


using std::istringstream;
using std::string;
using std::cout;
using std::function;
using std::vector;



void invoke(ifstream &ifs, function<void(int, double)> calc, const vector<int> & v, int num = 1) {

double doub_value;
string textInput;
string str_value;
for (int i = 0; i < v.size(); i++) 
{

getline(ifs,textInput);
istringstream a_stream(textInput);
for (int j=0; j<num; j++) {a_stream >> str_value;}
a_stream >> doub_value;
calc(v[i],doub_value);

}
}

void invoke2(ifstream &ifs, function<void(int, int, double)> calc, const vector<int> & v) {

double doub_value;
string textInput;
string str_value;
for (int i = 0; i < v.size(); i++) {

getline(ifs,textInput);
istringstream a_stream(textInput);
a_stream >> str_value >> doub_value;
calc(v[i],v[i],doub_value);
}
}

//Worm2D::Worm2D(TVector<double> &v, double output):Worm(v,output){}


ifstream & setParamsFromDump(ifstream &ifs, Worm & w) {

   vector<int> v = {DA,DB,DD,VD,VA,VB}; 
   string textInput;
   

   {
   getline(ifs,textInput);
   getline(ifs,textInput);
   auto calc = bind(&NervousSystem::SetNeuronTimeConstant, &(w.n), placeholders::_1, placeholders::_2);
   invoke(ifs,calc,v);
   }
   { 
   getline(ifs,textInput);  
   getline(ifs,textInput);   
   auto calc = bind(&NervousSystem::SetNeuronBias, &(w.n), placeholders::_1, placeholders::_2);
   invoke(ifs,calc,v);
   }
   {
   getline(ifs,textInput);
   getline(ifs,textInput);   
   auto calc = bind(&NervousSystem::SetChemicalSynapseWeight, &(w.n), 
   placeholders::_1, placeholders::_2, placeholders::_3);
   invoke2(ifs,calc,v);
   }
 

   return ifs;

}

#include <iomanip>

ostream& writeNSysToFile(ostream& os, NervousSystem& c)
{
    // Set the precision
    os << setprecision(32);
    os << "size: " << c.size << endl;
    os << "maxchemcons: " << c.maxchemconns << endl;
    os << "maxelecconns: " <<  c.maxelecconns << endl << endl;
    //os << "size, maxchemconns and maxelecconns" << endl;
    // Write the size, maxchemconns and maxelecconns
    //os << c.size << " " << c.maxchemconns << " " << c.maxelecconns << endl << endl;
    // Write the time constants
    os << "time constants: " << endl;
    for (int i = 1; i <= c.size; i++)
        os << c.taus[i] << " ";
    os << endl << endl;
    os << "biases: " << endl;
    // Write the biases
    for (int i = 1; i <= c.size; i++)
        os << c.biases[i] << " ";
    os << endl << endl;
    // Write the gains
    os << "gains: " << endl;
    for (int i = 1; i <= c.size; i++)
        os << c.gains[i] << " ";
    os << endl << endl;
    os << "chemical weights: " << endl;
    // Write the chemical weights in sparse format (N from1 weight1 ... fromN weightN)
    for (int i = 1; i <= c.size; i++) {
        //cout << c.NumChemicalConns[i] << "  ";
        os << c.NumChemicalConns[i] << "  "; //fixed
        for (int j = 1; j <= c.NumChemicalConns[i]; j++)
            os << c.chemicalweights[i][j].from << " " << c.chemicalweights[i][j].weight << "  ";
        os << endl;
    }
    os << endl;
    os << "electrical weights: " << endl;
    // Write the electrical weights in sparse format (N from1 weight1 ... fromN weightN)
    for (int i = 1; i <= c.size; i++) {
        //cout << c.NumElectricalConns[i] << "  ";
        os << c.NumElectricalConns[i] << "  "; //fixed 
        for (int j = 1; j <= c.NumElectricalConns[i]; j++)
            os << c.electricalweights[i][j].from << " " << c.electricalweights[i][j].weight << "  ";
        os << endl;
    }
    // Return the ostream
    return os;
}


istream& readNSysFromFile(istream& is, NervousSystem& c)
{
    // Read the sizes
    int size;
    is >> size;
    int maxchemconns;
    is >> maxchemconns;
    int maxelecconns;
    is >> maxelecconns;
    c.SetCircuitSize(size, maxchemconns, maxelecconns);
    // Read the time constants
    for (int i = 1; i <= size; i++) {
        is >> c.taus[i];
        c.Rtaus[i] = 1/c.taus[i];
    }
    // Read the biases
    for (int i = 1; i <= size; i++)
        is >> c.biases[i];
    // Read the gains
    for (int i = 1; i <= size; i++)
        is >> c.gains[i];
    // Read the chemical weights
    int n;
    for (int i = 1; i <= size; i++) {
        is >> n;
        for (int j = 1; j <= n; j++) {
            is >> c.chemicalweights[i][j].from;
            is >> c.chemicalweights[i][j].weight;
            c.NumChemicalConns[i]++;
        }
    }
    // Read the electrical weights
    for (int i = 1; i <= size; i++) {
        is >> n;
        for (int j = 1; j <= n; j++) {
            is >> c.electricalweights[i][j].from;
            is >> c.electricalweights[i][j].weight;
            c.NumElectricalConns[i]++;
        }
    }
    // Return the istream
    return is;
}

ostream& writeMuscSysToFile(ostream& os, Muscles& m)
{   
    os << setprecision(32);
    os << "T_muscle: " << m.T_muscle << endl;
    os << "Nmuscles: " << m.Nmuscles << endl;
    return os;
}


ostream& writeWSysToFile(ostream& os, Worm& w)
{   
    
    os << setprecision(32);

    os << "Worm parameters" << endl;
    {
    vector<string> names = {"NMJ_DA", "NMJ_DB", "NMJ_VD", "NMJ_VB", "NMJ_VA", "NMJ_DD"};
    vector<double> vals = {w.NMJ_DA, w.NMJ_DB, w.NMJ_VD, w.NMJ_VB, w.NMJ_VA, w.NMJ_DD};

    for (int i=0; i<names.size(); i++)
    {
    os << names[i] + ": " << vals[i] << endl;
    }}
    os << endl;
    {
    vector<string> names = {"AVA_act", "AVA_inact", "AVB_act", "AVB_inact"};
    vector<double> vals = {w.AVA_act, w.AVA_inact, w.AVB_act, w.AVB_inact};

    for (int i=0; i<names.size(); i++)
    {
    os << names[i] + ": " << vals[i] << endl;
    }}
    os << endl;
    {
    vector<string> names = {"AVA_output", "AVB_output"};
    vector<double> vals = {w.AVA_output, w.AVB_output};

    for (int i=0; i<names.size(); i++)
    {
    os << names[i] + ": " << vals[i] << endl;
    }}
    return os;

}    

ostream& writeGlobalParsToFile(ostream& os)
{   
    os << setprecision(32);
    os << "Worm global parameters" << endl;

    os << "N_muscles: " << N_muscles << endl;
    os << "N_units: " << N_units << endl;
    os << "N_neuronsperunit: " << N_neuronsperunit << endl;
    os << "N_stretchrec: " << N_stretchrec << endl;
    os << "T_muscle: " << T_muscle << endl;
    os << "NmusclePerNU: " << NmusclePerNU << endl << endl;

    os << "Name conventions" << endl;
    vector<string> names = {"DA","DB","DD","VD","VA","VB","Head","Tail"};
    vector<int> vals = {DA,DB,DD,VD,VA,VB,Head,Tail};
    for (int i=0; i<names.size(); i++)
    {
    os << names[i] + ": " << vals[i] << endl;
    }

    os << "Algorithm global parameters" << endl << endl;
    os << "skip_steps: " << skip_steps << endl;
    os <<  "Transient: " << Transient << endl; 
    os <<  "StepSize: " << StepSize << endl;
    os <<  "N_curvs: " << N_curvs << endl;
    os <<  "AvgSpeed: " << AvgSpeed << endl;
    os <<  "BBCfit: " <<  BBCfit << endl;


    return os;
}

  /* 

int skip_steps = 10;
using namespace std;

// Integration parameters
int Duration = 24;
const double Transient = 8.0;
const double StepSize = 0.005;
const int N_curvs = 23;

// Fitness traj
const double    AvgSpeed = 0.0001; //0.00022;              // Average speed of the worm in meters per seconds
const double    BBCfit = AvgSpeed*Duration;

// Genotype -> Phenotype Mapping Ranges
const double    BiasRange               = 16.0; //15.0;
const double    SCRange                 = 16.0; //15.0;
const double    CSRange                 = 16.0; //15.0;
const double    ESRange                 = 2.0;
const double    SRmax                   = 200.0;
const double    NMJmax                  = 0.8; //1.2;
const double    NMJmin                  = 0.0;

const int SR_A = 1;
const int SR_B = 2;

// Size of genotype
int	VectSize = 17;  */






