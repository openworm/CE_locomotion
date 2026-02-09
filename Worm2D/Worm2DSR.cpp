#include "Worm2DSR.h"
//#include "../neuromlLocal/c302ForW2D.h"

/* Worm2DSRm::Worm2DSRm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_):
//Worm2Dm(par1_, n_ptr_, new Muscles),Worm2D(par1_,n_ptr_),w2dsr_ptr(sr_ptr_){}
Worm2Dm(par1_, n_ptr_),Worm2D(par1_,n_ptr_),w2dsr_ptr(sr_ptr_){}*/

Worm2DSRb::Worm2DSRb(const json & j):w2dsr_ptr(getSR(j)){
  setParsFromJson(j);
}
Worm2DSRb::Worm2DSRb(shared_ptr<SR> sr_ptr_):w2dsr_ptr(sr_ptr_){}


Worm2DSR::Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_):
Worm2Dm(par1_, n_ptr_),Worm2D(par1_,n_ptr_),Worm2DSRb(sr_ptr_){} 


Worm2DSR::Worm2DSR(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSR(getJsonFromFile(jsonfilename_),cmd){}

Worm2DSRE::Worm2DSRE(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSRE(getJsonFromFile(jsonfilename_),cmd){}

Worm2DSRE::Worm2DSRE(const json & j, shared_ptr<const CmdArgs> cmd, bool callInit):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), shared_ptr<W2Dbaseparameters>(make_shared<W2Dbaseparameters>())),
  Worm2DSR(j,cmd),genPhenLims(makeVals(j)),itsJson(j)
  {
    if (callInit) writeOrigGen(cmd);
  }
  

Worm2DSR::Worm2DSR(const json & j, shared_ptr<const CmdArgs> cmd):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), shared_ptr<W2Dbaseparameters>(make_shared<W2Dbaseparameters>())),
  Worm2D(getIzqPars(j) ,nullptr), Worm2DSRb(j)
{

    bool do_nml =  cmd->getArgValInt("--donml",0);
    if (!do_nml){

    NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
    setNSFromJson(j,n);

    //if (j["Nervous system"].contains("section sizes"))
      //jsects = j["Nervous system"]["section sizes"];

    }

    W2Dbaseparameters1b->setParsFromJson(j["Worm"]);
    setWormPars(cmd);

    //if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);
   
    setMuscBodExt(j);
    //setUpMuscleConn(j);
    //setUpBodyConn(j);
    //makeExternalInputConnFromJson(j);

   

}

Worm2DSRm::Worm2DSRm(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSRm(getJsonFromFile(jsonfilename_), cmd){}

Worm2DSRm::Worm2DSRm(const json & j, shared_ptr<const CmdArgs> cmd):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), shared_ptr<W2DbaseparametersNML>(make_shared<W2DbaseparametersNML>()), 0),
 Worm2DSRb(j)
{

    W2Dbaseparameters1b->setParsFromJson(j["Worm"]);
    setWormPars(cmd);

    //if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);
   
    setBodExt(j);
    //setUpMuscleConn(j);
    //setUpBodyConn(j);
    //makeExternalInputConnFromJson(j);

}




void Worm2DSR::addParsToJson(json & j)
{
  NervousSystem* const n = dynamic_cast<NervousSystem*>(n_ptr);
  if (n!=nullptr){
  string nsHead = "Nervous system";
  appendAllNSJson(j[nsHead], *n);
  
  //j[nsHead]["section sizes"] = jsects;
  }
  Worm2D::addParsToJson(j);
  Worm2DSRb::addParsToJson(j);

}

void Worm2DSRm::addParsToJson(json & j)
{
 Worm2DSRb::addParsToJson(j);
  Worm2Dm::addParsToJson(j);
}

void Worm2DSRb::addParsToJson(json & j)
{
if (w2dsr_ptr!=nullptr) w2dsr_ptr->addParsToJson(j);

}

void  Worm2DSRb::setParsFromJson(const json & j){if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);}


shared_ptr<SR> Worm2DSRb::getSR(const json & j)
{

    if (j.contains("Stretch receptor")){
    
    if (j["Stretch receptor"]["Type"]["value"] == "SR18") return make_shared<SR18>();

    return make_shared<SRCE>(j["Stretch receptor"]["NSegs"]["value"],j["Stretch receptor"]["NStretch"]["value"]);

    }
    else return nullptr;

}

//json j;
//Worm2DSR w(j);

