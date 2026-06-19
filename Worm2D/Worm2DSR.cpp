#include "Worm2DSR.h"
#include <algorithm>
#include <map>
#include <set>
//#include "../neuromlLocal/c302ForW2D.h"

/* Worm2DSRm::Worm2DSRm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_):
//Worm2Dm(par1_, n_ptr_, new Muscles),Worm2D(par1_,n_ptr_),w2dsr_ptr(sr_ptr_){}
Worm2Dm(par1_, n_ptr_),Worm2D(par1_,n_ptr_),w2dsr_ptr(sr_ptr_){}*/

//Worm2DSRb::Worm2DSRb(const json & j):w2dsr_ptr(getSR(j)){
//  setParsFromJson(j);
//}

Worm2DSRb::Worm2DSRb(shared_ptr<SR> sr_ptr_):w2dsr_ptr(sr_ptr_){}


Worm2DSR::Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, 
  shared_ptr<SR> sr_ptr_, shared_ptr<const CmdArgs> cmd, const json & j):
Worm2Dm(par1_, n_ptr_, cmd, j),Worm2D(par1_,n_ptr_),Worm2DSRb(sr_ptr_){} 

Worm2DSR::Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_,
  shared_ptr<SR> sr_ptr_, shared_ptr<const CmdArgs> cmd, const json & j,
  bool forceNoOrigInputs):
Worm2Dm(par1_, n_ptr_, cmd, j),Worm2D(par1_,n_ptr_,forceNoOrigInputs),Worm2DSRb(sr_ptr_)
{
  const json & js1 = BPitsJson;

  bool do_nml =  cmd->getArgValInt("--donml",0);
  if (!do_nml){
    bool doLegacy;
    getValCJWorm<bool>("do_legacy",doLegacy);

    NervousSystem * n = dynamic_cast<NervousSystem*>(n_ptr);
    assert(n);

    setNSFromJson(js1,*n, doLegacy);
  }

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(js1);

  setMuscBodExt(js1);
}

Worm2DSR::Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, 
  shared_ptr<SR> sr_ptr_, shared_ptr<const CmdArgs> cmd):
Worm2Dm(par1_, n_ptr_, cmd),Worm2D(par1_,n_ptr_),Worm2DSRb(sr_ptr_){} 


Worm2DSR::Worm2DSR(const json & j, shared_ptr<const CmdArgs> cmd):
Worm2Dm(getIzqPars(j), getNS(cmd, j), cmd, j), Worm2D(getIzqPars(j) ,nullptr), Worm2DSRb(getSR(j, this))
//Worm2DSR(getIzqPars(j),getNS(cmd, j),getSR(j, this),cmd ,j)
//Worm2DSR::Worm2DSR(const json & j, shared_ptr<const CmdArgs> cmd):
//Worm2Dm(getIzqPars(j),getNS(cmd, j)),Worm2D(getIzqPars(j) ,nullptr),Worm2DSRb(getSR(j, this))
{
  
  //BPitsJson = j;
  
    const json & js1 = BPitsJson;

   
    bool do_nml =  cmd->getArgValInt("--donml",0);
    if (!do_nml){
    
    bool doLegacy;
    getValCJWorm<bool>("do_legacy",doLegacy);

    NervousSystem * n = dynamic_cast<NervousSystem*>(n_ptr);
    assert(n);
    //cout << "doLegacy " << doLegacy << endl;
    
    setNSFromJson(js1,*n, doLegacy);
    }


    if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(js1);

    //Worm2DSRb::setParsFromJson(j);

    //if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);
    //InputSwitcher::construct(BPitsJson);

    setMuscBodExt(js1);

    //setUpMuscleConn(j);
    //setUpBodyConn(j);
    //makeExternalInputConnFromJson(j);

}

Worm2DSR::Worm2DSR(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSR(getJsonFromFile(jsonfilename_),cmd){}


Worm2DSRm::Worm2DSRm(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSRm(getJsonFromFile(jsonfilename_), cmd){}

Worm2DSRm::Worm2DSRm(const json & j, shared_ptr<const CmdArgs> cmd):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), 0, cmd, j),Worm2DSRb(getSR(j,this))//,baseParameters(j,cmd)
{
  const json & js1 = BPitsJson;

  

  //BPitsJson = j;
   // W2Dbaseparameters1b->setParsFromJson(j["Worm"]);
   // setWormPars(cmd);

    //if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);

    if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(js1);

    //InputSwitcher::construct(j);

    setBodExt(js1);
    //setUpMuscleConn(j);
    //setUpBodyConn(j);
    //makeExternalInputConnFromJson(j);

}


Worm2DSRE::Worm2DSRE(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSRE(getJsonFromFile(jsonfilename_),cmd){}

Worm2DSRE::Worm2DSRE(const json & j, shared_ptr<const CmdArgs> cmd, bool callInit):
Worm2Dm(getIzqPars(j),getNS(cmd, j), cmd, j),Worm2DSR(j,cmd),genPhenLims(makeVals())//,itsJson(j)
{

  
  //bool do_evol = cmd->getArgValInt("--doevol", 0);
  //if (!do_evol) return;
    
  if (!callInit) return;

  if (genPhenLims.size()>0) setInitPheno();
    
  writeOrigGen(cmd);


}

Worm2DSRE::Worm2DSRE(const json & j, shared_ptr<const CmdArgs> cmd, bool callInit,
  bool forceNoOrigInputs):
Worm2Dm(getIzqPars(j),getNS(cmd, j), cmd, j),
Worm2DSR(getIzqPars(j), getNS(cmd, j), getSR(j, this), cmd, j, forceNoOrigInputs),
genPhenLims(makeVals())
{

  if (!callInit) return;

  if (genPhenLims.size()>0) setInitPheno();
    
  writeOrigGen(cmd);

}


void Worm2DSRb::addParsToJson(json & j)
{
if (w2dsr_ptr!=nullptr) w2dsr_ptr->addParsToJson(j);

}

void Worm2DSRm::addParsToJson(json & j)
{
  j = BPitsJson;
  Worm2Dm::addParsToJson(j);
 Worm2DSRb::addParsToJson(j);
  
}


void Worm2DSR::addParsToJson(json & j)
{

  




  if (true){

  if (false){  

    
  NervousSystem* const n = dynamic_cast<NervousSystem*>(n_ptr);
  if (n){
  string nsHead = "Nervous system";
  appendAllNSJson(j[nsHead], *n);
  //appendNSToJsonByCell(j, *n, getCellNamesAll());
  appendNSToJsonByCell(j,*n);

  //j[nsHead]["section sizes"] = jsects;
  }
  }


  Worm2D::addParsToJson(j);
  Worm2DSRb::addParsToJson(j);
  }

}


void Worm2DSRE::resetFromJson(const json & js1)
{


  NervousSystem * const n = dynamic_cast<NervousSystem*>(n_ptr);
  if(n){
  bool doLegacy;
  getValCJWorm<bool>("do_legacy",doLegacy);

  //copy in current states, external inputs here??
    

  setNSFromJsonNZ(js1,*n,doLegacy);
  }
  
    Worm2DSRb::setParsFromJson(js1);


    setMuscBodExt(js1);
    makeExternalInputConnFromJson(js1);
}

void Worm2DSRE::resetFromBPJson()
{
resetFromJson(BPitsJson);

}




void WormCO2DSR::addParsToJson(json & j){

  j = BPitsJson;
  
  if (false){
  
  vector<double> states;
  for (int i = 1; i <= par1.N_size; i++) states.push_back(n_ptr->NeuronState(i));
  j["Nervous system"]["states"]["value"] = states;      
  j["Driving input"]["strengths"]["value"] = externalInputs;

  }
  else{
  Worm2DSRE::addParsToJson(j);// needs fixing bpjson overwrites sensor
  Sensor::addParsToJson(j);
  }
}



void  Worm2DSRb::setParsFromJson(const json & j)
{if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);}


shared_ptr<SR> Worm2DSRb::getSR(const json & j, baseParameters * basePar1_)
{
    if (j.contains("stretch_receptor")){
    
    if (j["stretch_receptor"]["type"]["value"] == "SR18") return make_shared<SR18>();

    return make_shared<SRCE>(j["stretch_receptor"]["n_segs"]["value"],
      j["stretch_receptor"]["n_stretch"]["value"], basePar1_);

    }

    else if (j.contains("Stretch receptor")){
    
    if (j["Stretch receptor"]["Type"]["value"] == "SR18") return make_shared<SR18>();

    return make_shared<SRCE>(j["Stretch receptor"]["NSegs"]["value"],
      j["Stretch receptor"]["NStretch"]["value"], basePar1_);

    }
    else return nullptr;

}

//json j;
//Worm2DSR w(j);

void Worm2DSR::Step1()
{
 
 

  b.StepBody(settedStepSize);

  zeroAllInputs();

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->updateAll(b);
  
  setExternalInput();
  //setExternalInputOrig();

  if (w2dsr_ptr!=nullptr) {w2dsr_ptr->incNS(*n_ptr);}
 
  n_ptr->EulerStep(settedStepSize);


  if (doOrigMuscInput) setMuscleInputOrig();
  else setMuscleInput();

  setBodyInput();
  

}

void Worm2DSRm::Step1()
{
  
  zeroAllInputs();

  b.StepBody(settedStepSize);

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->updateAll(b);
  
   
  setExternalInput();
  //setExternalInputOrig();

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->incNS(*n_ptr);

  n_ptr->EulerStep(settedStepSize);
  
  //setMuscleInput();


  setBodyInput();
  
}

vector<doubIntParamsHead> Worm2DSRb::getWormParams(){

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;

    var1.parDoub.head = "worm";
    var1.parDoub.names = {"variable 1"};
    var1.parDoub.vals = {1.0};

    parvec.push_back(var1);
    return parvec;

}


void Worm2DSRm::writeAct()
{
  
  size_t pos = getPos("act.dat");
  ofstream & ofs = ofsvec[pos];  
  int & tt = tts[pos];

  if (++tt >= dataskips) {
    tt = 0;

    ofs << datatime;
    //ofs << "\nSR: ";
    // Stretch receptors

     if (w2dsr_ptr!=nullptr) w2dsr_ptr->writeAct(ofs);

      
    // Head Neurons
        //ofs << "\nH: ";
        int offset = par1.N_units*par1.N_neuronsperunit;

        for (int i = offset + 1; i <= par1.N_size; i++) {
            ofs <<  " " << n_ptr->NeuronOutput(i);
        }


      writeVNC(ofs);
     
        // Muscles
        //ofs << "\nM: ";
      writeMusc(ofs);
        
      writeExtInp(ofs);

    

    ofs << endl;
  }
}

