#include "Worm.h"
#include <nlohmann/json.hpp>
#include <vector>
#include "jsonUtils.h"
#include <iomanip>
#include <cctype>
#include "../argUtils.h"

extern SuppliedArgs2018 supArgs1;

using std::vector;
using json = nlohmann::json;

static string normaliseJsonFieldName(string name)
{
  for (char & c : name) {
    if (c == ' ') c = '_';
    else c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  return name;
}

template<class T>
static Params<T> normaliseParamNames(Params<T> par)
{
  for (string & name : par.names) name = normaliseJsonFieldName(name);
  return par;
}




void to_json(json & j, const weightentry & w)
{
  j = json{{"from", w.from}, {"weight", w.weight}};
}

void to_json(json & j, const toFromWeight & w)
{
  j = json{{"to", w.to}, {"from", w.w.from}, {"weight", w.w.weight}};
}

void from_json(const json& j, toFromWeight & w) 
{
        j.at("to").get_to(w.to);
        j.at("from").get_to(w.w.from);
        j.at("weight").get_to(w.w.weight);
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

void appendMatrixToJson(json & j, TMatrix<weightentry> & vec, TVector<int> & sizes, int tot_size)
{    
    vector<toFromWeight> newvec;
    for (int i=1; i<=tot_size; i++){    
        for (int j=1; j<=sizes[i]; j++) { 
            toFromWeight tv(vec[i][j], i);
            newvec.push_back(tv);}        
    }
    j["value"] = newvec;

}
void appendNSToJson(json & j, NervousSystem& c)
{
    j["Chemical weights"]["message"] = "chemical weights in sparse format";
    appendMatrixToJson(j["Chemical weights"], c.chemicalweights, c.NumChemicalConns, c.size);
    appendMatrixToJson(j["Electrical weights"], c.electricalweights, c.NumElectricalConns, c.size);
    j["Electrical weights"]["message"] = "electrical weights in sparse format";
}

Params< vector<double> > getNervousSysParamsDoubleNH(NervousSystem& c)
{
Params< vector<double> > par;
par.names = {"taus", "biases", "gains", "outputs", "states", "paststates", "Rtaus", "externalinputs"};
par.vals = {
getVector<double>(c.taus, c.size), 
getVector<double>(c.biases, c.size), 
getVector<double>(c.gains, c.size),
getVector<double>(c.outputs, c.size),
getVector<double>(c.states, c.size),
getVector<double>(c.paststates, c.size),
getVector<double>(c.Rtaus, c.size),
getVector<double>(c.externalinputs, c.size),
};
return par;
}




// Neuron name conventions



Params< vector<string> > getNervousSysCellNames(vector<string> & cell_names, int neurons_per_unit)
{
Params< vector<string> > par;
par.names = {"Cell name"};
vector<string> cell_names_all;
for (int i=0;i<neurons_per_unit;i++) cell_names_all.insert(cell_names_all.end(),cell_names.begin(),cell_names.end());
par.vals = {cell_names_all};
return par;
}




Params<int> getNervousSysParamsIntNH(NervousSystem& c)
{
Params<int> par;    
par.names = {"size", "maxchemcons", "maxelecconns"};
par.vals = {c.size, c.maxchemconns, c.maxelecconns};
return par;
}

Params< vector<int> > getNervousSysVecInt(NervousSystem& c)
{
Params< vector<int> > par;
par.names = {"NumChemicalConns", "NumElectricalConns"};
par.vals = {
getVector<int>(c.NumChemicalConns, c.size), 
getVector<int>(c.NumElectricalConns, c.size), 
};
return par;
}


void getNSJson(NervousSystem & n, json & j)
{

{Params<vector<double> > parvec = getNervousSysParamsDoubleNH(n);
appendToJson<vector<double> >(j,parvec);}
        
{Params<int> parvec = getNervousSysParamsIntNH(n);
appendToJson<int>(j,parvec);}

{Params< vector<int> > parvec = getNervousSysVecInt(n);
appendToJson<vector<int> >(j,parvec);}

appendNSToJson(j, n);

}


Params<double> getStretchReceptorParams(StretchReceptor& s)
{
Params<double> par;
par.names = {"NSR", "NSEGS", "NSEGSSR", "VNC_gain", "Head_gain"};
par.vals = {s.NSR, s.NSEGS, s.NSEGSSR, s.SRvncgain, s.SRheadgain};
par.messages = {"Number of stretch receptor",
                                "Number of segments in the body",
                                "Number of segments sensed by each stretch receptor"};
par.messages_inds = {0,1,2}; //must be ordered
return par;
}

Params<double> getWormParams(Worm & w)
{
Params<double> par;
par.vals = {w.NMJ_DB, w.NMJ_VBa, w.NMJ_VBp, w.NMJ_DD, w.NMJ_VDa, w.NMJ_VDp, 
    w.NMJ_SMDD, w.NMJ_SMDV, w.NMJ_RMDD, w.NMJ_RMDV, w.NMJ_Gain_Map};
par.names = {"NMJ_DB", "NMJ_VBa", "NMJ_VBp", "NMJ_DD", "NMJ_VDa", "NMJ_VDp",
    "NMJ_SMDD", "NMJ_SMDV", "NMJ_RMDD", "NMJ_RMDV", "NMJ Gain"};
return par;
}

Params<double> getBodyParams(WormBody& b)
{

Params<double> par;

par.names = {"Medium", "L_worm", "R_min", "C_agar_par_total", 
"C_agar_perp_total", "C_water_par_total", "C_water_perp_total", "kappa_L", "kappa_D", 
"kappa_M0", "beta_L", "beta_D", "beta_M0", "delta_M"};

par.vals = {Medium, L_worm, R_min, C_agar_par_total, C_agar_perp_total, 
C_water_par_total, C_water_perp_total, kappa_L, kappa_D, kappa_M0, beta_L, 
beta_D, beta_M0, delta_M};

par.messages_inds.resize(par.vals.size());

for (size_t i=0;i<par.messages_inds.size();i++) par.messages_inds[i]=i;

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

Params<int> getBodyParamsInts(WormBody& b)
{
Params<int> par;
par.names = {"N_segments"};
par.vals = {N_segments};
par.messages = {"Number of body segments on each side, dorsal and ventral"};
par.messages_inds  = {0};
return par;
}

Params<double> getMusclesParamsDouble(Muscles & m)
{
Params<double> par;
par.names = {"T_muscle"};
par.vals = {m.T_muscle};
return par;
}

Params<int> getMusclesParamsInt(Muscles & m)
{
Params<int> par;
par.names = {"Nmuscles"};
par.vals = {m.Nmuscles};
return par;
}

/* vector<ParamsHead<double> > getGlobalParamsDoubleCeloc()
{
vector<ParamsHead<double> > parvec;
{ParamsHead<double> par;
par.head = "Worm global parameters";
par.names = {"T_muscle"};
par.vals = {T_muscle};
par.messages ={"Muscle time constant"};
par.messages_inds = {0};
parvec.push_back(par);}
{ParamsHead<double> par;
par.head =  "Integration parameters";
par.names =  {"Transient", "StepSize"};
par.vals = {Transient, StepSize};
parvec.push_back(par);}
{ParamsHead<double> par;
par.head =  "Fitness traj";
par.names =  {"AvgSpeed", "BBCfit"};
par.vals = {AvgSpeed, BBCfit};
par.messages ={"Average speed of the worm in meters per second", "AvgSpeed*Duration"};
par.messages_inds = {0,1};
parvec.push_back(par);}
{ParamsHead<double> par;
par.head =  "Genotype -> Phenotype Mapping Ranges";
par.names =  {"BiasRange", 
     "SCRange", 
     "CSRange", 
     "ESRange",  
      "SRmax",  
      "NMJmax",  
      "NMJmin"};  
par.vals = {BiasRange, SCRange, CSRange, ESRange, SRmax, NMJmax, NMJmin}; 
parvec.push_back(par);}

return parvec;
}

vector<ParamsHead<int> > getGlobalParamsIntCeloc()
{
vector<ParamsHead<int> > parvec;
{ParamsHead<int> par;
par.head = "Worm global parameters";
par.names = { "N_muscles", "N_units", "N_neuronsperunit", "N_stretchrec", "NmusclePerNU"};
par.vals = {N_muscles, N_units, N_neuronsperunit, N_stretchrec, NmusclePerNU};
par.messages = {"Number of muscles on each side of the body (ventral and dorsal)", 
"Number of neural units", "Number of neurons in a neural unit",  
"Number of stretch receptors in each of dorsal A, ventral A, dorsal B and ventral B", 
"Number of muscles per neural unit on each side (dorsal and ventral)"};
par.messages_inds = {0,1,2,3,4};
parvec.push_back(par);}
{ParamsHead<int> par;
par.head = "Name conventions";
par.names =  {"DA","DB","DD","VD","VA","VB","Head","Tail"};
par.vals = {DA,DB,DD,VD,VA,VB,Head,Tail};
parvec.push_back(par);}
{ParamsHead<int> par;
par.head =  "Integration parameters";
par.names =  {"skip_steps", "N_curvs"};
par.vals = {skip_steps, N_curvs};
parvec.push_back(par);}
{ParamsHead<int> par;
par.head =  "Stretch receptor parameters";
par.names =  {"SR_A", "SR_B"};
par.vals = {SR_A, SR_B};
parvec.push_back(par);}
{ParamsHead<int> par;
par.head =  "Worm global parameters";
par.names =  {"Vectsize"};
par.vals = {VectSize};
par.messages = {"Size of genotype"};
par.messages_inds = {0}; 
parvec.push_back(par);}

return parvec;
} */



void writeParsToJson(Worm & w)
{
    if (supArgs1.doOrigNS){

        json j;

        cout << "making json" << endl;
        vector<doubIntParamsHead> parvec;
        doubIntParamsHead var1 = supArgs1.getParams();
        parvec.push_back(var1);

        for (size_t i=0;i<parvec.size(); i++) {
            if (strcmp(parvec[i].parDoub.head.c_str(),"NULL")!=0)
            appendToJson<double>(j[parvec[i].parDoub.head],parvec[i].parDoub);
            if (strcmp(parvec[i].parInt.head.c_str(),"NULL")!=0)
            appendToJson<long>(j[parvec[i].parInt.head],parvec[i].parInt);
            }

       
            {string nsHead = "Nervous system";
            getNSJson(w.n, j[nsHead]);
            vector<string> cell_names = {"DB", "DD", "VBA", "VDA", "VBP", "VDP"};
            {Params< vector<string> > parvec = getNervousSysCellNames(cell_names, 6);
            appendToJson<vector<string> >(j[nsHead],parvec);}}
            
            {string nsHead = "Head Nervous system";
            getNSJson(w.h, j[nsHead]);
            vector<string> cell_names = {"SMDD", "RMDD", "SMDV", "RMDV"};
            {Params< vector<string> > parvec = getNervousSysCellNames(cell_names, 1);
            appendToJson<vector<string> >(j[nsHead],parvec);}}   
        
        {Params<double> par = getWormParams(w);
        appendToJson<double>(j["Worm"],par);}

        {Params<double> par = getStretchReceptorParams(w.sr);
        appendToJson<double>(j["Stretch receptor"],par);}

        {Params<double> par = getBodyParams(w.b);
        appendToJson<double>(j["body"], normaliseParamNames(par));}

        {Params<int> par = getBodyParamsInts(w.b);
            appendToJson<int>(j["body"], normaliseParamNames(par));}

            {
            Params<double> par =getMusclesParamsDouble(w.m);
            appendToJson<double>(j["Muscle"],par);
            }
            {
            Params<int> par =getMusclesParamsInt(w.m);
            appendToJson<int>(j["Muscle"],par);
            }

            /* {vector<ParamsHead<int> > parvec = getGlobalParamsInt();
                for (size_t i=0;i<parvec.size(); i++) {
                appendToJson<int>(j[parvec[i].head],parvec[i]);
                }}
                {vector<ParamsHead<double> > parvec = getGlobalParamsDouble();
                for (size_t i=0;i<parvec.size(); i++) {
                appendToJson<double>(j[parvec[i].head],parvec[i]);
                }}

 */
        ofstream json_out(supArgs1.rename_file("worm_data.json"));
        json_out << std::setw(4) << j << std::endl;
        json_out.close();


    }
}


/* 

void DumpParamsOrig(ofstream &ofs)
{
    ofs << "Time-constants: \n DB: " << n.NeuronTimeConstant(DB) << "\n VBA/P: " << n.NeuronTimeConstant(VBA) << " / " << n.NeuronTimeConstant(VBP) << "\n DD: " << n.NeuronTimeConstant(DD) << "\n VDA/P: " << n.NeuronTimeConstant(VDA) << " / " << n.NeuronTimeConstant(VDP) << endl;
    ofs << "Biases: \n DB: " << n.NeuronBias(DB) << "\n VBA/P: " << n.NeuronBias(VBA) << " / " << n.NeuronBias(VBP)  <<  "\n DD: " << n.NeuronBias(DD) << "\n VDA/P: " << n.NeuronBias(VDA) <<  " / " << n.NeuronBias(VDP) << endl;
    ofs << "Self conns: \n DB: " << n.ChemicalSynapseWeight(DB, DB) << "\n VBA/P: " << n.ChemicalSynapseWeight(VBA, VBA) << " / " << n.ChemicalSynapseWeight(VBP, VBP) << "\n DD: " << n.ChemicalSynapseWeight(DD, DD) <<  "\n VDA/P: " << n.ChemicalSynapseWeight(VDA, VDA) <<  " / " << n.ChemicalSynapseWeight(VDP, VDP) << endl;
    ofs << "Chem Conns: \n DB->DD: " << n.ChemicalSynapseWeight(DB, DD) <<  "\n DB->VDA/VDP: " << n.ChemicalSynapseWeight(DB, VDA) << " / " << n.ChemicalSynapseWeight(DB, VDP) << "\n VBA/P->DD: " << n.ChemicalSynapseWeight(VBA, DD) << " / " << n.ChemicalSynapseWeight(VBP, DD) << "\n VBA/P->VDA/P: " << n.ChemicalSynapseWeight(VBA, VDA) << " / " << n.ChemicalSynapseWeight(VBP, VDP) << "\n VDA/P->VBA/P: " << n.ChemicalSynapseWeight(VDA, VBA) << " / " << n.ChemicalSynapseWeight(VDP, VBP) << "\n DD->VDA: " << n.ChemicalSynapseWeight(DD, VDA) <<endl;
    ofs << "Gap Juncs: \n DB-DB+1: " << n.ElectricalSynapseWeight(DB, DB+N_neuronsperunit) << "\n VBA-VBP / VBP-VBP+1: " << n.ElectricalSynapseWeight(VBA, VBP) << " / " << n.ElectricalSynapseWeight(VBP, VBA+N_neuronsperunit) << "\n VBP-DB+1: " << n.ElectricalSynapseWeight(VBP, DB+N_neuronsperunit) << "\n DD-VDA/P: " << n.ElectricalSynapseWeight(DD, VDA) << " / " << n.ElectricalSynapseWeight(DD, VDP) << "\n DD-DD+1: " << n.ElectricalSynapseWeight(DD, DD+N_neuronsperunit) << "\n VDA-VDP / VDP-VDP+1: " << n.ElectricalSynapseWeight(VDA, VDP) << " / " << n.ElectricalSynapseWeight(VDP, VDA+N_neuronsperunit) <<  endl;
    ofs << "SR Gain (VNC and Head): " << sr.SRvncgain << " " << sr.SRheadgain << endl;
    ofs << "NMJ weights: \n B: " << NMJ_DB << " " << NMJ_VBa << " " << NMJ_VBp << "\n D: " <<  NMJ_DD << " " << NMJ_VDa << " " << NMJ_VDp << endl;
    ofs << "Head: \nBiases: \n SMD(D/V): " << h.NeuronBias(SMDD) << " " << h.NeuronBias(SMDV) << "\n RMD(D/V): "<< h.NeuronBias(RMDD) << " "<< h.NeuronBias(RMDV) << endl;
    ofs << "Time-constants: \n SMD(D/V): " << h.NeuronTimeConstant(SMDD) << " " << h.NeuronTimeConstant(SMDV) << "\n RMD(D/V): " << h.NeuronTimeConstant(RMDD) << " " << h.NeuronTimeConstant(RMDV) << endl;
    ofs << "Self conns: \n SMD(D/V): " <<h.ChemicalSynapseWeight(SMDD,SMDD) << " " << h.ChemicalSynapseWeight(SMDV,SMDV) << "\n RMD(D/V): " << h.ChemicalSynapseWeight(RMDD,RMDD) << " "<< h.ChemicalSynapseWeight(RMDV,RMDV) << endl;
    ofs << "Chem conns: " << "\n SMDD->RMDD: " << h.ChemicalSynapseWeight(SMDD, RMDD) << "\n SMDD->SMDV: " << h.ChemicalSynapseWeight(SMDD, SMDV) << "\n SMDD->RMDV: " << h.ChemicalSynapseWeight(SMDD, RMDV) << "\n RMDD->SMDD: " << h.ChemicalSynapseWeight(RMDD, SMDD) << "\n RMDD->SMDV: " << h.ChemicalSynapseWeight(RMDD, SMDV) << "\n RMDD->RMDV: " << h.ChemicalSynapseWeight(RMDD, RMDV) << "\n SMDV->SMDD: " << h.ChemicalSynapseWeight(SMDV, SMDD) << "\n SMDV->RMDD: " << h.ChemicalSynapseWeight(SMDV, RMDD) << "\n SMDV->RMDV: " << h.ChemicalSynapseWeight(SMDV, RMDV) << "\n RMDV->SMDD: " << h.ChemicalSynapseWeight(RMDV, SMDD) << "\n RMDV->RMDD: " << h.ChemicalSynapseWeight(RMDV, RMDD) << "\n RMDV->SMDV: " << h.ChemicalSynapseWeight(RMDV, SMDV) << endl;
    ofs << "Gap Juncs: " << "\n SMD-RMD: " << h.ElectricalSynapseWeight(SMDD, RMDD)
                         << "\n RMD-RMD: " << h.ElectricalSynapseWeight(RMDD, RMDV) << endl;
    ofs << "NMJ weights: \n SMD(D/V): " << NMJ_SMDD << " " << NMJ_SMDV << "\n RMD(D/V): " <<  NMJ_RMDD << " " << NMJ_RMDV << endl;
    ofs << "NMJ Gain: " << NMJ_Gain_Map << endl;
}
 */