void Worm2DSR::Step1()
{
 
  zeroAllInputs();
  
  b.StepBody(settedStepSize);

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->updateAll(b);
  
  setExternalInput();
  //setExternalInputOrig();

  if (w2dsr_ptr!=nullptr) {w2dsr_ptr->incNS(*n_ptr);}
 
  n_ptr->EulerStep(settedStepSize);
 
  //setMuscleInput();

  if (W2Dbaseparameters1->doOrigMuscInput) setMuscleInputOrig();
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

    var1.parDoub.head = "Worm";
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

void Worm2DSRE::addEvolvableToJson(json & j)
{
  

  j["Evolvable"]["value"] = itsJson["Evolvable"]["value"];

  return;


/*   j["Evolvable"]["value"] =  genPhenPars.genPhenLims;
  for (int i = 0; i<genPhenPars.TFnames.size();i++)
  {
    json j2;
    j2 = genPhenPars.TFIvec[i];
    vector<string> TFi = genPhenPars.TFnames[i];
    TFi.push_back("evolvable");
    set_nested_json(j, TFi, j2);
  }
  for (int i = 0; i<genPhenPars.IPnames.size();i++)
  {
    json j2;
    j2 = genPhenPars.IPvec[i];
    vector<string> TFi = genPhenPars.IPnames[i];
    TFi.push_back("evolvable");
    set_nested_json(j, TFi, j2);
  }
   for (int i = 0; i<genPhenPars.singValnames.size();i++)
  {
    json j2;
    j2 = genPhenPars.singVals[i];
    vector<string> TFi = genPhenPars.singValnames[i];
    TFi.push_back("evolvable");
    set_nested_json(j, TFi, j2);
  }
 */
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

  /* vector<string> evoName2 = evoName;
  v.erase(v.begin() + 2);
  vector<string> vecval1 = vecval;
  for (int i=0;i<vecval1.size();i++)
    if (vecval1[i]==evoName[i]) continue;
  
  {
    for (int j = i;j<evoName.size();j++) vecval.push_back(evoName[j]);
    break;
  }
 */
  return;

  //if (vecval=="not_set") {vecval = evoName;return;}
  //else if (vecval==evoName) return;
  //cout << "evoName " << evoName << " " << vecval << endl;
  //assert(0);

}


void getEvoNames1(json::const_iterator it2, vector<vector<string> > & evoNames, 
  vector<string> & path)
{

  path.push_back(it2.key());
  //string keyval = "";
  //for (int i=0;i<path.size();i++) {keyval.append("_");keyval.append(path[i]);};
 
  

  //path.clear();

  if (it2->at("evolvable").is_number()){
  int ind1 = it2->at("evolvable").get<int>();
  //setEvoStr(evoNames[ind1-1],evoName);
  //setEvoStr(evoNames[ind1-1],it2.key());
  setEvoStr(evoNames[ind1-1],path);

  
  }
  else{
  size_t idx = it2.key().find("weights");
  if(idx != string::npos)
        {
          vector<fromToInt> evols = it2->at("evolvable").template get< vector<fromToInt> >();
          //for (int i = 0; i<evols.size();i++) setEvoStr(evoNames[evols[i].val],evoName);
          for (int i = 0; i<evols.size();i++) setEvoStr(evoNames[evols[i].val-1],path);
          
        }
  else
        {
      
          vector<intPair> evols =  it2->at("evolvable").template get< vector<intPair> >();
          //for (int i = 0; i<evols.size();i++) setEvoStr(evoNames[evols[i].val],evoName);
          for (int i = 0; i<evols.size();i++) setEvoStr(evoNames[evols[i].val-1],path);
                            
        }
  }

  path.pop_back();
 


}

void getEvoNames(const json& j, vector<vector<string> > & evoNames, vector<string> & path)
{
  
    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->contains("evolvable")) getEvoNames1(it, evoNames, path);
      //else if (it->is_structured()) {
      else if (it->is_object()) {
      path.push_back(it.key());
      getEvoNames(*it, evoNames, path);
      path.pop_back();
      }
    }


    
}


vector<doubDoub> Worm2DSRE::makeVals(const json & j)
{

  
  if (!j.contains("Evolvable")) return vector<doubDoub>(0);



  vector<doubDoub> vdd;
  try {

    auto v1 = j["Evolvable"]["value"].template get<vector<intDoubDoub>>();
    vdd = todoubDoub(v1);
   
  }
  catch (const json::type_error&) {
   
    auto v1 = j.at("name").template get<vector<doubDoub>>();
    vdd = v1;
    assert(0);
  }

  vector<vector<string> > evoNames(vdd.size());
  
  vector<string> path;
  getEvoNames(j, evoNames, path);

  
  vector<string> evoKeys(vdd.size());
  for (int i=0;i<evoNames.size();i++)
  { evoKeys[i] = "";
    for (int j=0;j<evoNames[i].size()-1;j++) 
    {evoKeys[i].append(evoNames[i][j]);evoKeys[i].append("_");}
    evoKeys[i].append(evoNames[i][evoNames[i].size()-1]);
  }

  json & j2 = itsJson["Evolvable"]["value"];

  for(auto it = j2.begin(); it != j2.end(); ++it)
  {
    if (!it->contains("name"))
    (*it)["name"] = evoKeys[it->at("ind").get<int>()-1];

  }

  //itsJson["evoNames"] = evoKeys;


  return vdd;


}