void Worm2DSR::writeAct()
{
  
  size_t pos = getPos("act.dat");
  ofstream & ofs = ofsvec[pos];  
  int & tt = tts[pos];

  if (++tt >= dataskips) {
    tt = 0;

    ofs << datatime;
    //ofs << "\nSR: ";
    // Stretch receptors

    if (w2dsr_ptr!=nullptr) w2dsr_ptr->writeAct(ofs);

      
    // Head Neurons
        //ofs << "\nH: ";
    int offset = par1.N_units*par1.N_neuronsperunit;

    for (int i = offset + 1; i <= par1.N_size; i++) 
        ofs <<  " " << n_ptr->NeuronOutput(i);
    
      writeVNC(ofs);
     
        // Muscles
        //ofs << "\nM: ";
      writeMusc(ofs);
        
      writeExtInp(ofs);

   
    // Muscles
    //ofs << "\nM: ";
    
    ofs << endl;
  }
}

/* void Worm2DSRE::setNSEvoFromJson(const json & j, NervousSystem & n)
{
    const json & j2 = j["Nervous system"];
    
    chem_weights_evo = j2["Chemical weights"]["evolvable"].template get< vector<toFromInt> >();
    elec_weights_evo = j2["Electrical weights"]["evolvable"].template get< vector<toFromInt> >();
    biases_evo = j2["biases"]["evolvable"].template get< vector<intPair> >();
    taus_evo = j2["taus"]["evolvable"].template get< vector<intPair> >();
    gains_evo = j2["gains"]["evolvable"].template get< vector<intPair> >();

    
} */



const string evolvableRangesKey = "evolvable_ranges";
const string legacyEvolvableKey = "Evolvable";
const string evoTagRangePrefix = "evotag_";

json * getEvolvableRanges(json & j)
{
  if (j.contains(evolvableRangesKey)) return &j[evolvableRangesKey];
  if (j.contains(legacyEvolvableKey)) return &j[legacyEvolvableKey];
  return nullptr;
}

const json * getEvolvableRanges(const json & j)
{
  if (j.contains(evolvableRangesKey)) return &j.at(evolvableRangesKey);
  if (j.contains(legacyEvolvableKey)) return &j.at(legacyEvolvableKey);
  return nullptr;
}

bool parseEvoTagRangeKey(const string & key, int & evotag)
{
  if (key.find(evoTagRangePrefix) != 0) return false;
  const string num = key.substr(evoTagRangePrefix.size());
  if (num.empty()) return false;
  for (int i=0;i<num.size();i++)
    if (num[i] < '0' || num[i] > '9') return false;
  evotag = stoi(num);
  return true;
}

string makeEvoTagRangeKey(int evotag)
{
  return evoTagRangePrefix + to_string(evotag);
}

json * getEvolvableRangeBody(json & entry)
{
  if (!entry.is_object()) return nullptr;
  if (entry.contains("evotag")) return &entry;
  if (entry.size() != 1) return nullptr;
  auto it = entry.begin();
  if (!it->is_object()) return nullptr;
  return &(*it);
}

const json * getEvolvableRangeBody(const json & entry)
{
  if (!entry.is_object()) return nullptr;
  if (entry.contains("evotag")) return &entry;
  if (entry.size() != 1) return nullptr;
  auto it = entry.begin();
  if (!it->is_object()) return nullptr;
  return &(*it);
}

int getEvolvableRangeTag(const json & entry)
{
  if (entry.is_object() && entry.contains("evotag"))
  {
    if (entry.at("evotag").is_number_integer()) return entry.at("evotag").get<int>();
    int evotag;
    if (entry.at("evotag").is_string()
      && parseEvoTagRangeKey(entry.at("evotag").get<string>(), evotag)) return evotag;
    return -1;
  }
  if (entry.is_object() && entry.size() == 1) {
    auto it = entry.begin();
    int evotag;
    if (parseEvoTagRangeKey(it.key(), evotag)) return evotag;
  }
  return -1;
}

string getEvolvableRangeTagString(const json & entry, int fallback)
{
  if (entry.is_object() && entry.contains("evotag")) {
    if (entry.at("evotag").is_string()) return entry.at("evotag").get<string>();
    if (entry.at("evotag").is_number_integer()) return makeEvoTagRangeKey(entry.at("evotag").get<int>());
  }
  if (entry.is_object() && entry.size() == 1) return entry.begin().key();
  return makeEvoTagRangeKey(fallback);
}

vector<intDoubDoub> getEvolvableRangeVals(const json & ranges)
{
  vector<intDoubDoub> vals;
  set<string> seenTags;
  int fallback = 1;

  auto addVal = [&](const json & entry, const string & tag) {
    intDoubDoub val = entry.template get<intDoubDoub>();
    val.tag = tag.empty() ? getEvolvableRangeTagString(entry, fallback) : tag;
    if (val.ind <= 0) val.ind = fallback;
    if (seenTags.find(val.tag) != seenTags.end())
      cout << "WARNING: duplicate evolvable range evotag '" << val.tag
           << "' found; evotag identifiers should be unique" << endl;
    seenTags.insert(val.tag);
    vals.push_back(val);
    fallback++;
  };

  if (ranges.contains("value") && ranges.at("value").is_array()) {
    const json & rangeVals = ranges.at("value");
    for (auto it = rangeVals.begin(); it != rangeVals.end(); ++it)
      addVal(*it, "");
    return vals;
  }

  if (ranges.is_object()) {
    vector<intDoubDoub> flatVals;
    for (auto it = ranges.begin(); it != ranges.end(); ++it) {
      if (it.key() == "value" || !it->is_object()) continue;
      intDoubDoub val = json{{it.key(), *it}}.template get<intDoubDoub>();
      val.tag = it.key();
      if (val.ind <= 0) val.ind = fallback;
      flatVals.push_back(val);
      fallback++;
    }

    stable_sort(flatVals.begin(), flatVals.end(),
      [](const intDoubDoub & a, const intDoubDoub & b) {
        const bool aNumeric = a.ind > 0;
        const bool bNumeric = b.ind > 0;
        if (aNumeric && bNumeric) return a.ind < b.ind;
        if (aNumeric != bNumeric) return aNumeric;
        return false;
      });

    for (int i=0; i<flatVals.size(); i++) {
      if (seenTags.find(flatVals[i].tag) != seenTags.end())
        cout << "WARNING: duplicate evolvable range evotag '" << flatVals[i].tag
             << "' found; evotag identifiers should be unique" << endl;
      seenTags.insert(flatVals[i].tag);
      vals.push_back(flatVals[i]);
    }
  }

  return vals;
}

vector<string> getEvolvedUsedTagOrder(const json & j)
{
  vector<string> tags;
  if (!j.contains("evolved_used")) return tags;

  const json & evolvedUsed = j.at("evolved_used");
  const json * values = nullptr;
  if (evolvedUsed.is_array()) values = &evolvedUsed;
  else if (evolvedUsed.is_object()
      && evolvedUsed.contains("value")
      && evolvedUsed.at("value").is_array())
    values = &evolvedUsed.at("value");

  if (values == nullptr) return tags;
  for (auto it = values->begin(); it != values->end(); ++it)
  {
    if (it->is_string()) tags.push_back(it->get<string>());
    else if (it->is_number_integer()) tags.push_back(makeEvoTagRangeKey(it->get<int>()));
  }
  return tags;
}

vector<intDoubDoub> orderEvolvableRangeVals(vector<intDoubDoub> vals,
  const vector<string> & tagOrder)
{
  if (tagOrder.empty()) return vals;

  vector<intDoubDoub> ordered;
  vector<bool> used(vals.size(), false);

  for (int i=0; i<tagOrder.size(); i++)
  {
    for (int j=0; j<vals.size(); j++)
    {
      if (used[j] || vals[j].tag != tagOrder[i]) continue;
      ordered.push_back(vals[j]);
      used[j] = true;
      break;
    }
  }

  for (int i=0; i<vals.size(); i++)
    if (!used[i]) ordered.push_back(vals[i]);

  return ordered;
}

vector<intDoubDoub> getEvolvableRangeValsFromJson(const json & j)
{
  const json * ranges = getEvolvableRanges(j);
  if (ranges == nullptr) return vector<intDoubDoub>(0);
  return orderEvolvableRangeVals(
    getEvolvableRangeVals(*ranges),
    getEvolvedUsedTagOrder(j));
}

void convertEvolvableRangesToKeyed(json & ranges)
{
  json converted = json::object();
  int fallback = 1;

  if (ranges.contains("value") && ranges["value"].is_array()) {
    for (auto it = ranges["value"].begin(); it != ranges["value"].end(); ++it) {
      string evotag = getEvolvableRangeTagString(*it, fallback);
      json * bodyPtr = getEvolvableRangeBody(*it);
      if (evotag.empty() || bodyPtr == nullptr) {
        fallback++;
        continue;
      }

      json body = *bodyPtr;
      body.erase("evotag");
      converted[evotag] = body;
      fallback++;
    }
    ranges = converted;
    return;
  }

  if (!ranges.is_object()) return;
  for (auto it = ranges.begin(); it != ranges.end(); ++it) {
    if (it.key() == "value" || !it->is_object()) continue;
    json body = *it;
    body.erase("evotag");
    converted[it.key()] = body;
  }
  ranges = converted;
}

json * getEvolvableRangeBodyByTag(json & ranges, const string & tag)
{
  if (ranges.contains("value") && ranges["value"].is_array()) {
    for (auto it = ranges["value"].begin(); it != ranges["value"].end(); ++it) {
      if (getEvolvableRangeTagString(*it, 1) != tag) continue;
      return getEvolvableRangeBody(*it);
    }
    return nullptr;
  }

  if (ranges.is_object() && ranges.contains(tag) && ranges[tag].is_object())
    return &ranges[tag];

  return nullptr;
}

void Worm2DSRE::addEvolvableToJson(json & j)
{
  
  const json * ranges = getEvolvableRanges(BPitsJson);
  if (ranges == nullptr) return;

  json rangesOut = *ranges;
  convertEvolvableRangesToKeyed(rangesOut);
  j[evolvableRangesKey] = rangesOut;
  addEvoNames(j);

  return;



}

