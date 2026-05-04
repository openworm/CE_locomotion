
#include "jsonUtils.h"
#include <iomanip>
//#include "../argUtils.h"



using json = nlohmann::json;




bool parseValue(const std::string& s, double& v) { v = std::stod(s); return true; }
bool parseValue(const std::string& s, int&    v) { v = std::stoi(s); return true; }
bool parseValue(const std::string& s, long&   v) { v = std::stol(s); return true; }
bool parseValue(const std::string& s, std::string& v) { v = s; return true; }

bool parseValue(const std::string& s, bool& v) {
    if (s == "1" || s == "true" || s == "TRUE") { v = true; return true; }
    if (s == "0" || s == "false"|| s == "FALSE") { v = false; return true; }
    return false;
}


/* double Efunctor::eFunc(const double & val, const json & j)
{

  if (j.is_array())
  {
  for(auto it = j.begin(); it != j.end(); ++it) eFunc1(val,j);
  return;
  }

  eFunc1(val,j);
  

} */




json getJsonFromFile(const string & jsonfile_)
{
 
        cout << "jsonfile " << jsonfile_ << endl;

        ifstream json_in(jsonfile_);
        json j = json::parse(json_in);
       // assert(0);     
        //assert(0 && "Worm2DoscNML(const string & jsonfile_)");
        //json_in >> j;
        json_in.close();
        return j;
        
   
}

template<class T>
void appendToEnd(vector<T> & v1, const vector<T> & v2)
{
   v1.insert(v1.end(), v2.begin(), v2.end());
}

vector<toFromWeight> meanDupes(vector<toFromWeight> v1)
{

  vector<toFromWeight> v3;
  for (auto it = v1.begin(); it != v1.end(); ++it) 
  {
    int count = 1;
    double weighttot = it->w.weight;
    for (auto it2 = it+1; it2 !=v1.end(); ++it2)
      if (it2->to == it->to && it2->w.from == it->w.from) 
      {weighttot+=it2->w.weight;v1.erase(it2);count++;}
    v3.push_back({{it->w.from, weighttot/count}, it->to});

  }

  return v3;
}


void mergeToFromVec(vector<toFromWeight> & v1, const vector<toFromWeight> & v2, 
  const int & fval, const int & tval)
{

  for (int j = 0; j<v2.size(); j++)
  v1.push_back({{v2[j].w.from + fval, v2[j].w.weight}, v2[j].to + tval});
  v1 = meanDupes(v1);
}

void incToFromVec(vector<toFromWeight> & v1, const int & fval, const int & tval)
{

  for (int j = 0; j<v1.size(); j++) {v1[j].w.from += fval; v1[j].to += tval;}
  //v1.push_back({{v2[j].w.from + fval, v2[j].w.weight}, v2[j].to + tval});

}



template<class T>
void mergeVecKeys(json & j1, const json & j2, 
  const vector<string> & keyval, void (*func)(vector<T> & v1, const vector<T> & v2))
{
    vector<T> v1, v2;
    bool hasv1 = getEvoVecFromJ<T>(j1, keyval, v1);
    bool hasv2 = getEvoVecFromJ<T>(j2, keyval, v2);
    if (hasv1 && hasv2)
    {
    func(v1,v2);
    //v1.insert(v1.end(), v2.begin(), v2.end());
    json j3 = v1;
    set_nested_json(j1, keyval, j3);
    }
    else if (hasv2){
    json j3 = v2;
    set_nested_json(j1, keyval, j3);
    }
}


void mergeVecKeys(json & j1, const json & j2, 
  const vector<string> & keyval, const int & fval, const int & tval, bool doInc = false)
{
    vector<toFromWeight> v1, v2;
    bool hasv1 = getEvoVecFromJ<toFromWeight>(j1, keyval, v1);
    bool hasv2 = getEvoVecFromJ<toFromWeight>(j2, keyval, v2);
    if (hasv1 && hasv2)
    {
    mergeToFromVec(v1,v2,fval,tval);
    json j3 = v1;
    set_nested_json(j1, keyval, j3);
    }
    else if (hasv2){
    if (doInc) incToFromVec(v2,fval,tval);
    json j3 = v2;
    set_nested_json(j1, keyval, j3);
    }
}

