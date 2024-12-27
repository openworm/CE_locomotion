#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <functional>
#include <vector>
#include <nlohmann/json.hpp>
//#include <bits/stdc++.h>
#include "Worm2D.h"
#include "Mainvars.h"


using json = nlohmann::json;


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

template <class T>
struct Params {
vector<string> names;
vector<T> vals;
vector<int> messages_inds;
vector<string> messages;
};


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

template<class T>
vector<T> & append(vector<T> & v1, const vector<T> v2)
{
v1.insert(v1.end(), v2.begin(), v2.end());
return v1;
}    

Params<double> getWormParams(Worm & w)
{

Params<double> par;
par.names = {"NMJ_DA", "NMJ_DB", "NMJ_VD", "NMJ_VB", "NMJ_VA", "NMJ_DD"};
par.vals = {w.NMJ_DA, w.NMJ_DB, w.NMJ_VD, w.NMJ_VB, w.NMJ_VA, w.NMJ_DD};
append<string>(par.names,{"AVA_act", "AVA_inact", "AVB_act", "AVB_inact"});
append<string>(par.names,{"AVA_output", "AVB_output"});
append<double>(par.vals,{w.AVA_act, w.AVA_inact, w.AVB_act, w.AVB_inact});
append<double>(par.vals,{w.AVA_output, w.AVB_output});

return par;

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

    os << endl;
    os << "Integration parameters" << endl;
    os << "skip_steps: " << skip_steps << endl;
    os <<  "Transient: " << Transient << endl; 
    os <<  "StepSize: " << StepSize << endl;
    os <<  "N_curvs: " << N_curvs << endl;

    os << endl;
    os << "Fitness traj" << endl;
    os <<  "AvgSpeed: " << AvgSpeed << endl;
    os <<  "BBCfit: " <<  BBCfit << endl;

    os << endl;
    os <<  "Genotype -> Phenotype Mapping Ranges" << endl;
    os <<  "BiasRange: " << BiasRange << endl;
    os <<  "SCRange: " << SCRange << endl;
    os <<  "CSRange: " << CSRange  << endl;
    os <<  "ESRange: "  << ESRange << endl;
    os <<  "SRmax: "  << SRmax << endl;
    os <<  "NMJmax: "  << NMJmax << endl;
    os <<  "NMJmin: "  << NMJmin << endl;

    os << endl;
    os << "Stretch receptor parameters" << endl;
    os << "SR_A :" << SR_A << endl;
    os << "SR_B :" << SR_B << endl;
 
    os << endl;
    os << "Size of genotype" << endl;
    os << "Vectsize :" << VectSize << endl;

    
    return os;
}




template<class T>
void appendToJson(json & j, const Params<T> & par)
{
    int mess_ind = 0;
    for (int i=0;i<par.names.size(); i++) {
        if (par.messages_inds.size()>mess_ind && par.messages_inds[mess_ind]==i) 
        {j[par.names[i]]["message"] = par.messages[i];mess_ind++;}
        j[par.names[i]]["value"] = par.vals[i];
        }
               
}


template<class T>
ostream& writeVectorFormat(ostream& os, 
const vector<string> & names, const vector<T> & vals, 
const vector<int> & messages_inds, const vector<string> & messages)
{
    int mess_ind = 0;
    os << setprecision(32);
    for (int i=0;i<names.size(); i++) {
        if (messages_inds.size()>mess_ind && messages_inds[mess_ind]==i) {os << messages[mess_ind] << endl;mess_ind++;}
        os << names[i] + ": " << vals[i] << endl;
        }

return os;        
        
}


template<class T>
ostream& writeVectorFormat(ostream& os, 
const vector<string> & names, const vector<T> & vals)
{
    os << setprecision(32);
    for (int i=0;i<names.size(); i++) {
        os << names[i] + ": " << vals[i] << endl;
        }

return os;        
        
}    
struct intString
{
int ind;
string std;
};

Params<double> getStretchReceptorParams(StretchReceptor& s)
{
Params<double> par;
par.names = {"NSR", "NSEGS", "NSEGSSR", "SR_A_gain", "SR_B_gain"};
par.vals = {s.NSR, s.NSEGS, s.NSEGSSR, s.SR_A_gain, s.SR_B_gain};
par.messages = {"Number of stretch receptor, equal to number of units",
                                "Number of segments in the body",
                                "Number of segments sensed by each stretch receptor"};
par.messages_inds = {0,1,2}; //must be ordered
return par;
}


ostream& writeStretchSysToFile(ostream& os, StretchReceptor& s)
{   
    
    os << setprecision(32);
    Params<double> par = getStretchReceptorParams(s);
    
 
    os << "StretchReceptor parameters" << endl;
    writeVectorFormat<double>(os,par.names,par.vals,par.messages_inds,par.messages);
    
return os;
}