vector<string> Worm2DSRE::getEvolvedUsedTags() const
{
  vector<string> tags;
  for (int i=0; i<genPhenLims.size(); i++)
  {
    if (genPhenLims[i].tag.size() > 0)
      tags.push_back(genPhenLims[i].tag);
    else
      tags.push_back(makeEvoTagRangeKey(genPhenLims[i].ind));
  }

  return tags;
}






void setEvoStr(vector<string> & vecval, const vector<string> & evoName)
{
  if (vecval.size() == 0) {vecval = evoName; return;}

  for (int i=0;i<evoName.size();i++)
  {
    bool hasVal = false;
    const string & sval1 = evoName[i];
    for (int j=0;j<vecval.size();j++) if (vecval[j]==sval1) {hasVal = true;break;}
    if (!hasVal) vecval.push_back(sval1);
  }

 
  return;


}

bool isJsonArrayIndexKey(const string & key)
{
  if (key.empty()) return false;
  for (int i=0;i<key.size();i++)
  {
    if (key[i] < '0' || key[i] > '9') return false;
  }
  return true;
}

vector<string> shortenEvoNamePath(const vector<string> & path)
{
  vector<string> shortened;
  for (int i=0;i<path.size();i++)
  {
    if (path[i] == "value") continue;
    if (path[i] == "stretch_receptor"
        && i + 1 < path.size()
        && path[i + 1].find("sr_") == 0) continue;

    string component = path[i];
    if (component == "nervous_system") component = "ns";
    else if (component == "chemical_conns") component = "chemcons";
    else if (component == "electrical_conns") component = "eleccons";
    else if (component == "dorsal_conns") component = "dorscons";
    else if (component == "ventral_conns") component = "ventcons";
    shortened.push_back(component);
  }
  return shortened;
}

void replaceAll(string & text, const string & from, const string & to)
{
  if (from.empty()) return;
  size_t pos = 0;
  while ((pos = text.find(from, pos)) != string::npos)
  {
    text.replace(pos, from.length(), to);
    pos += to.length();
  }
}

void shortenEvoName(string & evoName)
{
  replaceAll(evoName, "chemcons_weight", "chemcons");
  replaceAll(evoName, "eleccons_weight", "eleccons");
  replaceAll(evoName, "dorscons_", "");
  replaceAll(evoName, "_weight_ventcons_", "_");
}

string getEvotagString(const json & evotag)
{
  if (evotag.is_string()) return evotag.get<string>();
  if (evotag.is_number_integer()) return makeEvoTagRangeKey(evotag.get<int>());
  return "";
}

int getEvotagNumber(const json & evotag)
{
  if (evotag.is_number_integer()) return evotag.get<int>();
  if (evotag.is_string()) {
    int ind;
    if (parseEvoTagRangeKey(evotag.get<string>(), ind)) return ind;
  }
  return -1;
}

void setEvoNameFromTag(int evotag, vector<vector<string> > & evoNames, const vector<string> & path)
{
  if (evotag < 1 || evotag > evoNames.size()) return;
  setEvoStr(evoNames[evotag-1], shortenEvoNamePath(path));
}

void setEvoNameFromTag(const string & evotag, vector<vector<string> > & evoNames,
  const vector<string> & path, const vector<intDoubDoub> & vdd)
{
  for (int i=0; i<vdd.size(); i++)
    if (vdd[i].tag == evotag) {
      setEvoStr(evoNames[i], shortenEvoNamePath(path));
      return;
    }
}


void getEvoNames1(json::const_iterator it2, vector<vector<string> > & evoNames, 
  vector<string> & path)
{

  path.push_back(it2.key());
  //string keyval = "";
  //for (int i=0;i<path.size();i++) {keyval.append("_");keyval.append(path[i]);};
 
  

  //path.clear();

  if (it2->at("evolvable").is_object()){
    const json & j1 = it2->at("evolvable");
    int ind1 = getEvotagNumber(j1["evotag"]);
    setEvoNameFromTag(ind1, evoNames, path);
  }
  else if (it2->at("evolvable").is_number()){
  int ind1 = it2->at("evolvable").get<int>();
  //setEvoStr(evoNames[ind1-1],evoName);
  //setEvoStr(evoNames[ind1-1],it2.key());
  setEvoNameFromTag(ind1, evoNames, path);
  }
  else{
  size_t idx = it2.key().find("weights");
  if(idx != string::npos)
        {
          vector<fromToInt> evols = it2->at("evolvable").template get< vector<fromToInt> >();
          //for (int i = 0; i<evols.size();i++) setEvoStr(evoNames[evols[i].val],evoName);
          for (int i = 0; i<evols.size();i++) setEvoNameFromTag(evols[i].val, evoNames, path);
          
        }
  else
        {
          vector<intPair> evols =  from_evo_json(it2->at("evolvable"));
          //vector<intPair> evols =  it2->at("evolvable").template get< vector<intPair> >();
          //for (int i = 0; i<evols.size();i++) setEvoStr(evoNames[evols[i].val],evoName);
          for (int i = 0; i<evols.size();i++) setEvoNameFromTag(evols[i].val, evoNames, path);
                            
        }
  }

  path.pop_back();
 


}

void getEvoNamesFromEvotags(const json& j, vector<vector<string> > & evoNames, vector<string> & path,
  const vector<intDoubDoub> & vdd)
{
    for(auto it = j.begin(); it != j.end(); ++it)
    {
      const bool parentIsArray = j.is_array();
      const string key = parentIsArray ? "" : it.key();
      if (!parentIsArray
        && (key == evolvableRangesKey || key == legacyEvolvableKey || key == "evolvable")) continue;

      const bool hasEvotag = it->is_object() && it->contains("evotag");
      const bool pushKey = !parentIsArray
        && !isJsonArrayIndexKey(key)
        && !(path.size() > 0 && path[path.size()-1] == "cells" && !hasEvotag);
      if (pushKey) path.push_back(key);

      if (hasEvotag)
      {
        string evotag = getEvotagString(it->at("evotag"));
        setEvoNameFromTag(evotag, evoNames, path, vdd);
      }
      else if (it->is_object() || it->is_array())
      {
        getEvoNamesFromEvotags(*it, evoNames, path, vdd);
      }

      if (pushKey) path.pop_back();
    }
}

void getEvoNames(const json& j, vector<vector<string> > & evoNames, vector<string> & path,
  const vector<intDoubDoub> & vdd)
{
    getEvoNamesFromEvotags(j, evoNames, path, vdd);
    bool foundEvotagNames = false;
    for (int i=0;i<evoNames.size();i++)
    {
      if (!evoNames[i].empty()) {foundEvotagNames = true; break;}
    }
    if (foundEvotagNames) return;

    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->contains("evolvable")) getEvoNames1(it, evoNames, path);
      //else if (it->is_structured()) {
      else if (it->is_object()) {
      path.push_back(it.key());
      getEvoNames(*it, evoNames, path, vdd);
      path.pop_back();
      }
    }


    
}

string makeUniqueEvotagKey(const string & baseKey, set<string> & usedKeys,
  int startIndex)
{
  string key = baseKey;
  int suffix = startIndex;
  while (usedKeys.find(key) != usedKeys.end())
  {
    key = baseKey + "_" + to_string(suffix);
    suffix++;
  }
  usedKeys.insert(key);
  return key;
}

void renameEvotagReferences(json & j, const map<string, string> & tagMap)
{
  if (j.is_object())
  {
    if (j.contains("evotag") && j["evotag"].is_string())
    {
      auto found = tagMap.find(j["evotag"].get<string>());
      if (found != tagMap.end()) j["evotag"] = found->second;
    }

    for (auto it = j.begin(); it != j.end(); ++it)
      renameEvotagReferences(*it, tagMap);
  }
  else if (j.is_array())
  {
    for (auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->is_string())
      {
        auto found = tagMap.find(it->get<string>());
        if (found != tagMap.end()) *it = found->second;
      }
      else renameEvotagReferences(*it, tagMap);
    }
  }
}

void renameDefaultEvotagKeysToNames(json & j, const vector<intDoubDoub> & vdd)
{
  json * ranges = getEvolvableRanges(j);
  if (ranges == nullptr || !ranges->is_object()) return;

  map<string, int> defaultNameCounts;
  set<string> reservedKeys;

  for (auto it = ranges->begin(); it != ranges->end(); ++it)
  {
    if (!it->is_object()) continue;
    int evotagNumber;
    if (parseEvoTagRangeKey(it.key(), evotagNumber))
    {
      string defaultName = it->contains("name") && it->at("name").is_string()
        ? it->at("name").get<string>()
        : it.key();
      defaultNameCounts[defaultName]++;
    }
    else reservedKeys.insert(it.key());
  }

  json renamedRanges = json::object();
  map<string, string> tagMap;
  map<string, int> duplicateIndexes;
  set<string> usedKeys = reservedKeys;
  vector<string> evolvedUsed;

  for (int i=0; i<vdd.size(); i++)
  {
    const string & oldTag = vdd[i].tag;
    if (!ranges->contains(oldTag) || !ranges->at(oldTag).is_object()) continue;

    json body = ranges->at(oldTag);
    int evotagNumber;
    string newTag = oldTag;
    if (parseEvoTagRangeKey(oldTag, evotagNumber))
    {
      string baseKey = body.contains("name") && body.at("name").is_string()
        ? body.at("name").get<string>()
        : oldTag;
      if (defaultNameCounts[baseKey] > 1)
      {
        int & duplicateIndex = duplicateIndexes[baseKey];
        newTag = makeUniqueEvotagKey(
          baseKey + "_" + to_string(duplicateIndex), usedKeys, 0);
        duplicateIndex++;
      }
      else newTag = makeUniqueEvotagKey(baseKey, usedKeys, 0);
      tagMap[oldTag] = newTag;
    }
    else usedKeys.insert(oldTag);

    renamedRanges[newTag] = body;
    if (body.value("active", true)) evolvedUsed.push_back(newTag);
  }

  for (auto it = ranges->begin(); it != ranges->end(); ++it)
  {
    if (renamedRanges.contains(it.key()) || tagMap.find(it.key()) != tagMap.end())
      continue;
    renamedRanges[it.key()] = *it;
  }

  *ranges = renamedRanges;
  renameEvotagReferences(j, tagMap);
  j["evolved_used"]["value"] = evolvedUsed;
}