double eFunc(const double & val, const json & j)
{

  //cout << "eFunc " << " " << val << endl;

  if (j.at("f_ind") == 1) return val * j.at("fact").get<double>();
  assert(0);

}


void setParsFromPheno1(const TVector<double> &pheno, json::iterator it2)
{
 
        if (it2->at("evolvable").is_number())
          it2->at("value") = pheno[it2->at("evolvable").get<int>()];
        else
        {
        size_t idx = it2.key().find("weights");
        if(idx != string::npos)
        {
          vector<toFromWeight> values = it2->at("value").template get< vector<toFromWeight> >();
          json jevol = it2->at("evolvable");
          for (auto itjevol = jevol.begin(); itjevol != jevol.end(); ++itjevol)
          for (int j = 0; j<values.size();j++)
          if (itjevol->at("from").get<int>() == values[j].w.from 
          && itjevol->at("to").get<int>()  == values[j].to)
          {
            int phenind = itjevol->at("val").get<int>();
            if (itjevol->contains("mfunc"))
            values[j].w.weight = eFunc(pheno[phenind], itjevol->at("mfunc"));
            else values[j].w.weight = pheno[phenind];
            break;
          } 


       /*    vector<fromToInt> evols = it2->at("evolvable").template get< vector<fromToInt> >();
          for (int i = 0; i<evols.size();i++)
          for (int j = 0; j<values.size();j++)
          if (evols[i].from == values[j].w.from && evols[i].to == values[j].to)
          {values[j].w.weight = pheno[evols[i].val];break;}  */



          it2->at("value") = values;
        }
        else
        {
          if (it2->at("value")[0].is_number())
        {
        vector<double> values = it2->at("value").template get< vector<double> >();
        vector<intPair> evols =  it2->at("evolvable").template get< vector<intPair> >();
        for (int i = 0; i<evols.size();i++) values[evols[i].ind-1] = pheno[evols[i].val];
        it2->at("value") = values;
        }
        else{

        vector<weightentry> values = it2->at("value").template get< vector<weightentry> >();
        vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
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



void recursive_iterate2(const TVector<double> & pheno, json& j)
{

    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->contains("evolvable")) setParsFromPheno1(pheno,it);
      //else if (it->is_structured()) recursive_iterate2(pheno,*it);
      else if (it->is_object()) recursive_iterate2(pheno,*it);
        
        //else if (it->contains("evolvable")) getInitGeno1(pheno,it);
        
    }
}

void Worm2DSRE::setParsFromPheno(const TVector<double> &pheno)
{
  json & js1 = itsJson;
  recursive_iterate2(pheno,js1);
  
  NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
    setNSFromJson(js1,n);

    //if (js1["Nervous system"].contains("section sizes"))
    //  jsects = js1["Nervous system"]["section sizes"];

    W2Dbaseparameters1b->setParsFromJson(js1["Worm"]);
   

    //if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);
    
    Worm2DSRb::setParsFromJson(js1);
    setMuscBodExt(js1);
    
}

void WormCO2DSR::setParsFromPheno(const TVector<double> &pheno)
{

Worm2DSRE::setParsFromPheno(pheno);
Sensor::setParsFromJson(itsJson);
    
}


void Worm2DSRE::setParsFromPheno_old(const TVector<double> &pheno)
{


  json & js1 = itsJson;

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
        vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++) values[evols[i].ind-1] = pheno[evols[i].val];
        it2->at("value") = values;
        }
        else{

        vector<weightentry> values = it2->at("value").template get< vector<weightentry> >();
        vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
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

    W2Dbaseparameters1b->setParsFromJson(js1["Worm"]);
   

    //if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);
    
    Worm2DSRb::setParsFromJson(js1);
    setMuscBodExt(js1);
    


    return;


/* for (int i = 0; i<genPhenPars.TFnames.size(); i++)
{

const vector<string> & s1 = genPhenPars.TFnames[i];
const vector<fromToInt> & v1 = genPhenPars.TFIvec[i];
if (s1[0]=="Nervous system")
{
NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);

if (s1[1]=="Chemical weights")
for (int j = 0; j<v1.size(); j++)
n.SetChemicalSynapseWeight(v1[j].from, v1[j].to, pheno(v1[j].val)); //unity indices
else if (s1[1]=="Electrical weights")
for (int j = 0; j<v1.size(); j++)
n.SetElectricalSynapseWeight(v1[j].from, v1[j].to, pheno(v1[j].val)); //unity indices

}
else if (s1[0]=="Dorsal NMJ")
{
  directMuscEvo = true;
  if (s1[1]=="weights")
  for (int j = 0; j<v1.size(); j++) 
  for (int k = 0; k<dMuscConnvec.size(); k++)
  if ((v1[j].to == dMuscConnvec[k].to) && (v1[j].from == dMuscConnvec[k].w.from))
  {dMuscConnvec[k].w.weight = pheno(v1[j].val);break;}

}
else if (s1[0]=="Ventral NMJ")
{
  directMuscEvo = true;
  if (s1[1]=="weights")
  for (int j = 0; j<v1.size(); j++) 
  for (int k = 0; k<vMuscConnvec.size(); k++)
  if ((v1[j].to == vMuscConnvec[k].to) && (v1[j].from == vMuscConnvec[k].w.from))
  {vMuscConnvec[k].w.weight = pheno(v1[j].val);break;}

}

}

for (int i = 0; i<genPhenPars.IPnames.size(); i++)
{

  const vector<string> & s1 = genPhenPars.IPnames[i];
  const vector<intPair> & v1 = genPhenPars.IPvec[i];

if (s1[0]=="Nervous system")
{
NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
if (s1[1]=="biases")
for (int j = 0; j<v1.size(); j++)
n.SetNeuronBias(v1[j].ind, pheno(v1[j].val));
else if (s1[1]=="taus")
for (int j = 0; j<v1.size(); j++)
n.SetNeuronTimeConstant(v1[j].ind, pheno(v1[j].val));
else if (s1[1]=="gains")
for (int j = 0; j<v1.size(); j++)
n.SetNeuronGain(v1[j].ind, pheno(v1[j].val));
}
else if (s1[0]=="VNC NMJ" || s1[0] == "VNC 18")
{
assert(!directMuscEvo);
if (s1[1]=="D inds")
{
for (int j = 0; j<v1.size(); j++)
for (int k = 0; k<dorsinds.size(); k++)
if (v1[j].ind == dorsinds[k].from) {dorsinds[k].weight = pheno(v1[j].val);break;}
}
else if (s1[1]=="V inds")
{
for (int j = 0; j<v1.size(); j++)
for (int k = 0; k<ventinds.size(); k++)
if (v1[j].ind == ventinds[k].from) {ventinds[k].weight = pheno(v1[j].val);break;}
}

}

}

for (int i = 0; i<genPhenPars.singValnames.size(); i++)
{
const vector<string> & s1 = genPhenPars.singValnames[i];
const int & v1 = genPhenPars.singVals[i];
if (s1[0]=="VNC NMJ" || s1[0] == "VNC 18") assert(!directMuscEvo);
namedVars[s1[1]] = pheno(v1);
//doubVars.setVal(s1[1], pheno(v1));


}

//w2dsr_ptr->setWeights();
//w2dsr_ptr->setNSWeights(*this);



setMuscBodExt();

return; */


}


void Worm2DSRE::writeOrigGen(shared_ptr<const CmdArgs> cmd)
{

  string directoryName = cmd->getArgVal("--folder","HJUYGYT");
  struct stat sb;
  if (stat(directoryName.c_str(), &sb) != 0) 
  {cout << "Directory doesn't exist." << endl;exit(1);}

    ofstream BestIndividualFile;
    //bestVector = s.BestIndividual();
    BestIndividualFile.open(rename_file("EvoWJbest.gen.dat", directoryName));
    //BestIndividualFile.open(bestfilename);
    BestIndividualFile << setprecision(32);
    vector<double> initGeno = getInitGeno();
    BestIndividualFile << initGeno[0];
    for (int i=1;i<initGeno.size();i++)
    BestIndividualFile << " " << initGeno[i];
    BestIndividualFile << endl;
    BestIndividualFile.close();

}

bool check123456(double val, double val2)
{

if (val<123456.001 && val>123455.999) return true;
if (val==val2) return false;

assert(0);

}

bool check123456(double val)
{
return (val<123456.001 && val>123455.999);
}


void getInitGeno1(vector<double> & pheno, json::const_iterator it2)
{
 
        if (it2->at("evolvable").is_number()){
        //cout << "ph " << it.key() << " " << it2.key() << endl;
        cout << "ph " << pheno[it2->at("evolvable").get<int>()-1] << " " <<  it2->at("value") << endl;
        //assert(check123456(pheno[it2->at("evolvable").get<int>()-1], it2->at("value")));
        if (check123456(pheno[it2->at("evolvable").get<int>()-1], it2->at("value")))
        pheno[it2->at("evolvable").get<int>()-1] = it2->at("value");
        }
  //        it2->at("value") = pheno[it2->at("evolvable")];
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
            int phenind = itjevol->at("val").get<int>() - 1;
            double phenval;
            if (itjevol->contains("mfunc"))
              phenval = eFunc(pheno[phenind], itjevol->at("mfunc"));
            else phenval = pheno[phenind];

            cout << "phvals " << iind << " " << j << " " 
            <<  itjevol->at("val").get<int>()
            << " " << itjevol->at("from").get<int>() << " " <<  itjevol->at("to").get<int>()  << endl;
            //cout << "ph " << it.key() << " " << it2.key() << endl;
            cout << "ph " << phenval << " " << pheno[phenind] << " " << values[j].w.weight << endl;

            //assert(check123456(phenval, values[j].w.weight));
            if (check123456(phenval, values[j].w.weight)) pheno[phenind] = values[j].w.weight;
            break;
          
          } 
        iind ++;
        }