template<class T>
void mergeValKeys(json & j1, const json & j2, const vector<string> & keyval, T (*func)(const T&, const T&))
{
  T v1, v2;
  bool hasv1 = getEvoValFromJ<T>(j1, keyval, v1);
  bool hasv2 = getEvoValFromJ<T>(j2, keyval, v2);

  if (hasv1 && hasv2)
  {
    json j3 = func(v1, v2);
    set_nested_json(j1, keyval, j3);
  }
  else if (hasv2)
  {
    json j3 = v2;
    set_nested_json(j1, keyval, j3);
  }

}



void mergeJson(json & j1, const json & j2)
{

    string NS = "Nervous system";
    string CW = "Chemical weights";
    string EW = "Electrical weights";
    string V = "value";
    string SR = "Stretch receptor";
    string W = "weights";

    int j1size = j1[NS]["size"][V];

    int j1SRsize = 0;
    if (j1.contains(SR))  j1SRsize = j1[SR]["NStretch"][V];

    {
    
    vector<vector<string> > keys;
    keys.push_back({NS,CW,V});
    keys.push_back({NS,EW,V});

    for (int i = 0; i<keys.size(); i++) mergeVecKeys(j1,j2, keys[i], j1size, j1size);

    }


    {
    vector<vector<string> > keys;
    keys.push_back({SR,"SR D", W,V});
    keys.push_back({SR,"SR V", W,V});
    for (int i = 0; i<keys.size(); i++) mergeVecKeys(j1,j2, keys[i], 0, j1SRsize);
    }
    {
    vector<vector<string> > keys;
    keys.push_back({SR,"SR D NS", W,V});
    keys.push_back({SR,"SR V NS", W,V});
    for (int i = 0; i<keys.size(); i++) mergeVecKeys(j1,j2, keys[i], j1SRsize, j1size, true);
    }

    {
    vector<vector<string> > keys;
    keys.push_back({"Dorsal NMJ", W,V});
    keys.push_back({"Ventral NMJ", W,V});
    for (int i = 0; i<keys.size(); i++) mergeVecKeys(j1,j2, keys[i], j1size, 0, true);
    }

    {
    vector<vector<string> > keys;
    keys.push_back({"Driving input", W,V});
    for (int i = 0; i<keys.size(); i++) mergeVecKeys(j1,j2, keys[i], 0, j1size, true);
    }

    {
    vector<vector<string> > keys;
    keys.push_back({"OutputNS", W,V});
    for (int i = 0; i<keys.size(); i++) mergeVecKeys(j1,j2, keys[i], j1size, j1size, true);
    }
    
    {
    vector<string> vecvals = {"Rtaus", "biases", "externalinputs", "gains", "outputs", 
      "paststates", "states", "taus"};
    for (int i=0;i<vecvals.size();i++) mergeVecKeys<double>(j1, j2, {NS,vecvals[i],V}, appendToEnd<double>);
    }

    {
    vector<string> vecvals = {"NumChemicalConns", "NumElectricalConns"};
    for (int i=0;i<vecvals.size();i++) mergeVecKeys<int>(j1, j2, {NS,vecvals[i],V}, appendToEnd<int>);
    }
    {
    vector<string> vecvals = {"Cell name"};
    for (int i=0;i<vecvals.size();i++) mergeVecKeys<string>(j1, j2, {NS,vecvals[i],V}, appendToEnd<string>);
    }

    
    mergeVecKeys<double>(j1,j2, {"Driving input","strengths",V}, appendToEnd<double>);

    mergeValKeys<int>(j1, j2, {SR,"NStretch",V}, [](const int& v1, const int& v2){return v1 + v2;});

    mergeValKeys<int>(j1, j2, {NS,"size",V}, [](const int& v1, const int& v2){return v1 + v2;});
    mergeValKeys<int>(j1, j2, {NS,"maxchemcons",V}, [](const int& v1, const int& v2){return max(v1,v2);});
    mergeValKeys<int>(j1, j2, {NS,"maxeleccons",V}, [](const int& v1, const int& v2){return max(v1,v2);});
    mergeValKeys<string>(j1, j2, {NS,"Model name",V}, 
      [](const string& v1, const string& v2){return v1 + "_" + v2;});
    
   

   
}


void to_json(json & j, const weightentry & w)
{
  j = json{{"from", w.from}, {"weight", w.weight}};
}