void addEvoNames(json & j)
{

  json * ranges = getEvolvableRanges(j);
  if (ranges == nullptr)  return;
  vector<intDoubDoub> vdd = getEvolvableRangeValsFromJson(j);

  vector<vector<string> > evoNames(vdd.size());
  
  vector<string> path;
  getEvoNames(j, evoNames, path, vdd);

  
  vector<string> evoKeys(vdd.size());
  for (int i=0;i<evoNames.size();i++)
  {
    if (evoNames[i].empty())
    {
      evoKeys[i] = "evolvable_" + to_string(i + 1);
      continue;
    }
    evoKeys[i] = "";
    for (int j=0;j<evoNames[i].size()-1;j++) 
    {evoKeys[i].append(evoNames[i][j]);evoKeys[i].append("_");}
    evoKeys[i].append(evoNames[i][evoNames[i].size()-1]);
    shortenEvoName(evoKeys[i]);
  }

  convertEvolvableRangesToKeyed(*ranges);
  json & j2 = *ranges;

  for(auto it = j2.begin(); it != j2.end(); ++it)
  {
    if (!it->is_object()) continue;
    json * body = &(*it);
    const string evotag = it.key();
    int phenind = -1;
    for (int i=0; i<vdd.size(); i++)
      if (vdd[i].tag == evotag) {phenind = i; break;}
    if (body == nullptr || phenind < 0 || phenind >= evoKeys.size()) continue;
    if (!body->contains("name")) (*body)["name"] = evoKeys[phenind];
    if (!body->contains("active")) (*body)["active"] = true;

  }

  renameDefaultEvotagKeysToNames(j, vdd);
}


vector<intDoubDoub> Worm2DSRE::makeVals()
{

  
  json & j = BPitsJson;
  //itsJson = j;

  json * ranges = getEvolvableRanges(j);
  if (ranges == nullptr) return vector<intDoubDoub>(0);

  vector<intDoubDoub> vdd = getEvolvableRangeValsFromJson(j);

  //addEvoNames(j);



  if(false){
  vector<vector<string> > evoNames(vdd.size());
  
  vector<string> path;
  getEvoNames(j, evoNames, path, vdd);

  
  vector<string> evoKeys(vdd.size());
  for (int i=0;i<evoNames.size();i++)
  { evoKeys[i] = "";
    for (int j=0;j<evoNames[i].size()-1;j++) 
    {evoKeys[i].append(evoNames[i][j]);evoKeys[i].append("_");}
    evoKeys[i].append(evoNames[i][evoNames[i].size()-1]);
  }

  json & j2 = (*ranges)["value"];

  for(auto it = j2.begin(); it != j2.end(); ++it)
  {
    if (!it->contains("name"))
    (*it)["name"] = evoKeys[getEvotagNumber(it->at("evotag"))-1];

  }

  }


  convertEvolvableRangesToKeyed(*ranges);
  vector<intDoubDoub> vddactive;
  //vector<bool> actives(vdd.size());
  //vector<int> inds(vdd.size());
  for(int i=0; i<vdd.size(); i++)
  {
    json * body = getEvolvableRangeBodyByTag(*ranges, vdd[i].tag);
    if (body == nullptr) continue;
    if (!body->contains("active")) (*body)["active"] = true;
    bool act1 = body->at("active").get<bool>();
    if (act1) vddactive.push_back(vdd[i]);
    //inds[i] = it->at("ind").get<int>();

  }


  return vddactive;


}

int getPhenind(const vector<intDoubDoub> & vdd, int indval)
{

  for(int i=0; i<vdd.size(); i++) 
  if (indval == vdd[i].ind) return i;
  return -1;

}

int getPhenind(const vector<intDoubDoub> & vdd, const string & tag)
{
  for(int i=0; i<vdd.size(); i++)
    if (tag == vdd[i].tag) return i;
  return -1;
}

int getPhenindFromEvotag(const vector<intDoubDoub> & vdd, const json & evotag)
{
  if (evotag.is_string()) return getPhenind(vdd, evotag.get<string>());
  if (evotag.is_number_integer()) return getPhenind(vdd, evotag.get<int>());
  return -1;
}

void setParsFromPheno1v2(const TVector<double> &pheno, json & it2, Efunctor & ef,
  const vector<intDoubDoub> & vdd)
{
 

  
  assert(it2.contains("value") && it2.at("value").is_number());
  //cout << it2 << endl;
  int phenind = getPhenindFromEvotag(vdd,it2.at("evotag")) + 1;
  if (phenind>0)
    if (it2.contains("mfunc")) 
    it2.at("value") = ef.eFunc(pheno[phenind], it2.at("mfunc"), true);
    else it2.at("value") = pheno[phenind];
  
  return;
}


void setParsFromPheno1(const TVector<double> &pheno, json::iterator it2, Efunctor & ef,
  const vector<intDoubDoub> & vdd)
{

      //cout << "evolvable " << it2->at("evolvable") << endl;


        const bool domfuncs = true;

        if (it2->at("evolvable").is_object())
        {
          const json & jevol = it2->at("evolvable");
          int phenind = getPhenindFromEvotag(vdd,jevol.at("evotag")) + 1;
          if (phenind>0){
          if (domfuncs && jevol.contains("mfunc"))
          it2->at("value") = ef.eFunc(pheno[phenind], jevol.at("mfunc"), true);
          else it2->at("value") = pheno[phenind];
          }
        }
        else if (it2->at("evolvable").is_number())
          it2->at("value") = pheno[it2->at("evolvable").get<int>()];
        else
        {
        size_t idx = it2.key().find("weights");
        if(idx != string::npos)
        {
          vector<toFromWeight> values = it2->at("value").template get< vector<toFromWeight> >();
          const json & jevol = it2->at("evolvable");
          for (auto itjevol = jevol.begin(); itjevol != jevol.end(); ++itjevol)
          for (int j = 0; j<values.size();j++)
          if (itjevol->at("from").get<int>() == values[j].w.from 
          && itjevol->at("to").get<int>()  == values[j].to)
          {
            int phenind = getPhenindFromEvotag(vdd,itjevol->at("evotag")) + 1;

            //int phenind = itjevol->at("evotag").get<int>();
            
            if (phenind<=0) break;
            if (domfuncs && itjevol->contains("mfunc"))
            values[j].w.weight = ef.eFunc(pheno[phenind], itjevol->at("mfunc"), true);
            else values[j].w.weight = pheno[phenind];


            break;
          } 

          it2->at("value") = values;
        }
        else
        {
          if (it2->at("value")[0].is_number())
        {
        vector<double> values = it2->at("value").template get< vector<double> >();
        vector<intPair> evols =  from_evo_json(it2->at("evolvable"));
        //vector<intPair> evols =  it2->at("evolvable").template get< vector<intPair> >();
        for (int i = 0; i<evols.size();i++) {
          int phenind = getPhenind(vdd, evols[i].val) + 1;
          //values[evols[i].ind-1] = pheno[evols[i].val];
          values[evols[i].ind-1] = pheno[phenind];

        }
        it2->at("value") = values;
        }
        else{

        vector<weightentry> values = it2->at("value").template get< vector<weightentry> >();
        vector<intPair> evols =  from_evo_json(it2->at("evolvable"));
        //vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++)
         for (int j = 0; j<values.size();j++)
          if (evols[i].ind == values[j].from)
          { 
            //cout << "ph " << it.key() << " " << it2.key() << endl;
            //cout << "ph " << pheno[evols[i].val-1] << " " <<  values[j].weight << endl;
            //assert(check123456(pheno[evols[i].val-1], values[j].weight));
            int phenind = getPhenind(vdd, evols[i].val) + 1;
            //values[j].weight = pheno[evols[i].val];
            values[j].weight = pheno[phenind];
            //pheno[evols[i].val-1] = values[j].weight;
            break;
          }

          it2->at("value") = values;
        }



      }
        }
      
}




void applyFuncable1v2(json::iterator it2, Efunctor & ef)
{

  assert(it2->contains("value") && it2->at("value").is_number());
  //cout << it2 << endl;
  it2->at("value") = ef.eFunc(it2->at("value"), it2->at("mfunc"));
   
  
  return;

}


void applyFuncable1(json::iterator it2, Efunctor & ef)
{

         //cout << it2->at("funcable") << endl;

        if (it2->at("funcable").is_object())
        {
          const json & jevol = it2->at("funcable");
          //int phenind = jevol.at("val").get<int>();
          it2->at("value") = ef.eFunc(it2->at("value"), jevol.at("mfunc"));
          //if (jevol.contains("mfunc"))
          //it2->at("value") = ef.eFunc(pheno[phenind], jevol.at("mfunc"));
          //else it2->at("value") = pheno[phenind];
           //cout << "applying func1" << endl;
      

        }
        else if (it2->at("funcable").is_number()){

             // cout << "applying func2" << endl;
       

        }
          //it2->at("value") = pheno[it2->at("funcable").get<int>()];
        else
        {
        size_t idx = it2.key().find("weights");
        if(idx != string::npos)
        {
          vector<toFromWeight> values = it2->at("value").template get< vector<toFromWeight> >();
          const json & jevol = it2->at("funcable");
          for (auto itjevol = jevol.begin(); itjevol != jevol.end(); ++itjevol)
          for (int j = 0; j<values.size();j++)
          if (itjevol->at("from").get<int>() == values[j].w.from 
          && itjevol->at("to").get<int>()  == values[j].to)
          {
               //     cout << "applying func3" << endl;
   



            //int phenind = itjevol->at("val").get<int>();
            values[j].w.weight = ef.eFunc(values[j].w.weight, itjevol->at("mfunc"));
            //if (itjevol->contains("mfunc"))
            //values[j].w.weight = ef.eFunc(pheno[phenind], itjevol->at("mfunc"));
            //else values[j].w.weight = pheno[phenind];
            break;
          } 

          it2->at("value") = values;
        }
        else
        {
          if (it2->at("value")[0].is_number())
        {

         //    cout << "applying func4" << endl;
        //vector<double> values = it2->at("value").template get< vector<double> >();
        //vector<intPair> evols =  it2->at("funcable").template get< vector<intPair> >();
        //for (int i = 0; i<evols.size();i++) values[evols[i].ind-1] = pheno[evols[i].val];
        //it2->at("value") = values;
        }
        else{
    //cout << "applying func5" << endl;

        /* vector<weightentry> values = it2->at("value").template get< vector<weightentry> >();
        vector<intPair> evols =  it2->at("funcable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++)
         for (int j = 0; j<values.size();j++)
          if (evols[i].ind == values[j].from)
          { 
            //cout << "ph " << it.key() << " " << it2.key() << endl;
            //cout << "ph " << pheno[evols[i].val-1] << " " <<  values[j].weight << endl;
            //assert(check123456(pheno[evols[i].val-1], values[j].weight));
            values[j].weight = pheno[evols[i].val];
            //pheno[evols[i].val-1] = values[j].weight;
            break;
          }

          it2->at("value") = values; */
        }



      }
        }
      

}