/* 
          for (int i = 0; i<evols.size();i++)
          for (int j = 0; j<values.size();j++)
          if (evols[i].from == values[j].w.from && evols[i].to == values[j].to)
          {
            cout << "phvals " << i << " " << j << " " 
            <<  evols[i].val << " " << evols[i].from << " " << evols[i].to << endl;
            //cout << "ph " << it.key() << " " << it2.key() << endl;
            cout << "ph " << pheno[evols[i].val-1] << " " << values[j].w.weight << endl;
            assert(check123456(pheno[evols[i].val-1], values[j].w.weight));
            pheno[evols[i].val-1] = values[j].w.weight;
            //values[j].w.weight = pheno[evols[i].val];
            break;
          } */
          
          
          //it2->at("value") = values;
        }
        else
        {
        if (it2->at("value")[0].is_number())
        {
        vector<double> values = it2->at("value").template get< vector<double> >();
        vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++) 
        {
             //cout << "ph " << it.key() << " " << it2.key() << endl;
            cout << "ph " << pheno[evols[i].val-1] << " " <<  values[evols[i].ind-1] << endl;
          if (check123456(pheno[evols[i].val-1], values[evols[i].ind-1]))
          pheno[evols[i].val-1] = values[evols[i].ind-1];
        }
        //values[evols[i].ind] = pheno[evols[i].val];
        //it2->at("value") = values;

        }
        else{          
        vector<weightentry> values = it2->at("value").template get< vector<weightentry> >();
        vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++)
         for (int j = 0; j<values.size();j++)
          if (evols[i].ind == values[j].from)
          { 
           // cout << "ph " << it.key() << " " << it2.key() << endl;
            cout << "ph " << pheno[evols[i].val-1] << " " <<  values[j].weight << endl;
            if (check123456(pheno[evols[i].val-1], values[j].weight))
            pheno[evols[i].val-1] = values[j].weight;
            break;}
        }

        }
        
        }
      


}