Params<int> getBodyParamsInts(WormBody& b)
{
Params<int> par;
par.names = {"N_segments"};
par.vals = {N_segments};
par.messages = {"Number of segments"};
par.messages_inds  = {0};
return par;
}

Params<double> getBodyParams(WormBody& b)
{

Params<double> par;

par.names = {"Medium", "L_worm", "R_min", "C_agar_par_total", 
"C_agar_perp_total", "C_water_par_total", "C_water_perp_total", "kappa_L_fact", "kappa_D_fact", 
"kappa_M0_fact", "beta_L_fact", "beta_D_fact", "beta_M0_fact", "delta_M"};

par.vals = {Medium, L_worm, R_min, C_agar_par_total, C_agar_perp_total, 
C_water_par_total, C_water_perp_total, kappa_L_fact, kappa_D_fact, kappa_M0_fact, beta_L_fact, 
beta_D_fact, beta_M0_fact, delta_M};

par.messages_inds.resize(par.vals.size());

for (int i=0;i<par.messages_inds.size();i++) par.messages_inds[i]=i;

par.messages = {    
"Normalized medium drag coefficient (0 = water, 1 = agar)",
"Length of worm in m",
"Minor radius of prolate ellipse body in m",
"Total tangential drag coefficient for agar in kg/s",
"Total rod normal drag coefficient in agar in kg/s",
"Total rod tangential drag coefficient for water in kg/s",
"Total rod normal drag coefficient for water in kg/s",
"Lateral spring constant in kg/s",
"Diagonal spring constant in kg/s",
"Baseline active muscle spring constant in kg/s",
"Lateral passive damping constant in s",
"Diagonal passive damping constant in s",
"Baseline active damping constant in s",
"Rest muscle length scaling constant"

};

return par;

}



void writeParsToJson(Worm & w)
{

json j;
{Params<double> par = getBodyParams(w.b);
appendToJson<double>(j["body"],par);}

{Params<int> par = getBodyParamsInts(w.b);
appendToJson<int>(j["body"],par);}

{Params<double> par = getStretchReceptorParams(w.sr);
appendToJson<double>(j["stretch receptor"],par);
}
{
Params<double> par = getWormParams(w);
appendToJson<double>(j["worm"],par);
}

ofstream json_out(rename_file("worm_data.json"));
json_out << std::setw(4) << j << std::endl;
}


ostream& writeBodySysToFile(ostream& os, WormBody& b)
{   

os << "Worm Body parameters" << endl;    
{Params<double> par = getBodyParams(b);
writeVectorFormat<double>(os,par.names,par.vals,par.messages_inds,par.messages);
}

{Params<int> par = getBodyParamsInts(b);
writeVectorFormat<int>(os,par.names,par.vals,par.messages_inds,par.messages);
}

return os;
}

string output_dir_name = "";
string rename_file(const string & file_name){
  if (output_dir_name != "") return output_dir_name + "/" + file_name;
  return file_name;
}

void readJson(json j, ifstream & ifs)
{
ifs >> j;
}


void writeWormParams(Worm & w)
{
    {
    ifstream wormPheno; 
    wormPheno.open(rename_file("phenotype.dat"));
    setParamsFromDump(wormPheno, w);
    wormPheno.close();
    }
    {
    ofstream phenfile(rename_file("phenotype2.dat"));
    w.DumpParams(phenfile);
    phenfile.close();
    }
    {
    ofstream nv_file(rename_file("nv.dat"));
    nv_file << w.n;
    nv_file.close();
    }

    {
    ofstream nv_file(rename_file("w_verb.dat"));
    writeNSysToFile(nv_file, w.n);
    nv_file << endl;
    writeWSysToFile(nv_file, w);
    nv_file << endl;
    writeMuscSysToFile(nv_file, w.m);
    nv_file << endl;
    writeGlobalParsToFile(nv_file);
    nv_file << endl;
    writeStretchSysToFile(nv_file, w.sr);
    nv_file << endl;
    writeBodySysToFile(nv_file, w.b);
    nv_file.close();
    }
   
    {
    ifstream nv_file(rename_file("nv.dat"));
    nv_file >> w.n;
    nv_file.close(); 
    }

   /*  {
    ifstream nv_file(rename_file("nv.dat"));
    readJson(nv_file);
    nv_file.close(); 
    }
 */
    {
    ofstream nv_file(rename_file("nv2.dat"));
    nv_file << w.n;
    nv_file.close();
    }

    writeParsToJson(w);
   
}










 /*    os << "NSR: " << s.NSR << endl;
    os << "NSR: " << s.NSR << endl;
 
double NSR; // Number of stretch receptor, equal to number of units
    double NSEGS; // Number of segments in the body (50)
    double NSEGSSR; // Number of segments sensed by each stretch receptor
    
    double SR_A_gain;
    double SR_B_gain; */



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