void recursive_iterate2(const TVector<double> & pheno, json& j, Efunctor & ef, const vector<intDoubDoub> & vdd)
{

    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->contains("evolvable")) setParsFromPheno1(pheno,it,ef, vdd);
      //else if (it->is_structured()) recursive_iterate2(pheno,*it);
      else if (it->is_object()) recursive_iterate2(pheno,*it,ef,vdd);
        
        //else if (it->contains("evolvable")) getInitGeno1(pheno,it);
        
    }
}

void recursive_iterate2v2(const TVector<double> & pheno, json& j, Efunctor & ef, const vector<intDoubDoub> & vdd)
{
    for (auto& [j_key, it] : j.items())
    { 
      //cout << j_key << endl;
      if (j_key == legacyEvolvableKey || j_key == evolvableRangesKey) continue;
      if (it.contains("evolvable")) continue;
     
      if (it.contains("evotag")) setParsFromPheno1v2(pheno,it,ef, vdd);
      //else if (it->is_object()) recursive_iterate2v2(pheno,*it,ef,vdd);
      else if (it.is_structured()) recursive_iterate2v2(pheno,it,ef,vdd);
        
  
        
    }

 //assert(0);

}

void recursive_applyFuncablev2(json& j, Efunctor & ef)
{

    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->contains("funcable")) continue;
      if (it->contains("mfunc")) applyFuncable1v2(it,ef);
      else if (it->is_object()) recursive_applyFuncablev2(*it,ef);
        
    }
}

void recursive_applyFuncable(json& j, Efunctor & ef)
{

    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->contains("funcable")) applyFuncable1(it,ef);
      else if (it->is_object()) recursive_applyFuncable(*it,ef);
        
    }
}

void WormCO2DSR::applyFuncablesExt()
{

json itsJson2 = BPitsJson;

/* for (auto& el : j1_.items())
{
itsEf.itsJson[el.key()] = el.value();
}
 */

//itsEf.itsJson = j1_;

Worm2DSRE::applyFuncables(itsJson2);
const json & js1 = itsJson2;
resetFromJson(js1);
Sensor::setParsFromJson(js1);

}

void Worm2DSRE::applyFuncables()
{

applyFuncables(BPitsJson);
}

void Worm2DSRE::applyFuncables(json & j1_)
{

//recursive_applyFuncable(j1_, itsEf);
recursive_applyFuncablev2(j1_, itsEf);
}

void Worm2DSRE::applyScheduledFuncable(
    const int function_index, const bool has_condval, const int condval)
{
    NervousSystem * nervous_system =
        dynamic_cast<NervousSystem *>(n_ptr);
    TVector<double> states;
    TVector<double> past_states;
    if (nervous_system != nullptr)
    {
        states = nervous_system->states;
        past_states = nervous_system->paststates;
    }
    const vector<double> saved_external_inputs = externalInputs;

    Worm2Dbase::applyScheduledFuncable(
        function_index, has_condval, condval);
    if (!getCurrentPheno().empty())
        callEfcond();
    else
    {
        json transformed = BPitsJson;
        applyFuncables(transformed);
        resetScheduledFuncableFromJson(transformed);
    }

    if (nervous_system != nullptr)
    {
        for (int i = 1; i <= nervous_system->CircuitSize(); i++)
            nervous_system->SetNeuronState(i, states[i]);
        nervous_system->paststates = past_states;
    }
    externalInputs = saved_external_inputs;
}

void Worm2DSRE::resetScheduledFuncableFromJson(const json & j)
{
    resetFromJson(j);
}

void WormCO2DSR::resetScheduledFuncableFromJson(const json & j)
{
    resetFromJson(j);
    Sensor::setParsFromJson(j);
}



void Worm2DSRE::setParsFromPheno(const TVector<double> &pheno)
{

  setCurrentPheno(pheno);
  
  bool test = false;
  if (test){


  json j1 = BPitsJson;
  json j2 = BPitsJson;
  
  recursive_iterate2(pheno,j1,itsEf,genPhenLims);
  
  recursive_iterate2v2(pheno,j2,itsEf,genPhenLims);

  //const string s1 = "Stretch receptor";
  //const string s2 = "stretch_receptor";

  if (false){
  const string s1 = "VNC NMJ";
  const string s2 = "vnc_nmj";

  compare_numeric_values(j1[s1], BPitsJson[s1]);

  cout << "call1 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx xxxxxxxxxx " << endl;

  compare_numeric_values(j2[s2], BPitsJson[s2]);

  cout << "call2 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx xxxxxxxxxx " << endl;
  }
   
  std::vector<double> diffs1;
  std::vector<double> diffs2;

  collect_numeric_differences(j1, BPitsJson, diffs1);
  collect_numeric_differences(j2, BPitsJson, diffs2);

if (same_values_unordered(diffs1, diffs2))
{
    //std::cout << "The two JSON pairs have the same numerical differences\n";
}
else
{
    std::cout << "The two JSON pairs have different numerical differences\n";
    assert(0);
}

  BPitsJson = j1;

}
else
{
  //recursive_iterate2(pheno,BPitsJson,itsEf,genPhenLims);
  
  recursive_iterate2v2(pheno,BPitsJson,itsEf,genPhenLims);


}




  //applyFuncables();
  resetFromBPJson();
  
}

void WormCO2DSR::setParsFromPheno(const TVector<double> &pheno)
{
const json & js1 = BPitsJson;
Worm2DSRE::setParsFromPheno(pheno);
Sensor::setParsFromJson(js1);
    
}


void Worm2DSRE::setParsFromPheno_old(const TVector<double> &pheno)
{


  json & js1 = BPitsJson;

 for (auto it = js1.begin(); it != js1.end(); ++it)
    for (auto it2 = it->begin(); it2 != it->end(); ++it2)
      if (it2->contains("evolvable"))
      {
        if (it2->at("evolvable").is_number())
          it2->at("value") = pheno[it2->at("evolvable").get<int>()];
        else
        {
        size_t idx = it2.key().find("weights");
        if(idx != string::npos)
        {
          vector<toFromWeight> values = it2->at("value").template get< vector<toFromWeight> >();
          vector<fromToInt> evols = it2->at("evolvable").template get< vector<fromToInt> >();
          for (int i = 0; i<evols.size();i++)
          for (int j = 0; j<values.size();j++)
          if (evols[i].from == values[j].w.from && evols[i].to == values[j].to)
          {values[j].w.weight = pheno[evols[i].val];break;} 
          it2->at("value") = values;
        }
        else
        {
          if (it2->at("value")[0].is_number())
        {
        vector<double> values = it2->at("value").template get< vector<double> >();
        vector<intPair> evols =  from_evo_json(it2->at("evolvable"));
        //vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++) values[evols[i].ind-1] = pheno[evols[i].val];
        it2->at("value") = values;
        }
        else{

        vector<weightentry> values = it2->at("value").template get< vector<weightentry> >();
        vector<intPair> evols =  from_evo_json(it2->at("evolvable"));
        //vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++)
         for (int j = 0; j<values.size();j++)
          if (evols[i].ind == values[j].from)
          { 
            //cout << "ph " << it.key() << " " << it2.key() << endl;
            //cout << "ph " << pheno[evols[i].val-1] << " " <<  values[j].weight << endl;
            //assert(check123456(pheno[evols[i].val-1], values[j].weight));
            values[j].weight = pheno[evols[i].val];
            //pheno[evols[i].val-1] = values[j].weight;
            break;
          }

          it2->at("value") = values;
        }



      }
        }
      }


    NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
    setNSFromJson(js1,n);

    //if (js1["Nervous system"].contains("section sizes"))
    //  jsects = js1["Nervous system"]["section sizes"];

    //W2Dbaseparameters1b->setParsFromJson(js1["Worm"]);
   

    //if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);
    
    Worm2DSRb::setParsFromJson(js1);
    setMuscBodExt(js1);
    


    return;


}

void Worm2DSRE::writeOrigGen(shared_ptr<const CmdArgs> cmd)
{
vector<double> initGeno = getInitGeno();
writeOrigGen(cmd,initGeno);

}


void Worm2DSRE::writeOrigGen(shared_ptr<const CmdArgs> cmd, const vector<double> & initGeno)
{

  string directoryName = cmd->getArgVal("--folder","HJUYGYT");
  struct stat sb;
  if (stat(directoryName.c_str(), &sb) != 0) 
  {cout << "Directory doesn't exist." << endl;exit(1);}

    {ofstream BestIndividualFile;
    //bestVector = s.BestIndividual();
    BestIndividualFile.open(rename_file("EvoWJbest.gen.dat", directoryName));
    //BestIndividualFile.open(bestfilename);
    BestIndividualFile << setprecision(32);
    //vector<double> initGeno = getInitGeno();
    BestIndividualFile << initGeno[0];
    for (int i=1;i<initGeno.size();i++)
    BestIndividualFile << " " << initGeno[i];
    BestIndividualFile << endl;
    BestIndividualFile.close();}

    {ofstream BestIndividualFile;
    //bestVector = s.BestIndividual();
    BestIndividualFile.open(rename_file("EvoWJbest.phen.dat", directoryName));
    //BestIndividualFile.open(bestfilename);
    BestIndividualFile << setprecision(32);
    //vector<double> initGeno = getInitGeno();
    BestIndividualFile << current_pheno[0];
    for (int i=1;i<current_pheno.size();i++)
    BestIndividualFile << " " << current_pheno[i];
    BestIndividualFile << endl;
    BestIndividualFile.close();}




}