void recursive_iterate(vector<double> & pheno, const json& j)
{

    for(auto it = j.begin(); it != j.end(); ++it)
    {
      if (it->contains("evolvable")) getInitGeno1(pheno,it);
      //else if (it->is_structured()) recursive_iterate(pheno,*it);
      else if (it->is_object()) recursive_iterate(pheno,*it);

        //else if (it->contains("evolvable")) getInitGeno1(pheno,it);
        
    }
}



vector<double> Worm2DSRE::getInitGeno()
{

  const double checkval = 123456;
  vector<double> pheno(getVectSize(), checkval); 


  const json & js1 = itsJson;
  recursive_iterate(pheno,js1);


  
  for (int i = 0; i< pheno.size(); i++){
  cout << "pheno i " << pheno[i] << " " << i << endl;
  assert(!check123456(pheno[i]) && "init pheno not set");
  }

  

  vector<double> initialGeno(getVectSize());
  //initialGeno.resize(getVectSize());
  PhenGenMapping(initialGeno, pheno);

  return initialGeno;

}



vector<double> Worm2DSRE::getInitGeno_old()
{

const double checkval = 123456;
vector<double> pheno(getVectSize(), checkval); 


  json & js1 = itsJson;

 for (auto it = js1.begin(); it != js1.end(); ++it)
    for (auto it2 = it->begin(); it2 != it->end(); ++it2)
      if (it2->contains("evolvable"))
      {
        getInitGeno1(pheno, it2);

        if (false){
        if (it2->at("evolvable").is_number()){
        cout << "ph " << it.key() << " " << it2.key() << endl;
        cout << "ph " << pheno[it2->at("evolvable").get<int>()-1] << " " <<  it2->at("value") << endl;
        assert(check123456(pheno[it2->at("evolvable").get<int>()-1], it2->at("value")));
        pheno[it2->at("evolvable").get<int>()-1] = it2->at("value");
        }
  //        it2->at("value") = pheno[it2->at("evolvable")];
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
          {
            cout << "ph " << it.key() << " " << it2.key() << endl;
            cout << "ph " << pheno[evols[i].val-1] << " " << values[j].w.weight << endl;
            assert(check123456(pheno[evols[i].val-1], values[j].w.weight));
            pheno[evols[i].val-1] = values[j].w.weight;
            //values[j].w.weight = pheno[evols[i].val];
            break;} 
          //it2->at("value") = values;
        }
        else
        {
        if (it2->at("value")[0].is_number())
        {
        vector<double> values = it2->at("value").template get< vector<double> >();
        vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++) 
        {
             cout << "ph " << it.key() << " " << it2.key() << endl;
            cout << "ph " << pheno[evols[i].val-1] << " " <<  values[evols[i].ind-1] << endl;
          assert(check123456(pheno[evols[i].val-1], values[evols[i].ind-1]));
          pheno[evols[i].val-1] = values[evols[i].ind-1];
        }
        //values[evols[i].ind] = pheno[evols[i].val];
        //it2->at("value") = values;

        }
        else{          
        vector<weightentry> values = it2->at("value").template get< vector<weightentry> >();
        vector<intPair> evols =  it2->at("evolvable").template get<vector<intPair> >();
        for (int i = 0; i<evols.size();i++)
         for (int j = 0; j<values.size();j++)
          if (evols[i].ind == values[j].from)
          { 
            cout << "ph " << it.key() << " " << it2.key() << endl;
            cout << "ph " << pheno[evols[i].val-1] << " " <<  values[j].weight << endl;
            assert(check123456(pheno[evols[i].val-1], values[j].weight));
            pheno[evols[i].val-1] = values[j].weight;
            break;}
        }


        }
        
        }
      }

    }

