
#include "jsonUtils.h"
#include <iomanip>
//#include "../argUtils.h"

using json = nlohmann::json;

json getJsonFromFile(const string & jsonfile_){
        ifstream json_in(jsonfile_);
        json j;
        //assert(0 && "Worm2DoscNML(const string & jsonfile_)");
        json_in >> j;
        json_in.close();
        return j;
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


void to_json(json & j, const intPair & w)
{
  j = json{{"ind", w.ind}, {"val", w.val}};
}

void from_json(const json& j, intPair & w) 
{
        j.at("ind").get_to(w.ind);
        j.at("val").get_to(w.val);
}

void to_json(json & j, const toFromInt & w)
{
  j = json{{"to", w.to},  {"from", w.from},  {"val", w.val}};
}

void from_json(const json& j, toFromInt & w) 
{
        j.at("to").get_to(w.to);
        j.at("from").get_to(w.from);
        j.at("val").get_to(w.val);
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

void appendBodyToJson(json & j, WormBody& b)
{
{Params<double> par = getBodyParams(b);
    appendToJson<double>(j["Body"],par);}
{Params<int> par = getBodyParamsInts(b);
    appendToJson<int>(j["Body"],par);}
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


void appendMuscleToJson(json & j, Muscles & m)
{
{Params<double> par = getMusclesParamsDouble(m);
appendToJson<double>(j["Muscle"],par);}
{Params<int> par = getMusclesParamsInt(m);
appendToJson<int>(j["Muscle"],par);}
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

void appendAllNSJson( json & j, CTRNN & n)
{

{Params<vector<double> > parvec = getNervousSysParamsDoubleNH(dynamic_cast<NervousSystem&>(n));
appendToJson<vector<double> >(j,parvec);}
        
{Params<int> parvec = getNervousSysParamsIntNH(dynamic_cast<NervousSystem&>(n));
appendToJson<int>(j,parvec);}

{Params< vector<int> > parvec = getNervousSysVecInt(dynamic_cast<NervousSystem&>(n));
appendToJson<vector<int> >(j,parvec);}

appendNSToJson(j, dynamic_cast<NervousSystem&>(n));

}


void setNSFromJson(const json & j, NervousSystem & n)
{
    const json & j2 = j["Nervous system"];
    
    n.SetCircuitSize(j2["size"]["value"], j2["maxchemcons"]["value"], j2["maxelecconns"]["value"]);
   
    {vector<toFromWeight> weights = 
    j2["Chemical weights"]["value"].template get< vector<toFromWeight> >();
    for (int i = 0;i<weights.size();i++)
        n.SetChemicalSynapseWeight(weights[i].w.from, weights[i].to, weights[i].w.weight);}
    {vector<toFromWeight> weights = 
    j2["Electrical weights"]["value"].template get< vector<toFromWeight> >();
    for (int i = 0;i<weights.size();i++)
        n.SetElectricalSynapseWeight(weights[i].w.from, weights[i].to, weights[i].w.weight);}    
    {vector<double> vals = 
        j2["biases"]["value"].template get< vector<double> >();
        for (int i = 0;i<vals.size();i++)
        n.SetNeuronBias(i+1, vals[i]);}
    {vector<double> vals = 
        j2["taus"]["value"].template get< vector<double> >();
        for (int i = 0;i<vals.size();i++)
        n.SetNeuronTimeConstant(i+1, vals[i]);}
    {vector<double> vals = 
        j2["states"]["value"].template get< vector<double> >();
        for (int i = 0;i<vals.size();i++)
        n.SetNeuronState(i+1, vals[i]);}
    {vector<double> vals = 
        j2["gains"]["value"].template get< vector<double> >();
        for (int i = 0;i<vals.size();i++)
        n.SetNeuronGain(i+1, vals[i]);}
    {vector<double> vals = 
        j2["externalinputs"]["value"].template get< vector<double> >();
        for (int i = 0;i<vals.size();i++)
        n.SetNeuronExternalInput(i+1, vals[i]);}
    


}

void appendAllNSJson( json & j, NervousSystem & n)
{

{Params<vector<double> > parvec = getNervousSysParamsDoubleNH(n);
appendToJson<vector<double> >(j,parvec);}
        
{Params<int> parvec = getNervousSysParamsIntNH(n);
appendToJson<int>(j,parvec);}

{Params< vector<int> > parvec = getNervousSysVecInt(n);
appendToJson<vector<int> >(j,parvec);}

appendNSToJson(j, n);

}

vector<string> getCellNamesAll(const vector<string> & cell_names, int n_units)
{
    vector<string> cell_names_all;
    for (int i=0;i<n_units;i++) 
    cell_names_all.insert(cell_names_all.end(),cell_names.begin(),cell_names.end());
    return cell_names_all;
}

Params< vector<string> > getNervousSysCellNames(const vector<string> & cell_names, int n_units)
{
Params< vector<string> > par;
par.names = {"Cell name"};
par.vals = {getCellNamesAll(cell_names,n_units)};
return par;
}

void appendCellNamesToJson(json & j, const vector<string> & cell_names, const int & num_reps)
{
    Params< vector<string> > parvec = getNervousSysCellNames(cell_names, num_reps);
    appendToJson<vector<string> >(j,parvec);
}


double getParameterDouble(int argc, const char* argv[], string parName, const string defaultval)
{    

   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}

   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg], parName.c_str())==0) return stod(argv[arg+1]);
   return stod(defaultval.c_str());
}

int getParameterInt(int argc, const char* argv[], string parName, const string defaultval)
{    

   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}

   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg], parName.c_str())==0) return stoi(argv[arg+1]);
   return stoi(defaultval.c_str());
}

long getParameterLong(int argc, const char* argv[], string parName, const string defaultval)
{    

   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}

   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg], parName.c_str())==0) return stol(argv[arg+1]);
   return stol(defaultval.c_str());
}

string getParameterString(int argc, const char* argv[], string parName, const string defaultval)
{    

   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}

   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg], parName.c_str())==0) return (string) argv[arg+1];
   return defaultval;
}

string rename_file(const string & filename, const string & directoryName, const string & fileprefix)
{return directoryName + "/" + fileprefix + filename;}


bool directoryExists(const string & directoryName)
{
  struct stat sb;
  if (stat(directoryName.c_str(), &sb) != 0) return false;
  return true;
  //{cout << "Directory doesn't exist." << endl;exit(1);}
}