void from_json(const json & j, weightentry & w)
{
    
      j.at("from").get_to(w.from);
      j.at("weight").get_to(w.weight);
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

void to_json(json & j, const intDoubDoub & w)
{
  j = json{{"evotag", w.ind}, {"val1", w.val1}, {"val2", w.val2}};
}

void from_json(const json& j, intDoubDoub & w) 
{
        j.at("evotag").get_to(w.ind);
        j.at("val1").get_to(w.val1);
        j.at("val2").get_to(w.val2);
}

void to_json(json & j, const doubDoub & w)
{
  j = json{{"val1", w.val1}, {"val2", w.val2}};
}

void from_json(const json& j, doubDoub & w) 
{
        j.at("val1").get_to(w.val1);
        j.at("val2").get_to(w.val2);
}


vector<intPair> from_evo_json(const json& j)
{
vector<intPair> w;
for(auto it = j.begin(); it != j.end(); ++it)
  w.push_back({it->at("ind").get<int>(),it->at("evotag").get<int>()});

return w;

}

json to_evo_json(const vector<intPair> & w)
{
json j = json::array();
for (int i=0;i<w.size();i++) j.push_back({{"ind", w[i].ind}, {"evotag", w[i].val}});
return j;
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


void to_json(json & j, const fromToInt & w)
{
  j = json{{"to", w.to},  {"from", w.from},  {"evotag", w.val}};
}

void from_json(const json& j, fromToInt & w) 
{
        j.at("to").get_to(w.to);
        j.at("from").get_to(w.from);
        j.at("evotag").get_to(w.val);
}



void to_json(json & j, const fromToStr & w){
 j = json{{"from", w.from}, {"to", w.to}, {"evotag", w.evotag}};
}
void from_json(const json & j, fromToStr & w)
{
        j.at("from").get_to(w.from);
        j.at("to").get_to(w.to);
        j.at("evotag").get_to(w.evotag);
}

void to_json(json & j, const intStr & w)
{
j = json{{"val", w.val}, {"evotag", w.evotag}};

}

void from_json(const json & j, intStr & w)
{
j.at("val").get_to(w.val);
        j.at("evotag").get_to(w.evotag);

}

void from_json(const json & j, strDoubDoub & w)
{
   j.at("val1").get_to(w.val1);
        j.at("val2").get_to(w.val2);
        j.at("evotag").get_to(w.evotag);
}


void to_json(json & j, const strDoubDoub & w)
{
j = json{{"val1", w.val1}, {"val2", w.val2}, {"evotag", w.evotag}};

}


void set_nested_json(json & j, const vector<string> & keys, const json & value)
{

   json * current = &j;

  for (int i = 0; i<keys.size(); ++i)
  {
  const auto& k = keys[i];
  if (i==keys.size()-1)
  {

    (*current)[k]=value;

  }
  else 
  {
    if (!(*current)[k].is_object())
      (*current)[k] = json::object();
    current = &((*current)[k]);
}
}
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

void splitWeightEntry(const vector<weightentry> & w, vector<int> & ind, vector<double> & weight)
{

  for (int i=0;i<w.size();i++){ind.push_back(w[i].from); weight.push_back(w[i].weight);}
  
}


vector<weightentry> makeWeightEntry(const vector<int> & ind, const vector<double> & weight)
{
  vector<weightentry> w;
  for (int i=0;i<ind.size();i++) w.push_back({ind[i],weight[i]});
  return w;
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



void addWeightentry(json & j, const vector<weightentry> & vec, const string & frompar, const string & weightpar)
{

for (int i=0;i<vec.size();i++)
{
const weightentry & w = vec[i];
j.push_back({{frompar, w.from}, {weightpar, w.weight}});
} 

}


void addWeightentry(json & j, const weightentry & w, const string & frompar, const string & weightpar)
{

  j[frompar] = w.from;
  j[weightpar]=w.weight;

}


void addEvolvableIP(json & j, vector<intPair> & vec, const string & parameter, 
  const vector<string> & cell_names_full)
{

  for (int i=0;i<vec.size();i++) 
    {
    const intPair & val = vec[i];
    const string & name = cell_names_full[val.ind-1];
    bool found = false;
    j[name][parameter]["evotag"] = val.val;
    }
    
}


void addEvolvableTFI(json & j, const vector<fromToInt> & vec, const vector<string> & cell_names_full)
{

  //if (!j.contains("nervous_system")) return;
  //json & j2 = j["nervous_system"];

  for (int i=0;i<vec.size();i++) 
  {
  const fromToInt & val = vec[i];
  bool found = false;
  for (auto it = j.begin(); it != j.end(); ++it)
  {
  if (it->at("to")==cell_names_full[val.to-1] && it->at("from")==cell_names_full[val.from-1]) 
  {
   if (it->at("weight").contains("evotag")) it->at("weight").at("evotag") = val.val; 
   else (*it)["weight"]["evotag"] = val.val;
   found = true;
   break; 
  }
  }
  assert(found);
  }

}


void appendNSToJsonByCell(json & j, NervousSystem& n, const vector<string> & cell_names_full)
{

vector<double> taus = getVector<double>(n.taus, n.size);
vector<double> bias = getVector<double>(n.biases, n.size);
vector<double> gains = getVector<double>(n.gains, n.size);
vector<double> states = getVector<double>(n.states, n.size);

/* vector<string> cell_names_full;
for (int i=0;i<cell_names.size();i++) {
const string & name = cell_names[i] + "_" + to_string(i);
cell_names_full.push_back(name);
} */


if (!j.contains("nervous_system")) j["nervous_system"] = json::object();
json & j2 = j["nervous_system"];
if (!j2.contains("cells")) j2["cells"] = json::object();
json & j3 = j2["cells"];
for (int i=0;i<cell_names_full.size();i++) 
  {
    const string & name = cell_names_full[i];
    if (!j3.contains(name)) j3[name] = json::object();
    json & j4 = j3[name];
    j4["tau"]["value"] = taus[i];
    j4["bias"]["value"] = bias[i];
    j4["gain"]["value"] = gains[i];
    j4["state"]["value"] = states[i];
  }



  vector<toFromWeight> chem_wei = getNSToFromVec(n.chemicalweights, n.NumChemicalConns, n.size);
  if (!j2.contains("chemical_conns")) j2["chemical_conns"] = json::object();
  if (!j2["chemical_conns"].contains("value")) j2["chemical_conns"]["value"] = json::array();
  for (const toFromWeight& val : chem_wei)
  //for (int i=0;i<chem_wei.size();i++)
  {
  //const toFromWeight & val = chem_wei[i];
  json j = json::object();
  j["to"] = cell_names_full[val.to-1];
  j["from"] = cell_names_full[val.w.from-1];
  j["weight"] =  json::object();
  j["weight"]["value"] = val.w.weight;
  j2["chemical_conns"]["value"].push_back(j);
  }



  vector<toFromWeight> elec_wei = getNSToFromVec(n.electricalweights, n.NumElectricalConns, n.size);
  if (!j2.contains("electrical_conns")) j2["electrical_conns"] = json::object();
  if (!j2["electrical_conns"].contains("value")) j2["electrical_conns"]["value"] = json::array();

  //j2["electrical_conns"] = json::object();
  //j2["electrical_conns"]["value"] = json::array();
  //j2["electrical_conns"] = json::array();
  for (const toFromWeight& val : elec_wei)
  //for (int i=0;i<elec_wei.size();i++)
  {
  //const toFromWeight & val = elec_wei[i];
  json j = json::object();
  j["to"] = cell_names_full[val.to-1];
  j["from"] = cell_names_full[val.w.from-1];
  j["weight"] =  json::object();
  j["weight"]["value"] = val.w.weight;
  j2["electrical_conns"]["value"].push_back(j);
  }



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

vector<toFromWeight> getNSToFromVec(TMatrix<weightentry> & vec, TVector<int> & sizes, int tot_size)
{    
    vector<toFromWeight> newvec;
    for (int i=1; i<=tot_size; i++){    
        for (int j=1; j<=sizes[i]; j++) { 
            toFromWeight tv(vec[i][j], i);
            newvec.push_back(tv);}        
    }
    return newvec;
}


void appendMatrixToJson(json & j, TMatrix<weightentry> & vec, TVector<int> & sizes, int tot_size)
{    
   
    j["value"] = getNSToFromVec(vec,sizes,tot_size);

}

void appendChemNSToJson(json & j, NervousSystem& c)
{
  string sval = "Chemical weights";
  
  //if (!j.contains(sval)) j[sval] = json::object();

  j[sval]["message"] = "chemical weights in sparse format";
  appendMatrixToJson(j[sval], c.chemicalweights, c.NumChemicalConns, c.size);
}

void appendElecNSToJson(json & j, NervousSystem& c)
{
    appendMatrixToJson(j["Electrical weights"], c.electricalweights, c.NumElectricalConns, c.size);
    j["Electrical weights"]["message"] = "electrical weights in sparse format";
}

void appendNSToJson(json & j, NervousSystem& c)
{
  appendChemNSToJson(j,c);
  appendElecNSToJson(j,c);   
}

/* void appendAllNSJson( json & j, CTRNN & n)
{

{Params<vector<double> > parvec = getNervousSysParamsDoubleNH(dynamic_cast<NervousSystem&>(n));
appendToJson<vector<double> >(j,parvec);}
        
{Params<int> parvec = getNervousSysParamsIntNH(dynamic_cast<NervousSystem&>(n));
appendToJson<int>(j,parvec);}

{Params< vector<int> > parvec = getNervousSysVecInt(dynamic_cast<NervousSystem&>(n));
appendToJson<vector<int> >(j,parvec);}

appendNSToJson(j, dynamic_cast<NervousSystem&>(n));

}
 */
void setNSFromJsonNZ(const json & j, NervousSystem & n, const bool setStates)
{

  const json & j2 = j["Nervous system"];

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
        
        if (setStates) //removed external inputs here
        {vector<double> vals = 
        j2["states"]["value"].template get< vector<double> >();
        for (int i = 0;i<vals.size();i++)
        n.SetNeuronState(i+1, vals[i]);}

        {vector<double> vals = 
        j2["gains"]["value"].template get< vector<double> >();
        for (int i = 0;i<vals.size();i++)
        n.SetNeuronGain(i+1, vals[i]);}

        if (setStates) //removed external inputs here
        {vector<double> vals = 
        j2["externalinputs"]["value"].template get< vector<double> >();
        for (int i = 0;i<vals.size();i++)
        n.SetNeuronExternalInput(i+1, vals[i]);}


}

void setNSFromJson(const json & j, NervousSystem & n, const bool setStates)
{
    const json & j2 = j["Nervous system"];
    
    n.SetCircuitSize(j2["size"]["value"], j2["maxchemcons"]["value"], j2["maxelecconns"]["value"]);
    setNSFromJsonNZ(j,n,setStates);

   
    
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

vector<string> getCellNamesUnits(const vector<string> & cell_names, int n_units)
{
    vector<string> cell_names_all;
    for (int i=0;i<n_units;i++) 
    {
      vector<string> cell_names_unit;
      for (int j=0;j<cell_names.size();j++) cell_names_unit.push_back(cell_names[j] + "_" + to_string(i));
      cell_names_all.insert(cell_names_all.end(),cell_names_unit.begin(),cell_names_unit.end());
    }
    return cell_names_all;
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
    if (cell_names[0] == "not implemented" && j.contains("Cell name")) return;
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


void evoPars::addParsToJson(json &j) const
  {

    {vector<string> names = {"MutationVariance", "CrossoverProbability", 
         "MaxExpectedOffspring", "ElitistFraction",
         "Duration", "Transient", "StepSize"};
    vector<double> vals = {MutationVariance, CrossoverProbability, 
         MaxExpectedOffspring, ElitistFraction,
         Duration, Transient, StepSize};
    for (int i=0;i<names.size();i++) j[names[i]]["value"]=vals[i];}

    {vector<string> names = {"randomseed", "SelectionMode", "ReproductionMode", 
         "PopulationSize", "MaxGenerations", "CrossoverMode", "SearchConstraint", 
         "CheckpointInterval", "ReEvaluationFlag", "skip_steps", "N_curvs", "VectSize_temo"};
    vector<int> vals = {(int) randomseed, SelectionMode, ReproductionMode, 
         PopulationSize, MaxGenerations, CrossoverMode, SearchConstraint, CheckpointInterval, 
         ReEvaluationFlag, skip_steps, N_curvs, VectSize_temo};
    for (int i=0;i<names.size();i++) j[names[i]]["value"]=vals[i];}

      {vector<string> names = {"fileprefix", "evoType"};
      vector<string> vals = {fileprefix, evoType};
       for (int i=0;i<names.size();i++) j[names[i]]["value"]=vals[i];}


  }
const doubIntParamsHead evoPars::getParams() const
   {
       doubIntParamsHead var1;
       var1.parDoub.head = "Evolutionary Optimization Parameters";
       var1.parInt.head = "Evolutionary Optimization Parameters";
       var1.parDoub.names = 
       {"MutationVariance", "CrossoverProbability", 
         "MaxExpectedOffspring", "ElitistFraction",
         "Duration", "Transient", "StepSize"};
       var1.parDoub.vals = {MutationVariance, CrossoverProbability, 
         MaxExpectedOffspring, ElitistFraction,
         Duration, Transient, StepSize};

       var1.parInt.names = {"randomseed", "SelectionMode", "ReproductionMode", 
         "PopulationSize", "MaxGenerations", "CrossoverMode", "SearchConstraint", 
         "CheckpointInterval", "ReEvaluationFlag", "skip_steps", "N_curvs", "VectSize_temo"};
       var1.parInt.vals = {randomseed, SelectionMode, ReproductionMode, 
         PopulationSize, MaxGenerations, CrossoverMode, SearchConstraint, CheckpointInterval, 
         ReEvaluationFlag, skip_steps, N_curvs, VectSize_temo};

       return var1;
   }


  //void setFromArgs(int argc, const char* argv[]);
  //string rename_file(string filename);



string evoPars::rename_file(string filename){return directoryName + "/" + 
    fileprefix + filename;}


void evoPars::setFromArgs(shared_ptr<const CmdArgs> cmd)
{

bool seed_flag = 1;

MaxGenerations = cmd->getArgValInt("--maxgens", MaxGenerations);
MutationVariance = cmd->getArgValDoub("--MutVar", MutationVariance);
CrossoverProbability = cmd->getArgValDoub("--CrossProb", CrossoverProbability);
directoryName = cmd->getArgVal("--folder","HJUYGYT");
struct stat sb;
if (stat(directoryName.c_str(), &sb) != 0) 
{cout << "Directory doesn't exist." << endl;exit(1);}

PopulationSize = cmd->getArgValInt("-p",PopulationSize);
CheckpointInterval = cmd->getArgValInt("-cpt", CheckpointInterval);
Duration = cmd->getArgValDoub("-d", Duration);
Transient = cmd->getArgValDoub("-t", Transient);
evoType = cmd->getArgVal("--evoType", evoType);

if (seed_flag){ 
  long randomseed1 = cmd->getArgValLong("-R",-1);
  if (randomseed1!=-1)
  {seed_flag = 0;randomseed = randomseed1;}
}

if (seed_flag){ 
  long randomseed1 = cmd->getArgValLong("-r",-1);
  if (randomseed1!=-1)
  {seed_flag = 0;randomseed = randomseed1 + static_cast<long>(time(NULL));}
}


}

void evoPars::setFromArgs(int argc, const char* argv[])
{

  if (((argc-1) % 2) != 0)
     {
      cout << "The arguments are not configured correctly." << endl;
      exit(1);
    }
    
    bool seed_flag = 1;

    for (int arg = 1; arg<argc; arg+=2)
    { 
    if (strcmp(argv[arg],"--maxgens")==0) MaxGenerations = stoi(argv[arg+1]);

    if (strcmp(argv[arg],"--MutVar")==0) MutationVariance = stod(argv[arg+1]);
    if (strcmp(argv[arg],"--CrossProb")==0) CrossoverProbability = stod(argv[arg+1]);

    //if (strcmp(argv[arg],"--dorandinit")==0) simRandomInit = stol(argv[arg+1]);
    //if (strcmp(argv[arg],"--skipOrigSim")==0) skipOrigSim = stol(argv[arg+1]);
    //if (strcmp(argv[arg],"--donml")==0) do_nml = stol(argv[arg+1]);


    if (strcmp(argv[arg],"--folder")==0) {
      directoryName= argv[arg+1];
      struct stat sb;
      if (stat(directoryName.c_str(), &sb) != 0) 
      {cout << "Directory doesn't exist." << endl;exit(1);}
    }

    if (seed_flag){ 
    if (strcmp(argv[arg],"-R")==0) randomseed = stol(argv[arg+1]);
    if (strcmp(argv[arg],"-r")==0) 
    {long randomseed1 = static_cast<long>(time(NULL));
           randomseed = randomseed1 + stol(argv[arg+1]);
    }
    seed_flag = 0;
    }
    //if (strcmp(argv[arg], "--modelname")==0) model_name = argv[arg+1];
    if (strcmp(argv[arg],"-p")==0) PopulationSize = stoi(argv[arg+1]);
    if (strcmp(argv[arg],"-d")==0) Duration = stod(argv[arg+1]);
    if (strcmp(argv[arg],"-t")==0) Transient = stod(argv[arg+1]);
    if (strcmp(argv[arg],"-cpt")==0) CheckpointInterval = stoi(argv[arg+1]);
    if (strcmp(argv[arg],"--evoType")==0) evoType = (string) argv[arg+1];
    

    //cout << "doCPT " << doCPT << endl;
    //if (strcmp(argv[arg],"--nervous")==0) nervousSystemNameForSim = argv[arg+1];
    
    }

   

}