for (int i = 0; i< pheno.size(); i++){
  cout << "pheno i " << pheno[i] << " " << i << endl;
assert(!check123456(pheno[i]) && "init pheno not set");
}

vector<double> initialGeno(getVectSize());
//initialGeno.resize(getVectSize());
PhenGenMapping(initialGeno, pheno);

return initialGeno;


}



/* vector<double> Worm2DSRE::getInitGeno_old()
{

const double checkval = 123456;
vector<double> initialPheno(getVectSize(), checkval); 


for (int i = 0; i<genPhenPars.TFnames.size(); i++)
{

const vector<string> & s1 = genPhenPars.TFnames[i];
const vector<fromToInt> & v1 = genPhenPars.TFIvec[i];
if (s1[0]=="Nervous system")
{
NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);

if (s1[1]=="Chemical weights")
for (int j = 0; j<v1.size(); j++) 
{
  assert(check123456(initialPheno[v1[j].val-1], n.ChemicalSynapseWeight(v1[j].from, v1[j].to)));
  initialPheno[v1[j].val-1] = n.ChemicalSynapseWeight(v1[j].from, v1[j].to);

}
else if (s1[1]=="Electrical weights")
for (int j = 0; j<v1.size(); j++)
{assert(check123456(initialPheno[v1[j].val-1], n.ElectricalSynapseWeight(v1[j].from, v1[j].to)));
  initialPheno[v1[j].val-1] = n.ElectricalSynapseWeight(v1[j].from, v1[j].to);

}

}
else if (s1[0]=="Dorsal NMJ")
{
  if (s1[1]=="weights")
  for (int j = 0; j<v1.size(); j++) 
  for (int k = 0; k<dMuscConnvec.size(); k++)
  if ((v1[j].to == dMuscConnvec[k].to) && (v1[j].from == dMuscConnvec[k].w.from))
  { assert(check123456(initialPheno[v1[j].val-1], dMuscConnvec[k].w.weight));
    initialPheno[v1[j].val-1] = dMuscConnvec[k].w.weight; 
    
    break;}
}
else if (s1[0]=="Ventral NMJ")
{
  if (s1[1]=="weights")
  for (int j = 0; j<v1.size(); j++) 
  for (int k = 0; k<vMuscConnvec.size(); k++)
  if ((v1[j].to == vMuscConnvec[k].to) && (v1[j].from == vMuscConnvec[k].w.from))
  { assert(check123456(initialPheno[v1[j].val-1], vMuscConnvec[k].w.weight));
    initialPheno[v1[j].val-1] = vMuscConnvec[k].w.weight; 
   
    break;}
}



}

for (int i = 0; i<genPhenPars.IPnames.size(); i++)
{
const vector<string> & s1 = genPhenPars.IPnames[i];
const vector<intPair> & v1 = genPhenPars.IPvec[i];
if (s1[0]=="Nervous system")
{
NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
if (s1[1]=="biases")
for (int j = 0; j<v1.size(); j++) 
{assert(check123456(initialPheno[v1[j].val-1], n.NeuronBias(v1[j].ind)));
  initialPheno[v1[j].val-1] = n.NeuronBias(v1[j].ind);} 

else if (s1[1]=="taus")
for (int j = 0; j<v1.size(); j++)
{assert(check123456(initialPheno[v1[j].val-1], n.NeuronTimeConstant(v1[j].ind)));
  initialPheno[v1[j].val-1] = n.NeuronTimeConstant(v1[j].ind);
}
else if (s1[1]=="gains")
for (int j = 0; j<v1.size(); j++)
{assert(check123456(initialPheno[v1[j].val-1], n.NeuronGain(v1[j].ind)));
  initialPheno[v1[j].val-1] = n.NeuronGain(v1[j].ind);
}

}
else if (s1[0]=="VNC NMJ")
{
if (s1[1]=="D inds")
{
for (int j = 0; j<v1.size(); j++)
for (int k = 0; k<dorsinds.size(); k++)
if (v1[j].ind == dorsinds[k].from)
{assert(check123456(initialPheno[v1[j].val-1], dorsinds[k].weight));
  initialPheno[v1[j].val-1] = dorsinds[k].weight;
  
  break;}
}
else if (s1[1]=="V inds")
{
for (int j = 0; j<v1.size(); j++)
for (int k = 0; k<ventinds.size(); k++)
if (v1[j].ind == ventinds[k].from)
{assert(check123456(initialPheno[v1[j].val-1], ventinds[k].weight));
  initialPheno[v1[j].val-1] = ventinds[k].weight;
  
  break;}
}

}

}

for (int i = 0; i<genPhenPars.singValnames.size(); i++)
{
const vector<string> & s1 = genPhenPars.singValnames[i];
const int & v1 = genPhenPars.singVals[i];
assert(check123456(initialPheno[v1-1], namedVars[s1[1]]));
initialPheno[v1 -1] = namedVars[s1[1]];
}



for (int i = 0; i< initialPheno.size(); i++)
assert(!check123456(initialPheno[i]) && "init pheno not set");

vector<double> initialGeno(getVectSize());
//initialGeno.resize(getVectSize());
PhenGenMapping(initialGeno, initialPheno);

return initialGeno;
} */