void getInitPhenoVals(vector<double> & pheno, json::const_iterator it2)
{

 
        if (it2->at("evolvable").is_object())
        {
          const json & jevol = it2->at("evolvable");
          int phenind = getEvotagNumber(jevol.at("evotag")) - 1; 
          pheno[phenind] = it2->at("value");
        }
        else if (it2->at("evolvable").is_number())
        pheno[it2->at("evolvable").get<int>()-1] = it2->at("value");
        else
        {
        size_t idx = it2.key().find("weights");
        if(idx != string::npos)
        {
          vector<toFromWeight> values = it2->at("value").template get< vector<toFromWeight> >();
        
          const json  & jevol = it2->at("evolvable");
          int iind = 0;
          for (auto itjevol = jevol.begin(); itjevol != jevol.end(); ++itjevol){
          for (int j = 0; j<values.size();j++)
          if (itjevol->at("from").get<int>() == values[j].w.from && 
          itjevol->at("to").get<int>() == values[j].to)
          {
            int phenind = getEvotagNumber(itjevol->at("evotag")) - 1;
            pheno[phenind] = values[j].w.weight;
            break;
          
          } 
        iind ++;
        }

        }
        else
        {
        if (it2->at("value")[0].is_number())
        {
        vector<double> values = it2->at("value").template get< vector<double> >();
        vector<intPair> evols =  from_evo_json(it2->at("evolvable"));
        //vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++) pheno[evols[i].val-1] = values[evols[i].ind-1];
        
        }
        else{          
        vector<weightentry> values = it2->at("value").template get< vector<weightentry> >();
        vector<intPair> evols =  from_evo_json(it2->at("evolvable"));
        //vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++)
         for (int j = 0; j<values.size();j++)
          if (evols[i].ind == values[j].from)
          { 
            pheno[evols[i].val-1] = values[j].weight;
            break;
          }
        }

        }
        
        }
      

}

void getInitGeno1(vector<double> & pheno, json::const_iterator it2, Efunctor & ef, 
  const vector<intDoubDoub> & vdd, bool debug)
{

 
  
//  if (it->contains("funcable")) applyFuncable1(it,ef);

        //const bool do_mfunc = false; //should be false because funcs are called in setparsfrompheno
        const bool do_mfunc = true;
        //const bool hasfuncable = it2->contains("funcable");

        if (it2->at("evolvable").is_object())
        {
          const json & jevol = it2->at("evolvable");
          int phenind = getPhenindFromEvotag(vdd,jevol.at("evotag"));// + 1;
          if (phenind>=0){
          double phenval;
          if (do_mfunc && jevol.contains("mfunc")){
            json j2 = jevol.at("mfunc");
            j2["doInverse"] = true;
            phenval = ef.eFunc(it2->at("value"), j2, true);
          }
          else phenval = it2->at("value");
        
          (debug && check123456(pheno[phenind], phenval));
          pheno[phenind] = phenval;

        }
        }
        else if (it2->at("evolvable").is_number()){
        int phenind = getPhenind(vdd,it2->at("evolvable").get<int>());
        if (phenind>=0){
        //if (check123456(pheno[it2->at("evolvable").get<int>()-1], it2->at("value")))
        //pheno[it2->at("evolvable").get<int>()-1] = it2->at("value");
        (debug && check123456(pheno[phenind], it2->at("value")));
        pheno[phenind] = it2->at("value");
          }
        }
  
        else
        {
        size_t idx = it2.key().find("weights");
        if(idx != string::npos)
        {
          vector<toFromWeight> values = it2->at("value").template get< vector<toFromWeight> >();
          //vector<fromToInt> evols = it2->at("evolvable").template get< vector<fromToInt> >();

          json jevol = it2->at("evolvable");
          int iind = 0;
          for (auto itjevol = jevol.begin(); itjevol != jevol.end(); ++itjevol){
          for (int j = 0; j<values.size();j++)
          if (itjevol->at("from").get<int>() == values[j].w.from && 
          itjevol->at("to").get<int>() == values[j].to)
          {
            //int phenind = itjevol->at("val").get<int>() - 1;
            int phenind = getPhenindFromEvotag(vdd,itjevol->at("evotag"));

            if (phenind>=0){
            double phenval;
            if (do_mfunc && itjevol->contains("mfunc")) {
              json j2 = itjevol->at("mfunc");
              j2["doInverse"] = true;
              //phenval = ef.eFunc(pheno[phenind], itjevol->at("mfunc"));
              phenval = ef.eFunc(values[j].w.weight, j2, true);
              //phenval = ef.eFunc(pheno[phenind], j2);
            }
            else phenval = values[j].w.weight;
          

            if (false){
            cout << "phvals " << iind << " " << j << " " 
            <<  getEvotagString(itjevol->at("evotag"))
            << " " << itjevol->at("from").get<int>() << " " <<  itjevol->at("to").get<int>()  << endl;
            //cout << "ph " << it.key() << " " << it2.key() << endl;
            //cout << "ph " << phenval << " " << pheno[phenind] << " " << values[j].w.weight << endl;

            }

            //assert(check123456(phenval, values[j].w.weight));
            (debug && check123456(pheno[phenind], phenval));
            pheno[phenind] = phenval;
            //if (check123456(phenval, values[j].w.weight)) pheno[phenind] = values[j].w.weight;
          }
            break;
          
          } 
        iind ++;
        }
        }
        else
        {
        if (it2->at("value")[0].is_number())
        {
        vector<double> values = it2->at("value").template get< vector<double> >();
        vector<intPair> evols =  from_evo_json(it2->at("evolvable"));
        //vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++) 
        {
          int phenind = getPhenind(vdd,evols[i].val);
             //cout << "ph " << it.key() << " " << it2.key() << endl;
            //cout << "ph " << pheno[evols[i].val-1] << " " <<  values[evols[i].ind-1] << endl;
          
          //  if (check123456(pheno[evols[i].val-1], values[evols[i].ind-1]))
          //pheno[evols[i].val-1] = values[evols[i].ind-1];

          if (phenind>=0){
          (debug && check123456(pheno[phenind], values[evols[i].ind-1]));
          pheno[phenind] = values[evols[i].ind-1];
          }

        }
        //values[evols[i].ind] = pheno[evols[i].val];
        //it2->at("value") = values;

        }
        else{          
        vector<weightentry> values = it2->at("value").template get< vector<weightentry> >();
        vector<intPair> evols =  from_evo_json(it2->at("evolvable"));
        //vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++)
         for (int j = 0; j<values.size();j++)
          if (evols[i].ind == values[j].from)
          { 
           // cout << "ph " << it.key() << " " << it2.key() << endl;
           // cout << "ph " << pheno[evols[i].val-1] << " " <<  values[j].weight << endl;

           int phenind = getPhenind(vdd,evols[i].val);
            if (phenind>=0){
            //if (check123456(pheno[evols[i].val-1], values[j].weight))
            //pheno[evols[i].val-1] = values[j].weight;
            (debug && check123456(pheno[phenind], values[j].weight));
            pheno[phenind] = values[j].weight;
            }
            break;}
        }

        }
        
        }
      


}

void getInitGeno1v2(vector<double> & pheno, const json & it2, Efunctor & ef,
  const vector<intDoubDoub> & vdd, bool debug)
{
  assert(it2.contains("value") && it2.at("value").is_number());

  int phenind = getPhenindFromEvotag(vdd, it2.at("evotag"));
  if (phenind >= 0) {
    double phenval;
    if (it2.contains("mfunc")) {
      json j2 = it2.at("mfunc");
      j2["doInverse"] = true;
      phenval = ef.eFunc(it2.at("value"), j2, true);
    }
    else phenval = it2.at("value");

    (debug && check123456(pheno[phenind], phenval));
    pheno[phenind] = phenval;
  }
}

void recursive_iterate_old(vector<double> & pheno, const json& j, Efunctor & ef,
  const vector<intDoubDoub> & vdd, bool debug)
{

    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->contains("evolvable")) getInitGeno1(pheno,it,ef,vdd,debug);
      //else if (it->is_structured()) recursive_iterate(pheno,*it);
      else if (it->is_object()) recursive_iterate_old(pheno,*it,ef,vdd, debug);

        //else if (it->contains("evolvable")) getInitGeno1(pheno,it);
        
    }
}

void recursive_iterate(vector<double> & pheno, const json& j, Efunctor & ef,
  const vector<intDoubDoub> & vdd, bool debug)
{

    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (j.is_object()) {
        const string key = it.key();
        if (key == legacyEvolvableKey || key == evolvableRangesKey || key == "evolvable") continue;
      }
      if (it->contains("evolvable")) continue;

      if (it->contains("evotag") && it->contains("value")) getInitGeno1v2(pheno,*it,ef,vdd,debug);
      else if (it->is_structured()) recursive_iterate(pheno,*it,ef,vdd, debug);
        
    }
}

void recursive_iterate_pheno(vector<double> & pheno, const json& j)
{

    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->contains("evolvable")) getInitPhenoVals(pheno,it);
      else if (it->is_object()) recursive_iterate_pheno(pheno,*it);
    }
}


/* const vector<double> & Worm2DSRE::getCurrentPheno()
{
  setInitPheno();
  return current_pheno;
} */

void Worm2DSRE::setInitPheno()
{
  
  const double checkval = 123456;
  vector<double> pheno(getVectSize(), checkval); 

  const json & js1 = BPitsJson;
  recursive_iterate(pheno,js1,itsEf,genPhenLims, baseconsts.debug); //this tries to recreate pheno from actual values

  bool noNewStyleValues = true;
  for (int i = 0; i< pheno.size(); i++){
    if (!check123456(pheno[i])) {
      noNewStyleValues = false;
      break;
    }
  }
  if (noNewStyleValues) recursive_iterate_old(pheno,js1,itsEf,genPhenLims, baseconsts.debug);

  //recursive_iterate_pheno(pheno,js1); //this sets pheno as current parameter values not pheno

  for (int i = 0; i< pheno.size(); i++){
  //cout << "pheno i " << pheno[i] << " " << i << endl;
  assert(!check123456(pheno[i]) && "init pheno not set");
  }

  setCurrentPheno(pheno); //this sets current parameter values not pheno
  
}

vector<double> Worm2DSRE::getInitGeno()
{

  //setInitPheno();
  
  vector<double> initialGeno(getVectSize());
  //initialGeno.resize(getVectSize());
  PhenGenMapping(initialGeno, current_pheno);

  return initialGeno;

}








void Worm2DSRE::PhenGenMapping(vector<double> &gen, const vector<double> &phen)
{

  for (int i = 0; i<genPhenLims.size(); i++)
  {
    
  assert(genPhenLims[i].val1<=genPhenLims[i].val2);
  gen[i] = InverseMapSearchParameterGPT(phen[i], genPhenLims[i].val1, genPhenLims[i].val2);
  //cout << "phengen " << phen[i] << " " << genPhenLims[i].val1 << " " << genPhenLims[i].val2 << endl;
  assert(!isnan(gen[i]));
  }

}


void Worm2DSRE::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{

  for (int i = 0; i<genPhenLims.size(); i++)
  {
  assert(genPhenLims[i].val1<=genPhenLims[i].val2);
  phen(i+1) = MapSearchParameter(gen(i+1), genPhenLims[i].val1, genPhenLims[i].val2);
  }

}

void Worm2DSRE::testJson(json & j)
{

  vector<doubDoub> vec;
  vec.push_back({3.0,4.0});
  vec.push_back({-1.0,2.0});
  vec.push_back({-10.0,5.0});
  j[evolvableRangesKey] = toEvolvableRangesJson(vec); 

  {vector<fromToInt> vec;
  vec.push_back({1,3,1});
  vec.push_back({3,4,2});
  j["Dorsal NMJ"]["weights"]["evolvable"] = vec;
  j["Nervous system"]["Chemical weights"]["evolvable"] = vec;
}

  {vector<intPair> vec;
  vec.push_back({1,3});
  vec.push_back({3,3});
  j["Nervous system"]["biases"]["evolvable"] = vec;
  }

}

void Worm2DSRE::setEvolPars(W2Dparameters & w2par_, string evotype_)
{
    if (evotype_=="Evo21" || evotype_=="Evo21R"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(w2par_);

    Epars1.dbunit = 10;
    Epars1.vbunit = 13;
    }

}

void Sensor::InitializeSensors(RandomState &rs_)
{

  InitialiseAgent();
	
	ResetChemCon();
	//InitializeState(rs_);

	//ResetAgentIntState(rs_);
	UpdateChemCon();

}


void WormCO2DSR::initForSimulation(RandomState &rs_)
//void WormAgent::InitializeSimulation(RandomState &rs_)
{

  //return;
  Worm2DSRE::initForSimulation(rs_);
	//rs = &rs_;
  Sensor::ResetAgentsBody();
  InitializeSensors(rs_);
  
	
}



/* void WormCO2DSR::ResetAgentIntState(RandomState &rs)
{
	NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
	n.RandomizeCircuitState(0.0, 0.0, rs);
	//n.RandomizeCircuitState(0.0, 0.5, rs);
}
 */


void WormCO2DSR::InitializeState(RandomState &rs)
{

  Worm2DSRE::InitializeState(rs);

  //return;
 /*  if (false){

	NervousSystem * n = dynamic_cast<NervousSystem*>(n_ptr);

	if (n!=nullptr){

  bool randomInitialState;
  getValCJWorm<bool>("random_initial_state",randomInitialState);
  if (randomInitialState)
	//if (W2Dbaseparameters1->randomInitialState)
    {
        n->RandomizeCircuitState(-1, 1, rs);
        n->RandomizeCircuitOutput(0.2, 0.8, rs);
    }
	//else n->RandomizeCircuitState(0.0, 0.0, rs);
  }
  } */



}




void Sensor::InitialiseAgent()
{
  const double HSStepSize = wb.itsStepSize();
  for (SensorPars & sensor : spvec)
  {
    sensor.HSStepSize = HSStepSize;
    sensor.iSensorN = static_cast<int>(sensor.sensorN/sensor.HSStepSize);
    sensor.iSensorM = static_cast<int>(sensor.sensorM/sensor.HSStepSize);
  }
}


void WormCO2DSR::Step1()
{
   
    //UpdateSensors();
	Worm2DSRE::Step1();
  UpdateChemCon();
   
}

void EnvironmentPars::setParsFromJson(const string & key, const json & j)
{
  name = key;
  if (j.contains("name")) name = j.at("name").at("value").get<string>();
  x_center = j.at("x_center").at("value").get<double>();
  y_center = j.at("y_center").at("value").get<double>();
  gradSteep = j.at("grad_steep").at("value").get<double>();
}

void EnvironmentPars::writeParsToJson(json & j) const
{
  j["name"]["value"] = name;
  j["x_center"]["value"] = x_center;
  j["y_center"]["value"] = y_center;
  j["grad_steep"]["value"] = gradSteep;
}

void SensorPars::writeParsToJson2(json & j) const
{
  addParsToJson1<double>(j,{"sensor_n","sensor_m"}, {sensorN,sensorM});
  j["environment"]["value"] = environmentName;
  j["outputs"]["message"] =
    "Available sensor output names for sensor-to-cell connections";
  j["outputs"]["value"] = {
    {
      {"name", "output_1"},
      {"description",
       "Positive change in sensed concentration (present average above past average)"}
    },
    {
      {"name", "output_2"},
      {"description",
       "Negative change in sensed concentration (past average above present average)"}
    }
  };
  j.erase("ext_inp_1");
  j.erase("ext_inp_2");
  j.erase("hs_stepsize");
}

void SensorPars::writeParsToJson(json & j) const
{
  addParsToJson1<double>(j,{"sensorN","sensorM"}, {sensorN,sensorM});
  j.erase("HSStepSize");
  addParsToJson1<int>(j,{"extInp1", "extInp2"}, {extInp1, extInp2});
  j["environment"]["value"] = environmentName;
}

void SensorPars::setParsFromJson2(const json & j)
{
  sensorN = j["sensor_n"]["value"];
  sensorM = j["sensor_m"]["value"];
  if (j.contains("hs_stepsize")) HSStepSize = j["hs_stepsize"]["value"];
  if (j.contains("ext_inp_1")) extInp1 = j["ext_inp_1"]["value"];
  if (j.contains("ext_inp_2")) extInp2 = j["ext_inp_2"]["value"];
  if (j.contains("environment"))
    environmentName = j["environment"]["value"].get<string>();
}

void SensorPars::setParsFromJson(const json & j)
{
  sensorN = j["sensorN"]["value"];
  sensorM = j["sensorM"]["value"];
  if (j.contains("HSStepSize")) HSStepSize = j["HSStepSize"]["value"];
  extInp1 = j["extInp1"]["value"];
  extInp2 = j["extInp2"]["value"];
  if (j.contains("environment"))
    environmentName = j["environment"]["value"].get<string>();
}

void Sensor::setParsFromJson(const json & j)
{
  spvec.clear();
  environmentVec.clear();
  construct(j);
  InitialiseAgent();
  ResetChemCon();
  UpdateChemCon();
}

EnvironmentPars & Sensor::getEnvironment(const string & name)
{
  for (EnvironmentPars & environment : environmentVec)
    if (environment.name == name) return environment;
  throw runtime_error("Sensor refers to unknown environment '" + name + "'");
}

const EnvironmentPars & Sensor::getEnvironment(const string & name) const
{
  for (const EnvironmentPars & environment : environmentVec)
    if (environment.name == name) return environment;
  throw runtime_error("Sensor refers to unknown environment '" + name + "'");
}

void Sensor::construct(const json & j)
{
  int nextHiddenInput = 0;
  if (j.contains("driving_inputs")
      && j.at("driving_inputs").contains("inputs")
      && j.at("driving_inputs").at("inputs").contains("value"))
  {
    for (const auto & input :
         j.at("driving_inputs").at("inputs").at("value"))
      nextHiddenInput = max(
        nextHiddenInput, input.at("input_num").get<int>());
  }
  else if (j.contains("Driving input")
           && j.at("Driving input").contains("strengths")
           && j.at("Driving input").at("strengths").contains("value"))
  {
    nextHiddenInput = static_cast<int>(
      j.at("Driving input").at("strengths").at("value").size());
  }

  if (j.contains("environments"))
  {
    for (const auto & item : j.at("environments").items())
    {
      EnvironmentPars environment;
      environment.setParsFromJson(item.key(), item.value());
      for (const EnvironmentPars & existing : environmentVec)
        if (existing.name == environment.name)
          throw runtime_error("Duplicate environment name '" + environment.name + "'");
      environmentVec.push_back(environment);
    }
  }

  if (j.contains("sensors"))
  {
    const json & sensors = j["sensors"];
    int ind = 1;
    for (const auto & item : sensors.items())
    {
      const json & sensorJson = item.value();
      SensorPars sensor;
      sensor.name = item.key();
      sensor.setParsFromJson2(sensorJson);
      if (sensor.extInp1 < 0 || sensor.extInp2 < 0)
      {
        sensor.extInp1 = nextHiddenInput++;
        sensor.extInp2 = nextHiddenInput++;
      }
      if (sensor.environmentName.empty())
      {
        EnvironmentPars environment;
        environment.name = "environment_" + to_string(ind);
        environment.x_center = sensorJson.at("x_center").at("value");
        environment.y_center = sensorJson.at("y_center").at("value");
        environment.gradSteep = sensorJson.at("grad_steep").at("value");
        environmentVec.push_back(environment);
        sensor.environmentName = environment.name;
      }
      getEnvironment(sensor.environmentName);
      spvec.push_back(sensor);
      ind++;
    }
  }
  else if (j.contains("Sensors"))
  {
    const json & sensors = j["Sensors"];
    int ind = 1;
    while(sensors.contains("Sensor_" + to_string(ind)))
    {
      const json & sensorJson = sensors["Sensor_" + to_string(ind)];
      SensorPars sensor;
      sensor.name = "sensor_" + to_string(ind);
      sensor.setParsFromJson(sensorJson);
      if (sensor.environmentName.empty())
      {
        EnvironmentPars environment;
        environment.name = "environment_" + to_string(ind);
        environment.x_center = sensorJson.at("x_center").at("value");
        environment.y_center = sensorJson.at("y_center").at("value");
        environment.gradSteep = sensorJson.at("gradSteep").at("value");
        environmentVec.push_back(environment);
        sensor.environmentName = environment.name;
      }
      getEnvironment(sensor.environmentName);
      spvec.push_back(sensor);
      ind++;
    }
  }
  else if ((j.contains("worm") ? j.at("worm") : j.at("Worm")).contains("sensorM"))
  {
    const json & worm = j.contains("worm") ? j.at("worm") : j.at("Worm");
    SensorPars sensor;
    sensor.HSStepSize = wb.itsStepSize();
    sensor.extInp1 = 0;
    sensor.extInp2 = 1;
    sensor.sensorM = worm["sensorM"]["value"];
    sensor.sensorN = worm["sensorN"]["value"];
    sensor.environmentName = "environment_1";

    EnvironmentPars environment;
    environment.name = sensor.environmentName;
    environment.x_center = 0;
    environment.y_center = 0;
    wb.getValCJWorm<double>("grad_steep",environment.gradSteep);
    environmentVec.push_back(environment);
    spvec.push_back(sensor);
  }
}