void Worm2DSRE::PhenGenMapping(vector<double> &gen, const vector<double> &phen)
{

  for (int i = 0; i<genPhenLims.size(); i++)
  {
  assert(genPhenLims[i].val1<=genPhenLims[i].val2);
  gen[i] = InverseMapSearchParameterGPT(phen[i], genPhenLims[i].val1, genPhenLims[i].val2);
  cout << "phengen " << phen[i] << " " << genPhenLims[i].val1 << " " << genPhenLims[i].val2 << endl;
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
  j["Evolvable"]["value"] = vec; 

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
	NervousSystem * n = dynamic_cast<NervousSystem*>(n_ptr);

	if (n!=nullptr){
	if (W2Dbaseparameters1->randomInitialState)
    {
        n->RandomizeCircuitState(-1, 1, rs);
        n->RandomizeCircuitOutput(0.2, 0.8, rs);
    }
	//else n->RandomizeCircuitState(0.0, 0.0, rs);
}

}




void Sensor::InitialiseAgent()
{
	//VelDelta = (int) (HST/gradPars->HSStepSize);

  if (spvec.size()>0){
  spvec[0].HSStepSize = CO2DSRpars->HSStepSize;
  spvec[0].gradSteep = CO2DSRpars->gradSteep;

  }

  for (int i = 0; i<spvec.size(); i++){

  SensorPars & sp1 = spvec[i];
	sp1.iSensorN = (int) (sp1.sensorN/sp1.HSStepSize);
	//dSensorN = (double) iSensorN;
	sp1.iSensorM = (int) (sp1.sensorM/sp1.HSStepSize);
  }


	//dSensorM = (double) iSensorM;
	//int upperbound = ((int) (((2*CO2DSRpars->RunDuration) + sensorN + sensorM) / CO2DSRpars->HSStepSize)) + 1;

	//cout << "uppervel " << upperbound << " " << VelDelta << endl;
 	
	//chemConHistory.SetBounds(1, upperbound);
	//chemConHistory.FillContents(0.0);

	//histCurv.SetBounds(1, VelDelta);
	//histCurv.FillContents(0.0);
	//histTheta.SetBounds(1, VelDelta);
	//histTheta.FillContents(0.0);
	
}


void WormCO2DSR::Step1()
{
   
    //UpdateSensors();
	Worm2DSRE::Step1();
  UpdateChemCon();
   
}


void SensorPars::writeParsToJson(json & j) const
{

addParsToJson1<double>(j,{"sensorN","sensorM","gradSteep", 
  "HSStepSize", "x_center", "y_center"},
    {sensorN,sensorM,gradSteep,HSStepSize,x_center,y_center});
addParsToJson1<int>(j,{"extInp1", "extInp2"}, {extInp1, extInp2});

}

void SensorPars::setParsFromJson(const json & j)
{

  sensorN = j["sensorN"]["value"];
  sensorM = j["sensorM"]["value"];
  gradSteep = j["gradSteep"]["value"];
  HSStepSize = j["HSStepSize"]["value"];
  x_center  = j["x_center"]["value"];
  y_center = j["y_center"]["value"];
  extInp1 = j["extInp1"]["value"];
  extInp2 = j["extInp2"]["value"];

//double sensorN, sensorM;
//double dSensorN, dSensorM;
//int iSensorN, iSensorM;
//double chemCon, presentAvgCon, pastAvgCon;
//double presentAvgCon, pastAvgCon;
//int extInp1, extInp2;
//double gradSteep, HSStepSize, x_center, y_center;

}

void Sensor::setParsFromJson(const json & j)
{

if (j.contains("Sensors")){
json j2 = j["Sensors"];
for (int i=0; i<spvec.size(); i++)
{
SensorPars & sp1 = spvec[i];
sp1.setParsFromJson(j2["Sensor_" + to_string(i+1)]);
}
}else if (j["Worm"].contains("sensorM"))
{

  SensorPars & sp1 = spvec[0];

  sp1.gradSteep = CO2DSRpars->gradSteep;
  sp1.HSStepSize = CO2DSRpars->HSStepSize;
  sp1.extInp1 = 0;
  sp1.extInp2 = 1;
  sp1.sensorM = j["Worm"]["sensorM"]["value"];
  sp1.sensorN = j["Worm"]["sensorN"]["value"];
  sp1.x_center = 0, sp1.y_center = 0;
 
}



}


void Sensor::construct(const json & j)
{


  if (j.contains("Sensors"))
 {
  json j2 = j["Sensors"];
  int ind = 1;
  while(j2.contains("Sensor_" + to_string(ind))){

  SensorPars sp1;
  sp1.setParsFromJson(j2["Sensor_" + to_string(ind)]);
  spvec.push_back(sp1);
  ind++;
  }
  
 }else if (j["Worm"].contains("sensorM"))
 {


  SensorPars sp1;
  sp1.gradSteep = CO2DSRpars->gradSteep;
  sp1.HSStepSize = CO2DSRpars->HSStepSize;
  sp1.extInp1 = 0;
  sp1.extInp2 = 1;
  sp1.sensorM = j["Worm"]["sensorM"]["value"];
  sp1.sensorN = j["Worm"]["sensorN"]["value"];
  sp1.x_center = 0, sp1.y_center = 0;
  spvec.push_back(sp1);

  
 }

 
}

void  Sensor::addParsToJson(json & j) const
{

if (spvec.size()<1) return;

json & j2 = j["Sensors"];

for (int i =0; i<spvec.size(); i++)
{

const SensorPars & sp1 = spvec[i];
sp1.writeParsToJson(j2["Sensor_" + to_string(i+1)]);

}
if (spvec.size()>0)
{
const SensorPars & sp1 = spvec[0];
sp1.writeParsToJson(j["Worm"]);

}


}




void Sensor::ResetChemCon()
{ 
  for (int i = 0; i<spvec.size(); i++){
    
  SensorPars & sp1 = spvec[i];
	double chemCon = -headDistanceToLocation(sp1.x_center,sp1.y_center) * sp1.gradSteep;

	//double dist = distanceToCenter();
	//chemCon = -dist * CO2DSRpars->gradSteep;

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
	//pastCon = chemCon;
	double chemCon = -headDistanceToLocation(sp1.x_center,sp1.y_center) * sp1.gradSteep;
  sp1.chemConHistory.push_back(chemCon);

  }
	//chemConHistory(timer) = chemCon;
	//timer += 1;
}

void Sensor::assignExternalInput(vector<double> & externalInputs)
{
  
  for (int i = 0; i<spvec.size(); i++){
    
   

  SensorPars & sp1 = spvec[i];

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


void WormCO2DSR::assignExternalInput()
{
  Worm2DSRE::assignExternalInput();
  Sensor::assignExternalInput(externalInputs);
}