void  Sensor::addParsToJson(json & j) const
{
  if (spvec.empty())
  {
    j.erase("sensors");
    j.erase("Sensors");
    if (j.contains("worm"))
    {
      j["worm"].erase("sensorM");
      j["worm"].erase("sensorN");
    }
    return;
  }

  vector<string> cellNames;
  if (j.contains("nervous_system")
      && j.at("nervous_system").contains("cell_names")
      && j.at("nervous_system").at("cell_names").contains("value"))
    cellNames = j.at("nervous_system").at("cell_names").at("value").
      template get<vector<string>>();
  else
    cellNames = wb.getDistinctCellNames();

  const json drivingWeights =
    j.contains("driving_inputs")
    && j.at("driving_inputs").contains("weights")
    && j.at("driving_inputs").at("weights").contains("value")
      ? j.at("driving_inputs").at("weights").at("value")
      : json::array();

  set<int> sensorInputNumbers;
  json oldSensors = j.contains("sensors") ? j.at("sensors") : json::object();
  j["sensors"] = json::object();
  json & environmentsJson = j["environments"];
  for (int i = 0; i<spvec.size(); i++)
  {
    const SensorPars & sensor = spvec[i];
    const string sensorName =
      sensor.name.empty() ? "sensor_" + to_string(i+1) : sensor.name;
    json & sensorJson = j["sensors"][sensorName];
    if (oldSensors.contains(sensorName) && oldSensors.at(sensorName).is_object())
      sensorJson = oldSensors.at(sensorName);
    const json previousWeights =
      sensorJson.contains("weights")
      && sensorJson.at("weights").contains("value")
        ? sensorJson.at("weights").at("value")
        : json::array();
    json & environmentJson = environmentsJson[sensor.environmentName];
    for (const string key : {"x_center", "y_center", "grad_steep"})
      if (!environmentJson.contains(key) && sensorJson.contains(key))
        environmentJson[key] = sensorJson[key];

    sensor.writeParsToJson2(sensorJson);
    json newWeights = json::array();
    const int inputNumbers[2] = {sensor.extInp1 + 1, sensor.extInp2 + 1};
    sensorInputNumbers.insert(inputNumbers[0]);
    sensorInputNumbers.insert(inputNumbers[1]);

    for (const toFromWeight & connection : wb.itsExternalInputConn())
    {
      int output = 0;
      if (connection.w.from == inputNumbers[0]) output = 1;
      else if (connection.w.from == inputNumbers[1]) output = 2;
      else continue;

      if (connection.to < 1
          || static_cast<size_t>(connection.to) > cellNames.size())
        throw runtime_error("Sensor connection refers to an unknown cell index");
      const string & cellName = cellNames[connection.to - 1];

      json entry;
      for (const auto & oldEntry : previousWeights)
        if (oldEntry.value("from_output", 0) == output
            && oldEntry.value("to_cell", string()) == cellName)
        {
          entry = oldEntry;
          break;
        }
      if (entry.is_null())
        for (const auto & oldEntry : drivingWeights)
          if (oldEntry.value("from_input", 0) == connection.w.from
              && oldEntry.value("to_cell", string()) == cellName)
          {
            entry = oldEntry;
            entry.erase("from_input");
            break;
          }

      if (entry.is_null()) entry = json::object();
      entry["from_output"] = output;
      entry["to_cell"] = cellName;
      entry["weight"]["value"] = connection.w.weight;
      newWeights.push_back(entry);
    }
    sensorJson["weights"]["message"] =
      "Weights from sensor outputs to Nervous System cells";
    sensorJson["weights"]["value"] = newWeights;
    sensorJson.erase("x_center");
    sensorJson.erase("y_center");
    sensorJson.erase("grad_steep");
  }

  if (j.contains("driving_inputs"))
  {
    json remainingInputs = json::array();
    map<int, int> inputNumberMap;
    if (j.at("driving_inputs").contains("inputs")
        && j.at("driving_inputs").at("inputs").contains("value"))
      for (const auto & input :
           j.at("driving_inputs").at("inputs").at("value"))
      {
        const int oldNumber = input.at("input_num").get<int>();
        if (sensorInputNumbers.count(oldNumber)) continue;
        json newInput = input;
        const int newNumber = static_cast<int>(remainingInputs.size()) + 1;
        newInput["input_num"] = newNumber;
        inputNumberMap[oldNumber] = newNumber;
        remainingInputs.push_back(newInput);
      }

    json remainingWeights = json::array();
    for (const auto & connection : drivingWeights)
    {
      const int oldNumber = connection.at("from_input").get<int>();
      if (sensorInputNumbers.count(oldNumber)) continue;
      auto mapped = inputNumberMap.find(oldNumber);
      if (mapped == inputNumberMap.end()) continue;
      json newConnection = connection;
      newConnection["from_input"] = mapped->second;
      remainingWeights.push_back(newConnection);
    }

    if (remainingInputs.empty() && remainingWeights.empty())
      j.erase("driving_inputs");
    else
    {
      j["driving_inputs"]["inputs"]["value"] = remainingInputs;
      j["driving_inputs"]["weights"]["value"] = remainingWeights;
    }
  }
  j.erase("Driving input");

  for (const EnvironmentPars & environment : environmentVec)
  {
    json & environmentJson = environmentsJson[environment.name];
    environment.writeParsToJson(environmentJson);
  }
  j.erase("Sensors");
  if (j.contains("worm"))
  {
    j["worm"].erase("sensorM");
    j["worm"].erase("sensorN");
  }
}




void Sensor::ResetChemCon()
{ 
  for (int i = 0; i<spvec.size(); i++){
    
  SensorPars & sp1 = spvec[i];
  const EnvironmentPars & environment = getEnvironment(sp1.environmentName);
	double chemCon = -headDistanceToLocation(
    environment.x_center,environment.y_center) * environment.gradSteep;

	//pastCon = chemCon;
  sp1.chemConHistory.clear();
	//timer = iSensorN + iSensorM + 1;
	for (int i = 1; i <= sp1.iSensorN + sp1.iSensorM + 1; i++) sp1.chemConHistory.push_back(chemCon);
		//chemConHistory(i) = chemCon;
	sp1.presentAvgCon = chemCon * sp1.iSensorN;
	sp1.pastAvgCon = chemCon * sp1.iSensorM;

  }
}


void Sensor::UpdateChemCon()
{
	for (int i = 0; i<spvec.size(); i++){
    
  SensorPars & sp1 = spvec[i];
  const EnvironmentPars & environment = getEnvironment(sp1.environmentName);
	//pastCon = chemCon;
	double chemCon = -headDistanceToLocation(
    environment.x_center,environment.y_center) * environment.gradSteep;
  sp1.chemConHistory.push_back(chemCon);

  }
	//chemConHistory(timer) = chemCon;
	//timer += 1;
}

void Sensor::assignExternalInput(vector<double> & externalInputs)
{
  for (size_t i = 0; i < spvec.size(); i++){
  SensorPars & sp1 = spvec[i];

  const size_t requiredHistory =
    static_cast<size_t>(sp1.iSensorN + sp1.iSensorM + 1);
  if (sp1.iSensorN <= 0 || sp1.iSensorM <= 0
      || sp1.chemConHistory.size() < requiredHistory)
    throw runtime_error(
      "Invalid sensor_" + to_string(i + 1) + " history: size="
      + to_string(sp1.chemConHistory.size()) + ", iSensorN="
      + to_string(sp1.iSensorN) + ", iSensorM="
      + to_string(sp1.iSensorM));
  if (sp1.extInp1 < 0 || sp1.extInp2 < 0
      || static_cast<size_t>(sp1.extInp1) >= externalInputs.size()
      || static_cast<size_t>(sp1.extInp2) >= externalInputs.size())
    throw runtime_error(
      "Invalid external input index for sensor_" + to_string(i + 1)
      + ": ext_inp_1=" + to_string(sp1.extInp1)
      + ", ext_inp_2=" + to_string(sp1.extInp2)
      + ", input count=" + to_string(externalInputs.size()));

  double dSensorN = (double) sp1.iSensorN;
  double dSensorM = (double) sp1.iSensorM;
  sp1.presentAvgCon += sp1.chemConHistory[sp1.chemConHistory.size()-1] - 
  sp1.chemConHistory[sp1.chemConHistory.size()- sp1.iSensorN -1];
  sp1.pastAvgCon += sp1.chemConHistory[sp1.chemConHistory.size() - sp1.iSensorN - 1] 
  -  sp1.chemConHistory[sp1.chemConHistory.size()- sp1.iSensorN - sp1.iSensorM - 1];


	//presentAvgCon += chemConHistory(timer - 1) - chemConHistory(timer - iSensorN - 1);
	//pastAvgCon += chemConHistory(timer - iSensorN - 1) - chemConHistory(timer - iSensorN - iSensorM - 1);
	double tempDiff = (sp1.presentAvgCon/dSensorN) - (sp1.pastAvgCon/dSensorM);
	externalInputs[sp1.extInp1] = tempDiff > 0.0 ? tempDiff: 0.0;
	externalInputs[sp1.extInp2] =  tempDiff < 0.0 ? fabs(tempDiff): 0.0;

  }
}

void Sensor::setGradientSteepness(const double & gradSteep)
{
  for (EnvironmentPars & environment : environmentVec)
    environment.gradSteep = gradSteep;
}


void WormCO2DSR::assignExternalInput()
{
  //Worm2DSRE::assignExternalInput();
  Sensor::assignExternalInput(externalInputs);
}
